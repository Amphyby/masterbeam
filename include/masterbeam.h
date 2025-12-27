#pragma once

#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/gui.h>
#include <gui/modules/menu.h>
#include <gui/modules/text_box.h>

typedef enum {
    MasterbeamSceneMainMenu,
    MasterbeamSceneAbout,
    MasterbeamSceneMainPanel,
    MasterbeamSceneCount,
} MasterbeamScene;

typedef struct {
    FuriThread* thread;
    FuriMutex* mutex;
} BleContext;

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    Menu* menu_main;
    TextBox* text_box;
    BleContext* ble_ctx;
} MasterbeamApp;

// BLE API functions
BleContext* ble_context_alloc(void);
void ble_context_free(BleContext* ble_ctx);
void ble_start_connection(BleContext* ble_ctx);
void ble_send_command(BleContext* ble_ctx, const char* command);
