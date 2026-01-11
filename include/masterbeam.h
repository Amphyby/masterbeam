#pragma once

#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/gui.h>
#include <gui/modules/menu.h>
#include <gui/modules/text_box.h>
#include <furi_ble/gatt.h>

typedef enum {
    MasterbeamSceneMainMenu,
    MasterbeamSceneAbout,
    MasterbeamSceneMainPanel,
    MasterbeamSceneCount,
} MasterbeamScene;

typedef struct {
    FuriThread* thread;
    FuriMutex* mutex;
    bool is_connected; // Connection status for Arduino (central) to read
    uint16_t svc_handle; // GATT service handle
    BleGattCharacteristicInstance char_status; // Status characteristic instance
    BleGattCharacteristicInstance char_command; // Command characteristic instance
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
bool ble_is_connected(BleContext* ble_ctx);
void ble_set_connected(BleContext* ble_ctx, bool connected);
