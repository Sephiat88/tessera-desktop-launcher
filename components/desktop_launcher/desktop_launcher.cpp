#include "desktop_launcher.h"
#include "esphome/core/log.h"
#include <algorithm>

namespace esphome::desktop_launcher {
namespace ui = tessera::ui;
static const char *const TAG = "desktop_launcher";

void DesktopLauncher::setup() {
  ESP_LOGI(TAG, "Desktop launcher 1.0.1 diagnostic build");
  add_tile("launcher", []() -> tessera::Tile * { return new LauncherTile(); });
}

void LauncherTile::create(const tessera::TileContext &c) {
  // Context strings and JSON references expire after this callback.
  entity_ = c.entity ? c.entity : "";
  target_ = c.options["target"] | "";
  icon_path_ = c.options["icon_path"] | "";
  ESP_LOGI(TAG, "create: entity=%s icon=%s pictures=%s area=%dx%d",
           entity_.c_str(), icon_path_.c_str(), ui::pictures() ? "yes" : "no", c.width, c.height);
  if (!ui::pictures()) ESP_LOGW(TAG, "Tessera picture pipeline unavailable");
  else if (icon_path_.rfind("/local/", 0) != 0)
    ESP_LOGW(TAG, "Icon path must start with /local/");
  const int line = ui::line_height(tessera::Font::BODY);
  const int gap = ui::px(3);
  const int side = std::max(1, std::min(c.width, c.height - 2 * line - 2 * gap));

  fallback_ = ui::label(c.parent, tessera::Font::ICON, theme::INK);
  ui::set_text(fallback_, ui::icon(0xF0322));  // laptop, included in Tessera's font
  lv_obj_align(fallback_, LV_ALIGN_TOP_MID, 0, 0);

  if (ui::pictures() && icon_path_.rfind("/local/", 0) == 0) {
    picture_ = ui::picture(c.parent);
    lv_obj_set_size(picture_, side, side);
    lv_obj_align(picture_, LV_ALIGN_TOP_MID, 0, 0);
    ESP_LOGI(TAG, "Picture place: %dx%d source=%s", side, side, icon_path_.c_str());
    // Core requests the image from HA via the add-on; LVGL draws the prepared atlas.
    ui::set_picture(picture_, icon_path_, tessera::Fit::CONTAIN);
    lv_obj_move_foreground(fallback_);
  }

  title_ = ui::label(c.parent, tessera::Font::BODY, theme::INK);
  lv_obj_set_width(title_, c.width);
  lv_obj_set_style_text_align(title_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(title_, LV_ALIGN_BOTTOM_MID, 0, -line - gap);
  ui::set_text(title_, c.name && *c.name ? c.name : "Desktop");

  state_ = ui::label(c.parent, tessera::Font::BODY, theme::MUTED);
  lv_obj_set_width(state_, c.width);
  lv_obj_set_style_text_align(state_, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(state_, LV_ALIGN_BOTTOM_MID, 0, 0);
  ui::set_text(state_, "--");
}

void LauncherTile::on_state(JsonObjectConst data) {
  std::string value = data["state"] | "--";
  const char *unit = data["attributes"]["unit_of_measurement"] | "";
  if (*unit) value += std::string(" ") + unit;
  ui::set_text(state_, value);
}

void LauncherTile::on_theme() {
  ui::set_color(title_, theme::INK);
  ui::set_color(state_, theme::MUTED);
  ui::set_color(fallback_, theme::INK);
}

void LauncherTile::on_picture(lv_obj_t *place, tessera::PictureState result) {
  if (place != picture_) return;
  const char *outcome = result == tessera::PictureState::SHOWN ? "SHOWN" :
                        result == tessera::PictureState::NONE ? "NONE" : "FAILED";
  ESP_LOGI(TAG, "Picture outcome: %s source=%s", outcome, icon_path_.c_str());
  if (result == tessera::PictureState::SHOWN) {
    lv_obj_add_flag(fallback_, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_move_foreground(fallback_);
    lv_obj_clear_flag(fallback_, LV_OBJ_FLAG_HIDDEN);
  }
}

void LauncherTile::on_tap() {
  const std::string &target = target_.empty() ? entity_ : target_;
  const auto dot = target.find('.');
  const std::string domain = target.substr(0, dot);
  const char *service = nullptr;
  if (domain == "button") service = "button.press";
  else if (domain == "input_button") service = "input_button.press";
  else if (domain == "switch") service = "switch.toggle";
  else if (domain == "input_boolean") service = "input_boolean.toggle";
  else if (domain == "script") service = "script.turn_on";
  if (!service || dot == std::string::npos) {
    ESP_LOGW(TAG, "Select an action target: sensors are read-only");
    return;
  }
  if (!tessera::action(service, target))
    ESP_LOGW(TAG, "Action could not be submitted");
  // Never invent a sensor state or display success before HA confirms it.
}
}  // namespace esphome::desktop_launcher
