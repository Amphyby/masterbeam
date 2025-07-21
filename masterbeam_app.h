#pragma once

#include "scenes/masterbeam_scene.h"

#define HID_BT_KEYS_STORAGE_NAME ".bt_masterbeam.keys"

typedef struct MasterbeamApp MasterbeamApp;
struct MasterbeamApp {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    FuriHalBleProfileBase* ble_hid_profile;
    Bt* bt;
    Gui* gui;
    NotificationApp* notifications;
    Popup* popup;
};
