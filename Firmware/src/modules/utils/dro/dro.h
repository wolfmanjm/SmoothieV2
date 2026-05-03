#pragma once

#include "Module.h"

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
};
