#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/menu.h>
#include <gui/modules/text_box.h>

#include "../include/masterbeam.h"

#define TAG "Masterbeam"

// Forward declarations for scene handlers
extern void scene_main_menu_on_enter(void* context);
extern bool scene_main_menu_on_event(void* context, SceneManagerEvent event);
extern void scene_main_menu_on_exit(void* context);

extern void scene_about_on_enter(void* context);
extern bool scene_about_on_event(void* context, SceneManagerEvent event);
extern void scene_about_on_exit(void* context);

extern void scene_main_panel_on_enter(void* context);
extern bool scene_main_panel_on_event(void* context, SceneManagerEvent event);
extern void scene_main_panel_on_exit(void* context);

// Scene on_enter handlers array
static void (*const masterbeam_on_enter_handlers[])(void*) = {
    scene_main_menu_on_enter,
    scene_about_on_enter,
    scene_main_panel_on_enter,
};

// Scene on_event handlers array
static bool (*const masterbeam_on_event_handlers[])(void*, SceneManagerEvent) = {
    scene_main_menu_on_event,
    scene_about_on_event,
    scene_main_panel_on_event,
};

// Scene on_exit handlers array
static void (*const masterbeam_on_exit_handlers[])(void*) = {
    scene_main_menu_on_exit,
    scene_about_on_exit,
    scene_main_panel_on_exit,
};

// Scene handlers configuration
static const SceneManagerHandlers masterbeam_scene_handlers = {
    .on_enter_handlers = masterbeam_on_enter_handlers,
    .on_event_handlers = masterbeam_on_event_handlers,
    .on_exit_handlers = masterbeam_on_exit_handlers,
    .scene_num = MasterbeamSceneCount,
};

// Custom event callback
static bool masterbeam_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    MasterbeamApp* app = (MasterbeamApp*)context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

// Back event callback
static bool masterbeam_back_event_callback(void* context) {
    furi_assert(context);
    MasterbeamApp* app = (MasterbeamApp*)context;
    return scene_manager_handle_back_event(app->scene_manager);
}

// App allocation
static MasterbeamApp* masterbeam_app_alloc(void) {
    MasterbeamApp* app = malloc(sizeof(MasterbeamApp));

    // Allocate view dispatcher and scene manager
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&masterbeam_scene_handlers, app);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, masterbeam_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, masterbeam_back_event_callback);

    // Open GUI
    app->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeWindow);

    // Allocate views
    app->menu_main = menu_alloc();
    view_dispatcher_add_view(app->view_dispatcher, 0, menu_get_view(app->menu_main));

    app->text_box = text_box_alloc();
    view_dispatcher_add_view(app->view_dispatcher, 1, text_box_get_view(app->text_box));

    // Allocate BLE context
    app->ble_ctx = ble_context_alloc();

    return app;
}

// App deallocation
static void masterbeam_app_free(MasterbeamApp* app) {
    if(!app) return;

    // Free BLE context
    if(app->ble_ctx) {
        ble_context_free(app->ble_ctx);
    }

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, 0);
    menu_free(app->menu_main);

    view_dispatcher_remove_view(app->view_dispatcher, 1);
    text_box_free(app->text_box);

    // Free scene manager and view dispatcher
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    // Close GUI
    furi_record_close(RECORD_GUI);

    free(app);
}

// App entry point
int32_t masterbeam_app(void* p) {
    UNUSED(p);

    MasterbeamApp* app = masterbeam_app_alloc();

    // Go to main menu scene
    scene_manager_next_scene(app->scene_manager, MasterbeamSceneMainMenu);

    // Start view dispatcher
    view_dispatcher_run(app->view_dispatcher);

    masterbeam_app_free(app);

    return 0;
}
