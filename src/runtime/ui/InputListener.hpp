#pragma once

#include <core/Core.hpp>
#include <ui/InputBindings.hpp>

namespace jaccen
{
    struct InputListener
    {
        std::shared_ptr<const InputActions*> input;
    };
}