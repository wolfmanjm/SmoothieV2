// Electronic Leadscrew as seen in Clough42 et al.

#include "dro.h"

#include "max7129.h"
#include "buttonbox.h"
#include "ConfigReader.h"
#include "SlowTicker.h"
#include "main.h"
#include "OutputStream.h"
#include "MessageQueue.h"

#include <cmath>
#include <string>
#include <iostream>

#define enable_key "enable"

REGISTER_MODULE(DRO, DRO::create)

bool DRO::create(ConfigReader& cr)
{
    printf("DEBUG: configure DRO\n");
    DRO *t = new DRO();
    if(!t->configure(cr)) {
        printf("INFO: DRO not enabled\n");
        delete t;
    }
    return true;
}

DRO::DRO() : Module("DRO")
{}

bool DRO::configure(ConfigReader& cr)
{
    ConfigReader::section_map_t m;
    if(!cr.get_section("dro", m)) return false;

    bool enable = cr.get_bool(m, enable_key , false);
    if(!enable) {
        return false;
    }

    // register a startup function that will be called after all modules have been loaded
    // (as this module relies on the max7129 module having been loaded)
    register_startup(std::bind(&DRO::after_load, this));

    // start up timers
    SlowTicker::getInstance()->attach(10, std::bind(&DRO::update_display, this));

    return true;
}

void DRO::after_load()
{
    printf("DEBUG: DRO post config running...\n");

    // get display if available
    v= Module::lookup("max7129");
    if(v != nullptr) {
        display=  static_cast<MAX7129*>(v);
        display->init();

        printf("DEBUG: DRO MAX7129 display started\n");
    }else{
        printf("ERROR: DRO MAX7129 display is not available\n");
        return;
    }

    // create instances each with its own CS pin
    // int id1 = display.add_instance(x_cs);
    // int id2 = display.add_instance(y_cs);

        // display->lock();
        // display->clear(i);
        // display->unlock();

    started= true;
}

