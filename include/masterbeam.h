#pragma once

#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/gui.h>
#include <gui/modules/menu.h>
#include <gui/modules/text_box.h>

typedef enum {
    MasterbeamSceneMainMenu,
    MasterbeamSceneConnection,
    MasterbeamSceneAbout,
    MasterbeamSceneMainPanel,
    MasterbeamSceneCount,
} MasterbeamScene;

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    Menu* menu_main;
    TextBox* text_box;
    bool is_connected;
} MasterbeamApp;

typedef struct {
    FuriThread* thread;
    bool is_connected;
    FuriMutex* mutex;
} BleContext;
