#pragma once

#include <core/Core.hpp>

namespace jaccen
{
    ENUM_FLAGS(InputAction)
    {
        None = 0,

        Camera_MoveForward = 1 << 0,
        Camera_MoveBackward = 1 << 1,
        Camera_MoveLeft = 1 << 2,
        Camera_MoveRight = 1 << 3,
        Camera_Zoom = 1 << 4,
        Camera_ToggleControl = 1 << 5,

        Gizmo_ChangeOperation = 1 << 10,
        Gizmo_ChangeMode = 1 << 11,

        Object_Select = 1 << 20,
        Object_ClearSelected = 1 << 21,
        Object_DeleteSelected = 1 << 22,
        Object_Spawn = 1 << 23,

        Quit = 1 << 30,
    };

    class InputBindings
    {

    };

    class InputActions
    {
    public:
        bool check(InputAction action) const;
    };
}
