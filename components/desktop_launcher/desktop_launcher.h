#pragma once
#include "esphome/core/component.h"
#include "esphome/components/smart_display/plugin_api.h"
#include <string>

namespace esphome::desktop_launcher {
class LauncherTile : public tessera::Tile {
 public:
  void create(const tessera::TileContext &context) override;
  void on_theme() override;
  void on_tap() override;
 private:
  std::string entity_, target_, icon_path_;
  lv_obj_t *title_{}, *fallback_{}, *picture_{};
};
class DesktopLauncher : public Component, public tessera::Plugin {
 public:
  void setup() override;
};
}  // namespace esphome::desktop_launcher
