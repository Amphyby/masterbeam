#pragma once

#include <furi.h>
#include <furi_hal_bt.h>
#include <bt/bt_service/bt.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <notification/notification.h>

#include "scenes/masterbeam_scene.h"

#define HID_BT_KEYS_STORAGE_NAME ".bt_masterbeam.keys"

typedef struct MasterbeamApp MasterbeamApp;
struct MasterbeamApp {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Widget* widget;
    FuriHalBleProfileBase* ble_hid_profile;
    Bt* bt;
    Gui* gui;
    NotificationApp* notifications;
    Popup* popup;
};
