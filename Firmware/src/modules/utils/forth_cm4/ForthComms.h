#pragma once

#include "Module.h"

#include <string>
#include <map>
#include <vector>
#include <thread>

class OutputStream;
class GCode;

class ForthComms : public Module {
    public:
        ForthComms();

        static bool create(ConfigReader& cr);
        bool configure(ConfigReader&);

    private:
        bool fth_command( std::string& parameters, OutputStream& os );
        bool flash( std::string& params, OutputStream& os );
        bool terminal( std::string& params, OutputStream& os );
};
