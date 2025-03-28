#include "../power_settings_app.h"

#include <lib/toolbox/value_index.h>

enum PowerSettingsSubmenuIndex {
    PowerSettingsSubmenuIndexBatteryInfo,
    PowerSettingsSubmenuIndexReboot,
    PowerSettingsSubmenuIndexOff,
    PowerSettingsSubmenuIndexAutoPowerOff,
};

#define AUTO_POWEROFF_DELAY_COUNT 13
const char* const auto_poweroff_delay_text[AUTO_POWEROFF_DELAY_COUNT] = {
    "OFF",
    "5min",
    "10min",
    "15min",
    "30min",
    "45min",
    "60min",
    "90min",
    "2h",
    "6h",
    "12h",
    "24h",
    "48h"};

const uint32_t auto_poweroff_delay_value[AUTO_POWEROFF_DELAY_COUNT] = {
    0,
    300000,
    600000,
    900000,
    1800000,
    2700000,
    3600000,
    5400000,
    7200000,
    21600000,
    43200000,
    86400000,
    172800000};

// change variable_item_list visible text and app_poweroff_delay_time_settings when user change item in variable_item_list
static void power_settings_scene_start_auto_poweroff_delay_changed(VariableItem* item) {
    PowerSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, auto_poweroff_delay_text[index]);
    app->settings.auto_poweroff_delay_ms = auto_poweroff_delay_value[index];
}

static void power_settings_scene_start_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    PowerSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void power_settings_scene_start_on_enter(void* context) {
    PowerSettingsApp* app = context;
    VariableItemList* variable_item_list = app->variable_item_list;

    VariableItem* item;
    uint8_t value_index;

    variable_item_list_add(variable_item_list, "电池信息", 1, NULL, NULL);

    variable_item_list_add(variable_item_list, "重启", 1, NULL, NULL);

    variable_item_list_add(variable_item_list, "关机", 1, NULL, NULL);

    item = variable_item_list_add(
        variable_item_list,
        "闲置时关机",
        AUTO_POWEROFF_DELAY_COUNT,
        power_settings_scene_start_auto_poweroff_delay_changed, //function for change visible item list value and app settings
        app);

    value_index = value_index_uint32(
        app->settings.auto_poweroff_delay_ms,
        auto_poweroff_delay_value,
        AUTO_POWEROFF_DELAY_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, auto_poweroff_delay_text[value_index]);

    variable_item_list_set_selected_item(
        variable_item_list,
        scene_manager_get_scene_state(app->scene_manager, PowerSettingsAppSceneStart));

    variable_item_list_set_enter_callback(
        variable_item_list, power_settings_scene_start_submenu_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, PowerSettingsAppViewVariableItemList);
}

bool power_settings_scene_start_on_event(void* context, SceneManagerEvent event) {
    PowerSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PowerSettingsSubmenuIndexBatteryInfo) {
            scene_manager_next_scene(app->scene_manager, PowerSettingsAppSceneBatteryInfo);
        } else if(event.event == PowerSettingsSubmenuIndexReboot) {
            scene_manager_next_scene(app->scene_manager, PowerSettingsAppSceneReboot);
        } else if(event.event == PowerSettingsSubmenuIndexOff) {
            scene_manager_next_scene(app->scene_manager, PowerSettingsAppScenePowerOff);
        }
        scene_manager_set_scene_state(app->scene_manager, PowerSettingsAppSceneStart, event.event);
        consumed = true;
    }
    return consumed;
}

void power_settings_scene_start_on_exit(void* context) {
    PowerSettingsApp* app = context;
    variable_item_list_reset(app->variable_item_list);
}
