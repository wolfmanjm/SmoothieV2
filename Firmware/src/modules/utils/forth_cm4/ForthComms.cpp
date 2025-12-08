#include "ForthComms.h"

#include "OutputStream.h"
#include "ConfigReader.h"
#include "StringUtils.h"
#include "main.h"
#include "MessageQueue.h"
#include "Dispatcher.h"

#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"

#include "stm32h7xx_hal.h"

#include <cstddef>
#include <cmath>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include <tuple>

#define enable_key "enable"

#define HELP(m) if(params == "-h") { os.printf("%s\n", m); return true; }

REGISTER_MODULE(ForthComms, ForthComms::create)

bool ForthComms::create(ConfigReader& cr)
{
    printf("DEBUG: configure forth comms\n");
    ForthComms *dc = new ForthComms();
    if(!dc->configure(cr)) {
        printf("INFO: ForthComms not enabled\n");
        delete dc;
    }
    return true;
}

ForthComms::ForthComms() : Module("forthcomms")
{
    terminal_connected= false;
}

bool ForthComms::configure(ConfigReader& cr)
{
    ConfigReader::section_map_t m;
    if(!cr.get_section("forthcomms", m)) {
        printf("INFO: configure-forthcomms: no forthcomms section found, presume disabled\n");
        return false;
    }

    // if the module is disabled -> do nothing
    if(!cr.get_bool(m, enable_key, false)) {
        return false;
    }

    // command handlers
    using std::placeholders::_1;
    using std::placeholders::_2;

    THEDISPATCHER->add_handler("fth", std::bind( &ForthComms::fth_command, this, _1, _2) );

    return true;
}

// main fth command handler, handle all the subcommands, partial subcommand names can be used, minimum to be unique
// fth flash
// fth terminal
// fth reset

#define FTH_COMMANDS "fth [flash fn] | [terminal] | [run word {params...}] | [load filename] | [reset]"

static const std::vector<std::string> sub_commands= {"flash", "terminal", "run", "load", "reset"};

bool ForthComms::fth_command( std::string& params, OutputStream& os )
{
    HELP(FTH_COMMANDS)

    std::string subcmd = stringutils::shift_parameter(params);
    std::string m = stringutils::match_command(sub_commands, subcmd);
    if(m.empty()) {
        os.printf("unknown subcommand. Usage: %s\n", FTH_COMMANDS);
        return true;
    }else if(m == "?") {
        os.printf("subcommand is not unique. Usage: %s\n", FTH_COMMANDS);
        return true;
    }

    if(m == "flash") {
        return flash(params, os);
    }

    if(m == "terminal") {
        return terminal(params, os);
    }

    os.printf("unknown subcommand: %s\n", m.c_str());

    return true;
}

static uint8_t forth_comms_buffer[256*2+4] __attribute__((section (".sram_4_shared"))); // put in SRAM_4
// stores the parameters for the task
static struct task_params_struct {
    bool done;
    OutputStream *os;
} task_params;

static void start_CM4()
{
    // not sure why this does not work so presume it aways needs starting, does no harm if it is already running
//    if(!__HAL_PWR_GET_FLAG(PWR_FLAG2_SB_D2)) {
        // printf("INFO: ForthComms: CM4 was not started...\n");
        HAL_SYSCFG_CM4BootAddConfig(SYSCFG_BOOT_ADDR0, 0x081C0000); /*0x081C0000*/
        // printf("INFO: ForthComms: Set CM4 boot address to %08X\n",  0x081C0000);
        /* Enable CPU2 (Cortex-M4) boot regardless of option byte values */
        HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);
        // printf("INFO: ForthComms: Told CM4 to boot\n");
    // } else {
    //      printf("INFO: ForthComms: CM4 was already started\n");
    // }
}

