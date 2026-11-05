#pragma once

#include "esphome/core/defines.h"
#ifdef USE_MATTER

#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include <cstddef>

namespace esphome::matter_fabric {

class MatterFabricStatus : public PollingComponent {
public:
  void set_slot(size_t slot) { this->slot_ = slot; }
  void set_controller_sensor(text_sensor::TextSensor *s) { this->controller_ = s; }
  void set_subscriptions_sensor(text_sensor::TextSensor *s) { this->subscriptions_ = s; }
  void set_sessions_sensor(text_sensor::TextSensor *s) { this->sessions_ = s; }

  void update() override;
  float get_setup_priority() const override { return setup_priority::LATE; }

protected:
  size_t slot_{0};
  text_sensor::TextSensor *controller_{nullptr};
  text_sensor::TextSensor *subscriptions_{nullptr};
  text_sensor::TextSensor *sessions_{nullptr};
};

} // namespace esphome::matter_fabric

#endif // USE_MATTER
