#include "masterbeam_app.h"

MasterbeamApp* allocate() {
    MasterbeamApp* app = malloc(sizeof(MasterbeamApp));

    app->gui = furi_record_open(RECORD_GUI);

    app->bt = furi_record_open(RECORD_BT);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, hid_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, hid_back_event_callback);

    return app;
}

int32_t main_masterbeam(void* p) {
    UNUSED(p);

    MasterbeamApp* app = allocate();

    bt_disconnect(app->bt);

    return 0;
}
