#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/menu.h>
#include <input/input.h>

#include "../include/masterbeam.h"

void scene_main_panel_on_enter(void* context) {
    MasterbeamApp* app = (MasterbeamApp*)context;
    Menu* menu = app->menu_main;

    menu_reset(menu);

    menu_add_item(menu, "Feature 1 (Coming Soon)", NULL, 0, NULL, app);
    menu_add_item(menu, "Feature 2 (Coming Soon)", NULL, 1, NULL, app);

    menu_set_selected_item(menu, 0);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);
}

bool scene_main_panel_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void scene_main_panel_on_exit(void* context) {
    MasterbeamApp* app = (MasterbeamApp*)context;
    menu_reset(app->menu_main);
}
