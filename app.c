#include "masterbeam.h"
#include "masterbeam_icons.h"

#define TAG "Masterbeam"

/**
 * @brief      Allocate the masterbeam application.
 * @details    This function allocates the masterbeam application resources.
 * @return     MasterbeamApp object.
*/
static MasterbeamApp* masterbeam_app_alloc() {
    MasterbeamApp* app = (MasterbeamApp*)malloc(sizeof(MasterbeamApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&masterbeam_scene_handlers, app);

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_attach_next_scene(app->scene_manager, MasterbeamSceneStart);

    return app;
}

/**
 * @brief      Free the masterbeam application.
 * @details    This function frees the masterbeam application resources.
 * @param      app  The masterbeam application object.
*/
static void masterbeam_app_free(MasterbeamApp* app) {
    furi_record_close(RECORD_GUI);
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);
    free(app);
}

/**
 * @brief      Main function for masterbeam application.
 * @details    This function is the entry point for the masterbeam application.  It should be defined in
 *           application.fam as the entry_point setting.
 * @param      _p  Input parameter - unused
 * @return     0 - Success
*/
int32_t main_masterbeam(void* _p) {
    UNUSED(_p);

    MasterbeamApp* app = masterbeam_app_alloc();

    view_dispatcher_run(app->view_dispatcher);

    masterbeam_app_free(app);

    return 0;
}
