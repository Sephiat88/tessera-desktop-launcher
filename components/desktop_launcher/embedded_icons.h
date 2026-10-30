#pragma once
#include "esphome/components/smart_display/plugin_api.h"
#include <string>

namespace esphome::desktop_launcher::embedded_icons {
// Returns static flash-backed LVGL 9 pixel data, or nullptr for the fallback.
const lv_image_dsc_t *find(const std::string &path);
}
