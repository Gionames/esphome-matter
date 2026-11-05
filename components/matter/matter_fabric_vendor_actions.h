#pragma once

#include "esphome/core/defines.h"
#ifdef USE_MATTER

#include "matter_fabric_actions.h"

namespace esphome::matter {

// Removes every fabric commissioned by the given Matter vendor ID (for example
// 0x1217 = Amazon Alexa, 0x6006 = Google, 0x134B = Home Assistant). Independent
// of the slot order. Reboots after the last fabric is removed, like
// remove_fabric_by_slot().
inline bool remove_fabrics_by_vendor(uint16_t vendor) {
  static const char *const TAG = "matter.fabric";
  chip::FabricIndex indexes[CHIP_CONFIG_MAX_FABRICS];
  size_t found = 0;
  size_t removed = 0;
  size_t remaining = 0;

  chip::DeviceLayer::PlatformMgr().LockChipStack();
  auto &fabric_table = chip::Server::GetInstance().GetFabricTable();
  // Collect first: deleting while iterating would invalidate the iterator.
  for (const auto &fabric : fabric_table) {
    if (static_cast<uint16_t>(fabric.GetVendorId()) == vendor && found < CHIP_CONFIG_MAX_FABRICS)
      indexes[found++] = fabric.GetFabricIndex();
  }
  for (size_t i = 0; i < found; i++) {
    if (fabric_table.Delete(indexes[i]) == CHIP_NO_ERROR)
      removed++;
  }
  remaining = fabric_table.FabricCount();
  chip::DeviceLayer::PlatformMgr().UnlockChipStack();

  ESP_LOGI(TAG, "Vendor 0x%04X: removed %u of %u fabric(s). %u fabric(s) left", static_cast<unsigned>(vendor),
           static_cast<unsigned>(removed), static_cast<unsigned>(found), static_cast<unsigned>(remaining));
  if (removed > 0 && remaining == 0) {
    ESP_LOGW(TAG, "Last fabric removed. Rebooting to reopen commissioning");
    App.safe_reboot();
  }
  return removed > 0;
}

template <typename... Ts> class MatterRemoveFabricVendorAction : public Action<Ts...> {
public:
  TEMPLATABLE_VALUE(uint16_t, vendor)

  void play(Ts... x) override {
    uint16_t vendor = this->vendor_.value(x...);
    if (vendor == 0)
      return;
    remove_fabrics_by_vendor(vendor);
  }
};

} // namespace esphome::matter

#endif // USE_MATTER
