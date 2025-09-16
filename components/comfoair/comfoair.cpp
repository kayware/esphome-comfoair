
#include "comfoair.h"
#include "registers.h"

namespace esphome {
namespace comfoair {

void ComfoAirComponent::loop() {
    while (available() != 0) {
        uint8_t byte;
        read_byte(&byte);
        
        switch (response_state) {
        case ResponseState::Head:
            if (last_response_byte == COMMAND_PREFIX && byte == COMMAND_HEAD)
                response_state = ResponseState::Command;
            break;
        case ResponseState::Command:
            if (last_response_byte == 0x00) {
                response_code = byte;
                response_state = ResponseState::DataLength;
            }
            break;
        case ResponseState::DataLength:
            response_data_length = byte;
            response_index = 0;
            response_state = (response_data_length == 0) ? ResponseState::Checksum : ResponseState::Data;
            break;
        case ResponseState::Data:
            response_data[response_index++] = byte;
            if (response_index >= sizeof(response_data)) {
                ESP_LOGW(TAG, "Device response exceeds size limit!");
                response_state = ResponseState::Head;
                break;
            }
            if (response_index == response_data_length)
                response_state = ResponseState::Checksum;
            if (byte == 0x07)
                response_state = ResponseState::DataSkipNext;
            break;
        case ResponseState::DataSkipNext:
            response_state = ResponseState::Data;
            break;
        case ResponseState::Checksum:
        {
            uint8_t expected = (response_code + response_data_length + calc_checksum(response_data, response_data_length)) & 0xff;
            if (byte == expected)
                response_state = ResponseState::Tail;
            else {
                ESP_LOGW(TAG, "Got invalid checksum in response: 0x%02x", byte);
                response_state = ResponseState::Head;
            }
            break;
        }
        case ResponseState::Tail:
            if (last_response_byte == COMMAND_PREFIX && byte == COMMAND_TAIL) {
                ESP_LOGV(TAG, "Got valid response from device. Command 0x%02x, 0x%02x data bytes.", response_code, response_data_length);
                parse_response();
                response_state = ResponseState::Head;
            } else if (byte != COMMAND_PREFIX) {
                ESP_LOGW(TAG, "Got junk after checksum validation in command 0x%02x: 0x%02x", response_code, byte);
            }
            break;
        }

        last_response_byte = byte;
    }
}

void ComfoAirComponent::update() {
    switch (update_counter) {
    case -1:
    {
        const static uint8_t values[] = {
            0, 30, 60,
            0, 30, 60,
            80, 80, 0
        };

        ESP_LOGD(TAG, "Setting vent levels");
        write_command(CMD_SET_VENTILATION_LEVEL, values, sizeof(values));
    }
    case 0:
        ESP_LOGD(TAG, "Getting the fan status");
        write_command(CMD_GET_FAN_STATUS, nullptr, 0);
        break;
    case 1:
        ESP_LOGD(TAG, "Getting ventilation level data");
        write_command(CMD_GET_VENTILATION_LEVEL, nullptr, 0);
        break;
    case 2:
    {
        ESP_LOGD(TAG, "Setting ventillation level");
        uint8_t lvl[1] = { 3 };
        write_command(CMD_SET_VENTILATION_LEVEL, lvl, 1);
    }
    }

    if (++update_counter >= 3)
        update_counter = 0;
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

void ComfoAirComponent::parse_response() {
    switch (response_code) {
    case RES_GET_VENTILATION_LEVEL:
        ESP_LOGD(TAG, "Exhaust air levels: %d%% %d%% %d%% %d%%", response_data[0], response_data[1], response_data[2], response_data[10]);
        ESP_LOGD(TAG, "Supply air levels: %d%% %d%% %d%% %d%%", response_data[3], response_data[4], response_data[5], response_data[11]);
        ESP_LOGD(TAG, "Current exh/sup: %d%% / %d%% (Level %d)", response_data[6], response_data[7], response_data[8]);
        if (response_data[9])
            ESP_LOGD(TAG, "Supply fan is ACTIVE");
        break;
    case RES_GET_FAN_STATUS: {
        int exhaust_rpm = 1875000 / ((int)((uint32_t)response_data[2] << 8 | (uint32_t)response_data[3]));
        int supply_rpm = 1875000 / ((int)((uint32_t)response_data[4] << 8 | (uint32_t)response_data[5]));
        ESP_LOGD(TAG, "Exhaust: %d%%", response_data[0]);
        ESP_LOGD(TAG, "Supply: %d%%", response_data[1]);
        ESP_LOGD(TAG, "RPM exh/sup: %d%% / %d%%", supply_rpm, exhaust_rpm);

    }
        break;
    default:
        break;
    }
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

    write_byte((command + data_length + calc_checksum(data, data_length)) & 0xff);

    write_byte(COMMAND_PREFIX);
    write_byte(COMMAND_TAIL);
    flush();
}

uint8_t ComfoAirComponent::calc_checksum(uint8_t const* buffer, uint8_t size) const {
    uint8_t checksum = 173;
    for (uint16_t i = 0; i < size; ++i)
        checksum += buffer[i];
    return checksum;
}

}
}
