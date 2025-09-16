#ifndef COMFOAIR_H
#define COMFOAIR_H

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/climate/climate_mode.h"
#include "esphome/components/climate/climate_traits.h"

#define POLLING_FREQUENCY 1000

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


    float get_setup_priority() const override { return setup_priority::DATA; }
    void set_uart_component(uart::UARTComponent *parent) { set_uart_parent(parent); }

protected:

};

}
}

#endif