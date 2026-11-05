"""matter.remove_fabric action: removes one commissioned fabric by slot number."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation

from .types import matter_ns

CONF_SLOT = "slot"

MatterRemoveFabricAction = matter_ns.class_(
    "MatterRemoveFabricAction", automation.Action
)


@automation.register_action(
    "matter.remove_fabric",
    MatterRemoveFabricAction,
    cv.maybe_simple_value(
        {cv.Required(CONF_SLOT): cv.templatable(cv.int_range(min=1, max=255))},
        key=CONF_SLOT,
    ),
    synchronous=True,
)
async def matter_remove_fabric_to_code(config, action_id, template_arg, args):
    cg.add_global(
        cg.RawStatement('#include "esphome/components/matter/matter_fabric_actions.h"')
    )
    var = cg.new_Pvariable(action_id, template_arg)
    template_ = await cg.templatable(config[CONF_SLOT], args, cg.uint8)
    cg.add(var.set_slot(template_))
    return var


CONF_VENDOR = "vendor"

MatterRemoveFabricVendorAction = matter_ns.class_(
    "MatterRemoveFabricVendorAction", automation.Action
)


@automation.register_action(
    "matter.remove_fabric_vendor",
    MatterRemoveFabricVendorAction,
    cv.maybe_simple_value(
        {cv.Required(CONF_VENDOR): cv.templatable(cv.int_range(min=1, max=0xFFFF))},
        key=CONF_VENDOR,
    ),
    synchronous=True,
)
async def matter_remove_fabric_vendor_to_code(config, action_id, template_arg, args):
    cg.add_global(
        cg.RawStatement(
            '#include "esphome/components/matter/matter_fabric_vendor_actions.h"'
        )
    )
    var = cg.new_Pvariable(action_id, template_arg)
    template_ = await cg.templatable(config[CONF_VENDOR], args, cg.uint16)
    cg.add(var.set_vendor(template_))
    return var
