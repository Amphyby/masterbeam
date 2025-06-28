#pragma once

#include "../masterbeam_scene.h"

// Declare scene handlers
void masterbeam_scene_start_on_enter(void* context);
bool masterbeam_scene_start_on_event(void* context, SceneManagerEvenet event);
void masterbeam_scene_start_on_exit(void* context);
