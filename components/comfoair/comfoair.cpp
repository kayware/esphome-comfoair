
#include "comfoair.h"
#include "registers.h"

namespace esphome {
namespace comfoair {

void ComfoAirComponent::loop() {
}

void ComfoAirComponent::update() {
}

climate::ClimateTraits ComfoAirComponent::traits() {
    auto traits = climate::ClimateTraits();
    traits.set_supports_current_temperature(true);
    traits.set_supported_modes({
      climate::CLIMATE_MODE_FAN_ONLY
    });
    traits.set_supports_two_point_target_temperature(false);
    traits.set_supported_presets({
        climate::CLIMATE_PRESET_HOME,
    });
    traits.set_supports_action(false);
    traits.set_visual_min_temperature(12);
    traits.set_visual_max_temperature(29);
    traits.set_visual_temperature_step(1);
    traits.set_supported_fan_modes({
      climate::CLIMATE_FAN_AUTO,
      climate::CLIMATE_FAN_LOW,
      climate::CLIMATE_FAN_MEDIUM,
      climate::CLIMATE_FAN_HIGH,
      climate::CLIMATE_FAN_OFF,
    });
    return traits;
}

void ComfoAirComponent::control(const climate::ClimateCall &call) {
}

void ComfoAirComponent::write_command(uint8_t command, uint8_t const* data, uint8_t data_length) {
    write_byte(COMMAND_PREFIX);
    write_byte(COMMAND_HEAD);

    write_byte(0x00);
    write_byte(command);

    write_byte(data_length);

    for (uint16_t i = 0; i < data_length; ++i) {
        if (data[i] == 0x07)
            write_byte(0x07);
        write_byte(data[i]);
    }

    write_byte((command + data_length + calc_checksum(data, data_length, false)) & 0xff);

    write_byte(COMMAND_PREFIX);
    write_byte(COMMAND_TAIL);
    flush();
}

uint8_t ComfoAirComponent::calc_checksum(uint8_t const* buffer, uint8_t size, bool skip_double_seven) const {
    uint8_t checksum = 173, x;
    bool skip = false;
    for (uint16_t i = 0; i < size; ++i) {
        x = buffer[i];
        if (skip_double_seven && (x == 0x07)) {
            if (skip) {
                skip = false;
                continue;
            }
            skip = true;
        }
        checksum += x;
    }
    return checksum;
}

}
}
