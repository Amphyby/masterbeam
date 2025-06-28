#include "../masterbeam_scene.h"
#include "../masterbeam.h"
#include <gui/modules/submenu.h>
#include <furi.h>

#define SUBMENU_ITEM_COUUNT = 1;

static void (*const masterbeam_scene_start_on_enter[])(void*) = {
    //furi_assert(context);
    // MasterbeamApp* app = context;
    // Submenu* submenu = submenu_alloc();
    // submenu_set_header(submenu, "Masterneam menu");
    // submenu_add_item(submenu, "Start scanning", 0, NULL, app);
    // view_dispatcher_switch_to_view(app->view_dispatcher, (uint32_t)submenu);
};

static bool (*const masterbeam_scene_start_on_event[])(void*, SceneManagerEvent) = {
    //return false; // events are not being handled so far
};

static void (*const masterbeam_scene_start_on_exit[])(void*) = {};

//Scene handlers
const SceneManagerHandlers masterbeam_scene_handlers = {
    .on_enter_handlers = masterbeam_scene_start_on_enter,
    .on_event_handlers = masterbeam_scene_start_on_event,
    .on_exit_handlers = masterbeam_scene_start_on_exit,
    .scene_num = MasterbeamSceneCount,
};
