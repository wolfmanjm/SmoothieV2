#pragma once

#include "Module.h"

#include <string>
#include <map>

class MAX7219;

class DRO : public Module {
    public:
        DRO();
        static bool create(ConfigReader& cr);
        bool configure(ConfigReader& cr);

    private:
        void after_load();

        MAX7219 *display{nullptr};
        bool started{false};
        uint32_t poll_freq;

        std::map<std::string, int> axis_map;

};
