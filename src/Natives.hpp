#pragma once

#include "Runtime.hpp"

namespace Natives
{
    /**
     * Generates bindings for the native function which will than be pushed to the managed side.
     */
    static void GenerateBindings(Runtime* runtime)
    {
        {
            runtime->NewArea();
            runtime->AddFunction((void*)&Runtime::ConsoleLog);
        }
    }
}
