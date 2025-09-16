#ifndef COMFOAIR_H
#define COMFOAIR_H

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/climate/climate_mode.h"
#include "esphome/components/climate/climate_traits.h"

#define POLLING_FREQUENCY 10000

namespace esphome {
namespace comfoair {

static const char *TAG = "comfoair";

class ComfoAirComponent : public climate::Climate, public PollingComponent, public uart::UARTDevice {
public:

    ComfoAirComponent()
    : Climate()
    , PollingComponent(POLLING_FREQUENCY)
    , UARTDevice()
    {
    }

    void loop() override;
    void update() override;
    climate::ClimateTraits traits() override;
    void control(const climate::ClimateCall &call) override;

    float get_setup_priority() const override { return setup_priority::DATA; }
    void set_uart_component(uart::UARTComponent *parent) { set_uart_parent(parent); }

protected:

    void write_command(uint8_t command, uint8_t const* data, uint8_t data_length);
    uint8_t calc_checksum(uint8_t const* buffer, uint8_t size) const;
    void parse_response();

    int8_t update_counter { -1 };
    int8_t const update_counter_rollover { 2 };

    uint8_t response_data[64];
    uint8_t response_index { 0 };
    uint8_t response_data_length { 0 };
    uint8_t response_code { 0 };
    uint8_t last_response_byte { 0 };
    
    enum class ResponseState {
        Head,
        Command,
        DataLength,
        Data,
        DataSkipNext,
        Checksum,
        Tail,
    } response_state = ResponseState::Head;

};

}
}

#endif