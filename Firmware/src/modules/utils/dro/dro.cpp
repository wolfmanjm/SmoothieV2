// Electronic Leadscrew as seen in Clough42 et al.

#include "dro.h"

#include "max7219.h"
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
#define cs_pin_key "cs_pin"
#define poll_freq_key "poll_frequency_hz"

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
    ConfigReader::sub_section_map_t ssmap;
    if(!cr.get_sub_sections("dro", ssmap)) {
        printf("INFO: configure-dro: no button box section found\n");
        return false;
    }

    auto s = ssmap.find("common");
    if(s != ssmap.end()) {
        auto& mm = s->second; // map of common config settings
        poll_freq = cr.get_int(mm, poll_freq_key, 10);
        printf("INFO: configure-dro: poll freq set to %ld hz\n", poll_freq);
    }

    // get the CS for each axis
    int cnt = 0;
    for(auto& i : ssmap) {
        // foreach axis, name needs to be x,y,z,a,b,c
        std::string name = i.first;
        if(name == "common") continue;

        auto& m = i.second;
        if(!cr.get_bool(m, enable_key, true)) continue; // skip if not enabled
        std::string p = cr.get_string(m, cs_pin_key, "nc");

        if(p.find_first_of("xyzabc") == p.npos) {
            printf("ERROR: configure-dro: axis %s is not one of xyzabc\n", p.c_str());
            continue;
        }
        axis_map[p] = -1;
        ++cnt;
    }

    printf("INFO: configure-dro: %d axis loaded\n", cnt);

    if(cnt > 0) {
        // register a startup function that will be called after all modules have been loaded
        // (as this module relies on the max7219 module having been loaded)
        register_startup(std::bind(&DRO::after_load, this));

        // start up timers
        SlowTicker::getInstance()->attach(10, std::bind(&DRO::update_display, this));
    }

    return true;
}

void DRO::after_load()
{
    printf("DEBUG: DRO post config running...\n");

    // get display if available
    Module *v= Module::lookup("max7219");
    if(v != nullptr) {
        display=  static_cast<MAX7219*>(v);
        display->init();

        printf("DEBUG: DRO MAX7219 display started\n");
    }else{
        printf("ERROR: DRO MAX7219 display is not available\n");
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

