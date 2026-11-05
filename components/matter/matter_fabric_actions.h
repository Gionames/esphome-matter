#pragma once

#include "esphome/core/defines.h"
#ifdef USE_MATTER

#include "esphome/core/application.h"
#include "esphome/core/automation.h"
#include "esphome/core/log.h"

#include <app/server/Server.h>
#include <platform/PlatformManager.h>

namespace esphome::matter {

// Removes the fabric at the given 0-based position of the fabric table (same
// order as the fabric_N text sensors). Any fabric can be removed, including
// the last one. Controllers are not notified.
inline bool remove_fabric_by_slot(size_t slot) {
  static const char *const TAG = "matter.fabric";
  chip::FabricIndex index = chip::kUndefinedFabricIndex;
  CHIP_ERROR err = CHIP_ERROR_NOT_FOUND;
  size_t remaining = 0;

  chip::DeviceLayer::PlatformMgr().LockChipStack();
  auto &fabric_table = chip::Server::GetInstance().GetFabricTable();
  size_t position = 0;
  for (const auto &fabric : fabric_table) {
    if (position == slot) {
      index = fabric.GetFabricIndex();
      break;
    }
    position++;
  }
  if (index != chip::kUndefinedFabricIndex)
    err = fabric_table.Delete(index);
  remaining = fabric_table.FabricCount();
  chip::DeviceLayer::PlatformMgr().UnlockChipStack();

  if (err != CHIP_NO_ERROR) {
    ESP_LOGW(TAG, "Cannot remove fabric in slot %u: %s",
             static_cast<unsigned>(slot + 1), err.Format());
    return false;
  }
  ESP_LOGI(TAG, "Removed fabric index %u (slot %u). %u fabric(s) left",
           static_cast<unsigned>(index), static_cast<unsigned>(slot + 1),
           static_cast<unsigned>(remaining));
  if (remaining == 0) {
    // Without any fabric the device must be commissionable again. A reboot is
    // the reliable way to reopen the commissioning window.
    ESP_LOGW(TAG, "Last fabric removed. Rebooting to reopen commissioning");
    App.safe_reboot();
  }
  return true;
}

template <typename... Ts> class MatterRemoveFabricAction : public Action<Ts...> {
public:
  TEMPLATABLE_VALUE(uint8_t, slot)

  void play(Ts... x) override {
    // User facing slot numbers are 1-based like the fabric_N text sensors.
    uint8_t slot = this->slot_.value(x...);
    if (slot == 0)
      return;
    remove_fabric_by_slot(static_cast<size_t>(slot - 1));
  }
};

} // namespace esphome::matter

#endif // USE_MATTER
