#pragma once

#include "Module.h"
#include "Pin.h"

class ConfigReader;

class MAX7129 : public Module
{

public:
    // Constructor
    MAX7129();
    virtual ~MAX7129();

    static bool create(ConfigReader& cr);
    bool configure(ConfigReader& cr);
    void display_int(int32_t num, bool leading_zeros=false);
    void display_float3(float num);

    void clear();
    void init();

private:
    void spi_write(uint16_t b);
    void write_register(uint8_t reg, uint8_t data);
    void cs_select(bool flg) { cs->set(!flg); }
    Pin *cs{nullptr};
    Pin *clk{nullptr};
    Pin *mosi{nullptr};
};
