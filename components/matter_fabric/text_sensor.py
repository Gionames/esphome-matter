"""text_sensor platform matter_fabric: describes one fabric slot of the Matter fabric table.

text_sensor:
  - platform: matter_fabric
    slot: 1                       # same order as the matter fabric_N text sensors (1-based)
    controller:                   # Home Assistant, Google, Amazon Alexa, ...
      name: "Matter fabric 1 controller"
    subscriptions:                # active subscriptions of this fabric (0 = controller not listening)
      name: "Matter fabric 1 subscriptions"
    sessions:                     # active secure sessions of this fabric
      name: "Matter fabric 1 sessions"
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

DEPENDENCIES = ["matter"]

CONF_SLOT = "slot"
CONF_CONTROLLER = "controller"
CONF_SUBSCRIPTIONS = "subscriptions"
CONF_SESSIONS = "sessions"

matter_fabric_ns = cg.esphome_ns.namespace("matter_fabric")
MatterFabricStatus = matter_fabric_ns.class_("MatterFabricStatus", cg.PollingComponent)


def _sensor(icon):
    return text_sensor.text_sensor_schema(
        icon=icon, entity_category=ENTITY_CATEGORY_DIAGNOSTIC
    )


CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MatterFabricStatus),
            cv.Required(CONF_SLOT): cv.int_range(min=1, max=255),
            cv.Optional(CONF_CONTROLLER): _sensor("mdi:account-network"),
            cv.Optional(CONF_SUBSCRIPTIONS): _sensor("mdi:bell-ring-outline"),
            cv.Optional(CONF_SESSIONS): _sensor("mdi:lan-connect"),
        }
    ).extend(cv.polling_component_schema("10s")),
    cv.has_at_least_one_key(CONF_CONTROLLER, CONF_SUBSCRIPTIONS, CONF_SESSIONS),
)


async def to_code(config):
    var = cg.new_Pvariable(config["id"])
    await cg.register_component(var, config)
    cg.add(var.set_slot(config[CONF_SLOT] - 1))
    for key, setter in (
        (CONF_CONTROLLER, "set_controller_sensor"),
        (CONF_SUBSCRIPTIONS, "set_subscriptions_sensor"),
        (CONF_SESSIONS, "set_sessions_sensor"),
    ):
        if sensor_config := config.get(key):
            sens = await text_sensor.new_text_sensor(sensor_config)
            cg.add(getattr(var, setter)(sens))
