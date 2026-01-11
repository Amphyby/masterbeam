#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/text_box.h>
#include <gui/view.h>
#include <input/input.h>

#include "../include/masterbeam.h"

typedef enum {
    MainPanelCommandNone,
    MainPanelCommandLeft,
    MainPanelCommandRight,
    MainPanelCommandUp,
    MainPanelCommandDown,
    MainPanelCommandOk,
} MainPanelCommand;

typedef struct {
    MainPanelCommand last_command;
} SceneMainPanelData;

static void update_panel_display(MasterbeamApp* app, SceneMainPanelData* scene_data) {
    TextBox* text_box = app->text_box;
    text_box_reset(text_box);
    text_box_set_focus(text_box, TextBoxFocusEnd);

    char display_text[256];
    snprintf(
        display_text,
        sizeof(display_text),
        "Control Panel\n\n"
        "Masterbeam service active\n\n"
        "Use buttons to send commands:\n"
        "UP/DOWN/LEFT/RIGHT/OK\n\n"
        "Last: %s",
        scene_data->last_command == MainPanelCommandNone  ? "None" :
        scene_data->last_command == MainPanelCommandLeft  ? "LEFT" :
        scene_data->last_command == MainPanelCommandRight ? "RIGHT" :
        scene_data->last_command == MainPanelCommandUp    ? "UP" :
        scene_data->last_command == MainPanelCommandDown  ? "DOWN" :
        scene_data->last_command == MainPanelCommandOk    ? "OK" :
                                                            "Unknown");
    text_box_set_text(text_box, display_text);
}

// Input event callback to capture button presses
static bool scene_main_panel_input_callback(InputEvent* event, void* context) {
    MasterbeamApp* app = (MasterbeamApp*)context;
    SceneMainPanelData* scene_data = (SceneMainPanelData*)scene_manager_get_scene_state(
        app->scene_manager, MasterbeamSceneMainPanel);

    if(event->type != InputTypePress) {
        return false;
    }

    const char* command_str = NULL;

    switch(event->key) {
    case InputKeyLeft:
        scene_data->last_command = MainPanelCommandLeft;
        command_str = "LEFT";
        break;
    case InputKeyRight:
        scene_data->last_command = MainPanelCommandRight;
        command_str = "RIGHT";
        break;
    case InputKeyUp:
        scene_data->last_command = MainPanelCommandUp;
        command_str = "UP";
        break;
    case InputKeyDown:
        scene_data->last_command = MainPanelCommandDown;
        command_str = "DOWN";
        break;
    case InputKeyOk:
        scene_data->last_command = MainPanelCommandOk;
        command_str = "OK";
        break;
    default:
        return false;
    }

    if(command_str) {
        // Send command via BLE
        ble_send_command(app->ble_ctx, command_str);
        update_panel_display(app, scene_data);
        return true;
    }

    return false;
}

void scene_main_panel_on_enter(void* context) {
    MasterbeamApp* app = (MasterbeamApp*)context;

    // Allocate scene data
    SceneMainPanelData* scene_data = malloc(sizeof(SceneMainPanelData));
    scene_data->last_command = MainPanelCommandNone;
    scene_manager_set_scene_state(
        app->scene_manager, MasterbeamSceneMainPanel, (uint32_t)scene_data);

    // Set up text box display
    update_panel_display(app, scene_data);

    // Set the text box view with input callback
    View* view = text_box_get_view(app->text_box);
    view_set_input_callback(view, scene_main_panel_input_callback);
    view_set_context(view, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, 1);
}

bool scene_main_panel_on_event(void* context, SceneManagerEvent event) {
    MasterbeamApp* app = (MasterbeamApp*)context;
    SceneMainPanelData* scene_data = (SceneMainPanelData*)scene_manager_get_scene_state(
        app->scene_manager, MasterbeamSceneMainPanel);

    // Handle custom events (button presses)
    if(event.type == SceneManagerEventTypeCustom) {
        uint32_t button_code = event.event;
        const char* command_str = NULL;

        switch(button_code) {
        case 0: // LEFT
            scene_data->last_command = MainPanelCommandLeft;
            command_str = "LEFT";
            break;
        case 1: // RIGHT
            scene_data->last_command = MainPanelCommandRight;
            command_str = "RIGHT";
            break;
        case 2: // UP
            scene_data->last_command = MainPanelCommandUp;
            command_str = "UP";
            break;
        case 3: // DOWN
            scene_data->last_command = MainPanelCommandDown;
            command_str = "DOWN";
            break;
        case 4: // OK
            scene_data->last_command = MainPanelCommandOk;
            command_str = "OK";
            break;
        default:
            return false;
        }

        if(command_str) {
            // Send command via BLE
            ble_send_command(app->ble_ctx, command_str);
            update_panel_display(app, scene_data);
            return true;
        }
    }

    return false;
}

void scene_main_panel_on_exit(void* context) {
    MasterbeamApp* app = (MasterbeamApp*)context;
    SceneMainPanelData* scene_data = (SceneMainPanelData*)scene_manager_get_scene_state(
        app->scene_manager, MasterbeamSceneMainPanel);

    if(scene_data) {
        free(scene_data);
    }

    text_box_reset(app->text_box);
}
