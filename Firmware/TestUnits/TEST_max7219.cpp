// build with...
// rake testing=1 test=max7219 modules=utils/display/max7219 -m

#include "../Unity/src/unity.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <sstream>
#include <iostream>
#include <string>

#include "TestRegistry.h"

#include "FreeRTOS.h"
#include "task.h"

#include "max7129.h" //include the module
#include "ConfigReader.h"
#include "benchmark_timer.h"

MAX7129 display;

// define config here, this is in the same format they would appear in the config.ini file
const static char max7129_config[]= "\
[max7129]\n\
enable = true \n\
clk = PJ7 \n\
mosi = PE8 \n\
";

const static char *x_cs = "PJ8";
const static char *y_cs = "PD15";

#define WAIT(tmo) { uint32_t st = benchmark_timer_start(); while(benchmark_timer_as_ms(benchmark_timer_elapsed(st)) < tmo); }

REGISTER_TEST(MAX7129, run_int_tests)
{
    // load config with required settings for this test
    std::stringstream ss(max7129_config);
    ConfigReader cr(ss);
    TEST_ASSERT_TRUE(display.configure(cr));

    Module *m= Module::lookup("max7129");
    TEST_ASSERT_NOT_NULL(m);

    // create instances each with its own CS pin
    int id1 = display.add_instance(x_cs);
    TEST_ASSERT_EQUAL(id1, 0);
    int id2 = display.add_instance(y_cs);
    TEST_ASSERT_EQUAL(id2, 1);

    // inializes all displays
    display.init();

    display.clear(id1);
    display.clear(id2);

    printf("Display on X display....\n");

    printf("display 0\n");
    display.display_int(id1, 0);
    WAIT(3000);

    printf("display 00000000\n");
    display.display_int(id1, 0, true);
    WAIT(3000);

    printf("display 00000001\n");
    display.display_int(id1, 1, true);
    WAIT(3000);

    printf("count up to 100\n");
    for (int i = 0; i < 101; ++i) {
        display.display_int(id1, i);
        WAIT(100);
    }

    printf("display 123\n");
    display.display_int(id1, 123);
    WAIT(5000) ;

    printf("display -123\n");
    display.display_int(id1, -123);
    WAIT(5000) ;

    printf("display 12345678\n");
    display.display_int(id1, 12345678);
    WAIT(5000) ;

    printf("display -1234567\n");
    display.display_int(id1, -1234567);
    WAIT(5000) ;

    printf("display 101.234\n");
    display.display_float3(id1, 101.234);
    WAIT(5000) ;

    printf("display -101.234\n");
    display.display_float3(id1, -101.234);
    WAIT(5000) ;

    printf("display 1.235\n");
    display.display_float3(id1, 1.2345);
    WAIT(5000) ;

    printf("display 1.235\n");
    display.display_float3(id1, 1.23445);
    WAIT(5000) ;

    printf("display 1.200\n");
    display.display_float3(id1, 1.2);
    WAIT(5000) ;

}
