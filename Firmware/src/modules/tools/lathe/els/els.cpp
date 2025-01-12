// Electronic Leadscrew as seen in Clough42 et al.

#include "els.h"

#include "TM1638.h"
#include "buttonbox.h"
#include "ConfigReader.h"
#include "SlowTicker.h"
#include "main.h"
#include "Lathe.h"
#include "OutputStream.h"
#include "MessageQueue.h"

#include <cmath>
#include <string>
#include <iostream>

#define enable_key "enable"

REGISTER_MODULE(ELS, ELS::create)

bool ELS::create(ConfigReader& cr)
{
    printf("DEBUG: configure ELS\n");
    ELS *t = new ELS();
    if(!t->configure(cr)) {
        printf("INFO: ELS not enabled\n");
        delete t;
    }
    return true;
}

ELS::ELS() : Module("ELS")
{}

bool ELS::configure(ConfigReader& cr)
{
    ConfigReader::section_map_t m;
    if(!cr.get_section("els", m)) return false;

    bool enable = cr.get_bool(m, enable_key , false);
    if(!enable) {
        return false;
    }

    // register a startup function that will be called after all modules have been loaded
    // (as this module relies on the tm1638 and Lathe modules having been loaded)
    register_startup(std::bind(&ELS::after_load, this));

    // start up timers
    SlowTicker::getInstance()->attach(10, std::bind(&ELS::check_buttons, this));
    SlowTicker::getInstance()->attach(1, std::bind(&ELS::update_rpm, this));

    return true;
}

void ELS::after_load()
{
    printf("DEBUG: ELS: post config running...\n");

    Module *v= Module::lookup("Lathe");
    if(v == nullptr) {
        printf("ERROR: ELS: Lathe is not available\n");
        return;
    }

    lathe= static_cast<Lathe*>(v);

    // get display if available
    v= Module::lookup("tm1638");
    if(v != nullptr) {
        tm=  static_cast<TM1638*>(v);
        tm->lock();
        tm->displayBegin();
        tm->reset();
        tm->unlock();

        printf("DEBUG: ELS: TM1638 display started\n");
    }else{
        printf("ERROR: ELS: display is not available\n");
        return;
    }

    started= true;
}

void ELS::update_rpm()
{
    if(tm == nullptr || !started) return;

    // update display once per second
    if(tm->lock()) {
        // display current RPM in 4 left segments
        rpm= lathe->get_rpm();
        tm->DisplayDecNumNibble((int)roundf(rpm), var1, false, TMAlignTextRight);

        // display if running and what mode it is in
        if(lathe->is_running()) {
            tm->setLED(std::isnan(lathe->get_distance())?0:1, 1);
        }else{
            tm->setLED(std::isnan(lathe->get_distance())?0:1, 0);
        }
        tm->unlock();
    }
}

// called in slow timer every 100ms to scan buttons
void ELS::check_buttons()
{
    static uint8_t last_buttons= 0;

    if(tm == nullptr || !started) return;

    if(tm->lock()) {
        buttons = tm->readButtons();
        tm->unlock();
    }

    /* buttons contains a byte with values of button s8s7s6s5s4s3s2s1
     HEX  :  Switch no : Binary
     0x01 : S1 Pressed  0000 0001
     0x02 : S2 Pressed  0000 0010
     0x04 : S3 Pressed  0000 0100
     0x08 : S4 Pressed  0000 1000
     0x10 : S5 Pressed  0001 0000
     0x20 : S6 Pressed  0010 0000
     0x40 : S7 Pressed  0100 0000
     0x80 : S8 Pressed  1000 0000
    */

    static OutputStream os(&std::cout); // NULL output stream, but we need to keep some state between calls

    if((buttons & 0x01) && !(last_buttons & 0x01)) {
        // button 1 pressed - Stop current operation
        if(lathe->is_running()) {
            os.set_stop_request(true);
        }
    }

    if((buttons & 0x02) && !(last_buttons & 0x02)) {
        // button 2 pressed, start operation G33 K{var1}
        if(!lathe->is_running()) {
            std::string cmd("G33 K");
            cmd.append(std::to_string(var1));
            send_message_queue(cmd.c_str(), &os, false);
        }
    }

    // up/down buttons
    if((buttons & 0x80) && !(last_buttons & 0x80)) {
        if(++var1 > 9999) {
            var1= 9999;
        }else{
            if(tm->lock()) {
                tm->DisplayDecNumNibble((int)roundf(rpm), var1, false, TMAlignTextRight);
                tm->unlock();
            }
        }
    }

    if((buttons & 0x20) && !(last_buttons & 0x20)) {
        if(--var1 < 0) {
            var1= 0;
        }else{
            if(tm->lock()) {
                tm->DisplayDecNumNibble((int)roundf(rpm), var1, false, TMAlignTextRight);
                tm->unlock();
            }
        }
    }

    last_buttons= buttons;
}




