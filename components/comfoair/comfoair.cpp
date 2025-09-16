
#include "comfoair.h"

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

}
}