// This is a task
// NOTE It is possible the os will go away if the USB detaches. Need to handle that (like the network shell).
static void terminal_thread(void *params)
{
    struct task_params_struct *p = (struct task_params_struct*)params;
    OutputStream *os = p->os;
    uint8_t *addr = forth_comms_buffer;
    //uint8_t *addr = (uint8_t *)0x38000000;
    printf("DEBUG: ForthComms: Terminal thread starting. comms buffer is at: %p\n", addr);

    while(!p->done) {
        if(os->is_closed()) break;

        // consume
        uint8_t rx_w = addr[2];
        uint8_t rx_r = addr[3];
        uint8_t rx_u = rx_w - rx_r;

        if (rx_u > 0) {
            if(rx_w > rx_r) {
                // simple case where the chars are consecutive in the buffer
                os->write((const char *)addr + 4 + 256 + rx_r, rx_u);

            } else {
                // buffer wrapped, so chars are split between the top of the buffer and the bottom
                os->write((const char *)addr + 4 + 256 + rx_r, 256 - rx_r); // write out first part
                if(rx_w > 0) {
                    // write out second part
                    os->write((const char *)addr + 4 + 256, rx_w); // write out second part
                }
            }

            // update read pointer
            addr[3] = rx_w;
        }

        if(!p->done && rx_u == 0) {
            vTaskDelay(0); // sleep and yield
        }
    }

    os->printf("Exiting the Forth terminal\nok\n");

    printf("DEBUG: ForthComms: Terminal thread exiting\n");
    vTaskDelete(NULL);
}

bool ForthComms::produce(char *rbuf, size_t len)
{
    uint8_t *addr = forth_comms_buffer;
    while(len > 0) {
        uint8_t tx_w = addr[0];
        uint8_t tx_r = addr[1];
        uint8_t tx_f = 255 - (tx_w - tx_r);
        uint8_t cnt = 0;
        for (size_t i = 0; i < tx_f; ++i) {
            if(rbuf[i] == 4) {
                // halt character ^D
                task_params.done= true;
                terminal_connected= false;
                return false;
            }
            uint8_t off = tx_w + i;
            addr[4 + off] = rbuf[cnt];
            ++cnt;
            if(--len == 0) break;
        }
        if(cnt > 0) {
            addr[0] = tx_w + cnt;
        }
        if(len > 0) {
            // still have data to pass onto forth, so wait a while
            vTaskDelay(pdMS_TO_TICKS(10)); // sleep and yield
        }
    }
    return true;
}

// TODO add command line editing and send line instead of character at a time
// also stop character echo
bool ForthComms::terminal( std::string& params, OutputStream& os )
{
    os.set_no_response();

    // this terminal runs in a thread so as not to stall the comms thread or the rest of smoothie
    if(terminal_connected) {
        os.printf("Forth terminal is already connected\n");
        return true;
    }

    start_CM4(); // make sure CM4 is started

    // function callback that captures incoming keystrokes or data and sends them to the task talking to the forth kernel
    using std::placeholders::_1;
    using std::placeholders::_2;
    os.fast_capture_fnc = std::bind(&ForthComms::produce, this, _1, _2);

    task_params.done = false;
    task_params.os = &os;

    // start terminal thread
    // Note this is lower priority than command thread and the comms thread
    BaseType_t status = xTaskCreate(terminal_thread, "ForthTerminalThread", 4000 / 4, &task_params, (tskIDLE_PRIORITY + 1UL), (TaskHandle_t *) NULL);
    if (status != pdPASS) {
        os.printf("ERROR: xTaskCreate failed, status=%ld\n", status);
        return true;
    }

    os.printf("This terminal will talk directly to the forth kernel. Type control-D to exit back to smoothie\n");
    terminal_connected= true;
    return true;
}

extern "C" int do_flash(FILE *fp);

// flash a version of forth to the upper flash bank
bool ForthComms::flash( std::string& params, OutputStream& os )
{
    if(params.empty()) {
        os.printf("Usage: fth flash forth.bin\n");
        return true;
    }

    // Get filename which is the entire parameter line
    std::string filename = params;

    FILE *fp = fopen( filename.c_str(), "r");
    if(fp == nullptr) {
        os.printf("File not found: %s\n", filename.c_str());
        return true;
    }

    os.printf("Flashing Forth binary %s\n", filename.c_str());
    if(do_flash(fp) == 0) {
        os.printf("Flashing failed\n");
    }else{
        os.printf("Flashing ok\n");
    }

    fclose(fp);
    return true;
}
