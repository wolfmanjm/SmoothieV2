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
cs = PJ8 \n\
";

REGISTER_TEST(MAX7129, run_tests)
{
    // load config with required settings for this test
    std::stringstream ss(max7129_config);
    ConfigReader cr(ss);
    TEST_ASSERT_TRUE(display.configure(cr));

    Module *m= Module::lookup("max7129");
    TEST_ASSERT_NOT_NULL(m);

    display.init();
    display.clear();

    for (int i = 0; i < 999; ++i) {
        display.display_int(i);
        uint32_t st = benchmark_timer_start(); while(benchmark_timer_as_ms(benchmark_timer_elapsed(st)) < 100) ;
    }

    display.display_int(-123);
    uint32_t st = benchmark_timer_start(); while(benchmark_timer_as_ms(benchmark_timer_elapsed(st)) < 1000) ;

    display.display_int(-1234567);
    st = benchmark_timer_start(); while(benchmark_timer_as_ms(benchmark_timer_elapsed(st)) < 1000) ;


}
