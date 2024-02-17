#pragma once

#include "esphome/components/light/light_output.h"
#include "esphome/components/output/float_output.h"
#include "esphome/core/component.h"
#include "NeoPixelBus.h"

namespace esphome {
namespace color_temperature {

class CTLightWS2811 : public light::LightOutput {
 public:
  void set_color_temperature(output::FloatOutput *color_temperature) { color_temperature_ = color_temperature; }
  void set_brightness(output::FloatOutput *brightness) { brightness_ = brightness; }
  void set_cold_white_temperature(float cold_white_temperature) { cold_white_temperature_ = cold_white_temperature; }
  void set_warm_white_temperature(float warm_white_temperature) { warm_white_temperature_ = warm_white_temperature; }
  light::LightTraits get_traits() override {
    auto traits = light::LightTraits();
    traits.set_supported_color_modes({light::ColorMode::COLOR_TEMPERATURE});
    traits.set_min_mireds(this->cold_white_temperature_);
    traits.set_max_mireds(this->warm_white_temperature_);
    return traits;
  }

  void write_state(light::LightState *state) override {
    float color_temperature, brightness;
    state->current_values_as_ct(&color_temperature, &brightness);
    this->color_temperature_->set_level(color_temperature);
    this->brightness_->set_level(brightness);
    uint8_t *pixels = controller_->Pixels();
    float warm;
    float cold;
    if (color_temperature <= 0.5) {
      warm = 2.0 * color_temperature;
      cold = 1.0;
    } else {
      warm = 1.0;
      cold = 2.0 - 2.0 * color_temperature;
    }

    for (int i = 0; i < controller_->PixelCount(); i++) {
      pixels[i++] = (int) (255.0 * cold * brightness);
      pixels[i++] = (int) (255.0 * warm * brightness);
      pixels[i++] = 0;
    }
    controller_->Show();
  }
  void setup() { controller_ = new NeoPixelBus<NeoGrbFeature, NeoWs2811Method>(60, 2); }

 protected:
  NeoPixelBus<NeoGrbFeature, NeoWs2811Method> *controller_{nullptr};
  output::FloatOutput *color_temperature_;
  output::FloatOutput *brightness_;
  float cold_white_temperature_;
  float warm_white_temperature_;
};

}  // namespace color_temperature
}  // namespace esphome
