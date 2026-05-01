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
cs = PJ8 \n\
";

#define WAIT(tmo) { uint32_t st = benchmark_timer_start(); while(benchmark_timer_as_ms(benchmark_timer_elapsed(st)) < tmo); }

REGISTER_TEST(MAX7129, run_int_tests)
{
    // load config with required settings for this test
    std::stringstream ss(max7129_config);
    ConfigReader cr(ss);
    TEST_ASSERT_TRUE(display.configure(cr));

    Module *m= Module::lookup("max7129");
    TEST_ASSERT_NOT_NULL(m);

    display.init();
    display.clear();

    printf("display 0\n");
    display.display_int(0);
    WAIT(3000);

    printf("display 00000000\n");
    display.display_int(0, true);
    WAIT(3000);

    printf("display 00000001\n");
    display.display_int(1, true);
    WAIT(3000);

    printf("count up to 100\n");
    for (int i = 0; i < 101; ++i) {
        display.display_int(i);
        WAIT(100);
    }

    printf("display 123\n");
    display.display_int(123);
    WAIT(5000) ;

    printf("display -123\n");
    display.display_int(-123);
    WAIT(5000) ;

    printf("display 12345678\n");
    display.display_int(12345678);
    WAIT(5000) ;

    printf("display -1234567\n");
    display.display_int(-1234567);
    WAIT(5000) ;

    printf("display 101.234\n");
    display.display_float3(101.234);
    WAIT(5000) ;

    printf("display -101.234\n");
    display.display_float3(-101.234);
    WAIT(5000) ;

    printf("display 1.235\n");
    display.display_float3(1.2345);
    WAIT(5000) ;

    printf("display 1.235\n");
    display.display_float3(1.23445);
    WAIT(5000) ;

    printf("display 1.200\n");
    display.display_float3(1.2);
    WAIT(5000) ;

}
