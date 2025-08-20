#include "../masterbeam_app.h"

void masterbeam_scene_control_on_enter(void* context) {
    MasterbeamApp* app = context;
    view_dispatcher_switch_to_view(
        app->view_dispatcher,
        scene_manager_get_scene_state(app->scene_manager, MasterbeamSceneControl));
}

bool masterbeam_scene_control_on_event(void* context, SceneManagerEvent event) {
    MasterbeamApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);

    return consumed;
}

void masterbeam_scene_control_on_exit(void* context) {
    MasterbeamApp* app = context;
    UNUSED(app);
}
