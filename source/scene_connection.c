#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/menu.h>
#include <input/input.h>

#include "../include/masterbeam.h"

static void scene_connection_menu_callback(void* context, uint32_t index) {
    MasterbeamApp* app = (MasterbeamApp*)context;

    if(index == 0) {
        // Connect button pressed - initiate BLE connection
        app->is_connected = true;
        scene_manager_previous_scene(app->scene_manager);
    }
}

void scene_connection_on_enter(void* context) {
    MasterbeamApp* app = (MasterbeamApp*)context;
    Menu* menu = app->menu_main;

    menu_reset(menu);

    menu_add_item(menu, "Connect to Arduino", NULL, 0, scene_connection_menu_callback, app);

    menu_set_selected_item(menu, 0);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);
}

bool scene_connection_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void scene_connection_on_exit(void* context) {
    MasterbeamApp* app = (MasterbeamApp*)context;
    menu_reset(app->menu_main);
}
