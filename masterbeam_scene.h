#pragma once

#include <gui/scene_manager.h>

// Registering all scenes here
typedef enum {
    MasterbeamSceneStart,
    MasterbeamSceneCount,
} MasterbeamScene;

extern const SceneManagerHandlers masterbeam_scene_handlers;
