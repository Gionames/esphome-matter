#include "matter_fabric_status.h"
#ifdef USE_MATTER

#include <app/InteractionModelEngine.h>
#include <app/server/Server.h>
#include <platform/CHIPDeviceLayer.h>

#include <cstdint>
#include <cstdio>
#include <string>

namespace esphome::matter_fabric {

namespace {

// Matter vendor IDs (CSA) of the main controller ecosystems.
std::string controller_name(uint16_t vendor) {
  switch (vendor) {
  case 0x134B:
    return "Home Assistant";
  case 0x6006:
    return "Google";
  case 0x1217:
    return "Amazon Alexa";
  case 0x1349:
    return "Apple Home";
  case 0x110A:
    return "SmartThings";
  default: {
    char buf[24];
    snprintf(buf, sizeof(buf), "Altro (0x%04X)", (unsigned) vendor);
    return buf;
  }
  }
}

struct SessionCount {
  chip::FabricIndex fabric;
  uint32_t count;
};

bool count_session(void *context, chip::SessionHandle &handle) {
  auto *c = static_cast<SessionCount *>(context);
  if (handle->IsSecureSession() && handle->AsSecureSession()->IsActiveSession() &&
      handle->GetFabricIndex() == c->fabric)
    c->count++;
  return true;
}

} // namespace

void MatterFabricStatus::update() {
  bool present = false;
  uint16_t vendor = 0;
  uint32_t subscriptions = 0;
  SessionCount sessions{chip::kUndefinedFabricIndex, 0};

  chip::DeviceLayer::PlatformMgr().LockChipStack();
  auto &server = chip::Server::GetInstance();
  size_t position = 0;
  for (const auto &fabric : server.GetFabricTable()) {
    if (position++ != this->slot_)
      continue;
    present = true;
    vendor = static_cast<uint16_t>(fabric.GetVendorId());
    sessions.fabric = fabric.GetFabricIndex();
    subscriptions = chip::app::InteractionModelEngine::GetInstance()->GetNumActiveReadHandlers(
        chip::app::ReadHandler::InteractionType::Subscribe, fabric.GetFabricIndex());
    server.GetSecureSessionManager().ForEachSessionHandle(&sessions, count_session);
    break;
  }
  chip::DeviceLayer::PlatformMgr().UnlockChipStack();

  // An empty slot reads "-" (a text sensor cannot go back to unknown).
  if (this->controller_ != nullptr)
    this->controller_->publish_state(present ? controller_name(vendor) : "-");
  if (this->subscriptions_ != nullptr)
    this->subscriptions_->publish_state(present ? std::to_string(subscriptions) : "-");
  if (this->sessions_ != nullptr)
    this->sessions_->publish_state(present ? std::to_string(sessions.count) : "-");
}

} // namespace esphome::matter_fabric

#endif // USE_MATTER
