#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    // Add any app-wide data here
} MasterbeamApp;
