
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, climate
from esphome.const import (CONF_ID, CONF_UART_ID)

comfoair_ns = cg.esphome_ns.namespace("comfoair")
ComfoAirComponent = comfoair_ns.class_('ComfoAirComponent', climate.Climate, cg.Component, uart.UARTDevice)

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["climate"]
REQUIRED_KEY_NAME = "name"

CONFIG_SCHEMA = (
  climate.climate_schema(ComfoAirComponent)
  .extend(
    {
      cv.Required(REQUIRED_KEY_NAME): cv.string,
    }
  )
  .extend(uart.UART_DEVICE_SCHEMA)
)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)
    yield climate.register_climate(var, config)
    cg.add(var.set_name(config[REQUIRED_KEY_NAME]))
    paren = yield cg.get_variable(config[CONF_UART_ID])
    cg.add(var.set_uart_component(paren))
