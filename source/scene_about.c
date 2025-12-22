#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/text_box.h>

#include "../include/masterbeam.h"

void scene_about_on_enter(void* context) {
    MasterbeamApp* app = (MasterbeamApp*)context;
    TextBox* text_box = app->text_box;

    text_box_reset(text_box);
    text_box_set_font(text_box, TextBoxFontText);

    // Add the text content
    text_box_set_text(text_box, "Privyet zemlyane!");

    view_dispatcher_switch_to_view(app->view_dispatcher, 1);
}

bool scene_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void scene_about_on_exit(void* context) {
    MasterbeamApp* app = (MasterbeamApp*)context;
    text_box_reset(app->text_box);
}
