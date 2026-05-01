#pragma once

#include "Module.h"
#include "Pin.h"

#include <vector>
#include <map>

class ConfigReader;

class MAX7129 : public Module
{

public:
    // Constructor
    MAX7129();
    virtual ~MAX7129();

    static bool create(ConfigReader& cr);
    bool configure(ConfigReader& cr);
    int add_instance(const char *cs_pin);

    void display_int(int id, int32_t num, bool leading_zeros=false);
    void display_float3(int id, float num);

    void clear(int id);
    void init();

private:
    void spi_write(uint16_t b);
    void write_register(int id, uint8_t reg, uint8_t data);
    void cs_select(int id, bool flg);
    Pin *clk{nullptr};
    Pin *mosi{nullptr};
    std::vector<Pin *> cs_list;
};
