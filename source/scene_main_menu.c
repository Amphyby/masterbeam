#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/menu.h>

#include "../include/masterbeam.h"
#include "masterbeam_icons.h"

static void scene_main_menu_callback(void* context, uint32_t index) {
    MasterbeamApp* app = (MasterbeamApp*)context;

    switch(index) {
    case 0: // Main Panel
        scene_manager_next_scene(app->scene_manager, MasterbeamSceneMainPanel);
        break;
    case 1: // About
        scene_manager_next_scene(app->scene_manager, MasterbeamSceneAbout);
        break;
    default:
        break;
    }
}

void scene_main_menu_on_enter(void* context) {
    MasterbeamApp* app = (MasterbeamApp*)context;
    Menu* menu = app->menu_main;

    menu_reset(menu);

    menu_add_item(menu, "Main Panel", &A_ControlPanel, 0, scene_main_menu_callback, app);

    menu_add_item(menu, "About", &A_About, 1, scene_main_menu_callback, app);

    menu_set_selected_item(menu, 0);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);
}

bool scene_main_menu_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void scene_main_menu_on_exit(void* context) {
    MasterbeamApp* app = (MasterbeamApp*)context;
    menu_reset(app->menu_main);
}
