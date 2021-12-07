#pragma once

#include "coreclr_delegates.h"
#include "hostfxr.h"

#include <iostream>
#include <string>
#include <vector>

#include <Windows.h>

#define DOTNET_RUNTIME_SUCCESS 0
#define DOTNET_RUNTIME_LOAD_HOSTFXR_FAILED 1
#define DOTNET_RUNTIME_NO_RUNTIME_CONFIG 2
#define DOTNET_RUNTIME_NO_RUNTIME_LIBRARY 3
#define DOTNET_RUNTIME_ASSEMBLY_FAILED 4
#define DOTNET_RUNTIME_NO_START 5
#define DOTNET_RUNTIME_NO_STOP 6

#define DLL_HANDLE HMODULE
#define DLL_LOAD LoadLibraryA
#define DLL_UNLOAD FreeLibrary
#define DLL_GETFUNCTION GetProcAddress

#if defined(_WIN32)
#define CORECLR_DELEGATE_CALLTYPE __stdcall
#else
#define CORECLR_DELEGATE_CALLTYPE
#endif

#include <filesystem>
namespace fs = std::filesystem;

typedef void (*push_areas_fn)(void* areas);
typedef int(CORECLR_DELEGATE_CALLTYPE* start_fn)(void*);
typedef int(CORECLR_DELEGATE_CALLTYPE* stop_fn)();

constexpr static int storage_size = 128;

class Runtime
{
private:
    std::vector<void*> areas;
    void** current_area = nullptr;
    int current_index = 0;

    std::string version;
    std::string dotnetPath;
    std::string managedDllName;
    std::string managedFriendlyName;
    std::string dotnetlib_path;

    load_assembly_and_get_function_pointer_fn load_assembly_and_get_function_pointer = nullptr;
    hostfxr_initialize_for_runtime_config_fn init_fptr = nullptr;
    hostfxr_get_runtime_delegate_fn get_delegate_fptr = nullptr;
    hostfxr_close_fn close_fptr = nullptr;

    start_fn bridgeStart = nullptr;
    stop_fn bridgeStop = nullptr;

    DLL_HANDLE pFxrModule = nullptr;

private:
    std::wstring StrToWStr(const std::string& Str)
    {
        return std::wstring(Str.begin(), Str.end());
    }

    DLL_HANDLE LoadHostFxrLibrary()
    {
        std::string hostfxr_path = dotnetPath;
        hostfxr_path = hostfxr_path.append("/host/fxr/").append(version).append("/hostfxr.dll");
        return DLL_LOAD(hostfxr_path.c_str());
    }

    bool LoadHostFxr()
    {
        pFxrModule = LoadHostFxrLibrary();
        if (!pFxrModule)
            return false;

        init_fptr = (hostfxr_initialize_for_runtime_config_fn)DLL_GETFUNCTION(pFxrModule,
                                                                              "hostfxr_initialize_for_runtime_config");
        get_delegate_fptr =
            (hostfxr_get_runtime_delegate_fn)DLL_GETFUNCTION(pFxrModule, "hostfxr_get_runtime_delegate");
        close_fptr = (hostfxr_close_fn)DLL_GETFUNCTION(pFxrModule, "hostfxr_close");
        return (init_fptr && get_delegate_fptr && close_fptr);
    }

    void UnloadHostFxr()
    {
        DLL_UNLOAD(pFxrModule);
    }

    load_assembly_and_get_function_pointer_fn GetDotnetLoadAssembly(const char_t* config_path)
    {
        void* ptr = nullptr;
        hostfxr_handle cxt = nullptr;
        int rc = init_fptr(config_path, nullptr, &cxt);
        if (rc != 0 || cxt == nullptr)
        {
            close_fptr(cxt);
            return nullptr;
        }

        rc = get_delegate_fptr(cxt, hdt_load_assembly_and_get_function_pointer, &ptr);
        if (rc != 0 || ptr == nullptr)
            return nullptr;

        close_fptr(cxt);
        return (load_assembly_and_get_function_pointer_fn)ptr;
    }

public:
    Runtime(std::string& version, std::string& dotnetPath, std::string& managedDllName,
                    std::string& managedFriendlyName)
        : version(version)
        , dotnetPath(dotnetPath)
        , managedDllName(managedDllName)
        , managedFriendlyName(managedFriendlyName)
    {
    }

    int Load()
    {
        if (!LoadHostFxr())
            return DOTNET_RUNTIME_LOAD_HOSTFXR_FAILED;

        const std::string runtimePath = std::string(dotnetPath) + "/" + managedDllName + ".runtimeconfig.json";
        if (!fs::exists(runtimePath))
            return DOTNET_RUNTIME_NO_RUNTIME_CONFIG;

        std::string dllPath = std::string(dotnetPath) + "/" + managedDllName + ".dll";
        if (!fs::exists(dllPath))
            return DOTNET_RUNTIME_NO_RUNTIME_LIBRARY;

        const std::string config_path = runtimePath;
        load_assembly_and_get_function_pointer = GetDotnetLoadAssembly(StrToWStr(config_path).c_str());

        if (load_assembly_and_get_function_pointer == nullptr)
            return DOTNET_RUNTIME_ASSEMBLY_FAILED;

        dotnetlib_path = dllPath;

        bridgeStart = (start_fn)GetFunction("Start");
        if (bridgeStart == nullptr)
            return DOTNET_RUNTIME_NO_START;

        bridgeStop = (stop_fn)GetFunction("Stop");
        if (bridgeStop == nullptr)
            return DOTNET_RUNTIME_NO_STOP;

        return DOTNET_RUNTIME_SUCCESS;
    }

    void Start(const std::string& currDir)
    {
        const char* cCurrDir = currDir.c_str();
        bridgeStart((void*) cCurrDir);
    }

    void Stop()
    {
        bridgeStop();
        UnloadHostFxr();
    }

    void* GetFunction(const std::string& managedFuncName, bool bUnmanagedOnly = true)
    {
        void* delegate = nullptr;
        int rc = load_assembly_and_get_function_pointer(
            StrToWStr(dotnetlib_path).c_str(), StrToWStr(managedFriendlyName).c_str(),
            StrToWStr(managedFuncName).c_str(), bUnmanagedOnly ? UNMANAGEDCALLERSONLY_METHOD : nullptr, nullptr,
            &delegate);
        return rc == 0 ? delegate : nullptr;
    }

    void NewArea()
    {
        current_area = new void*[storage_size];
        areas.push_back(current_area);
        current_index = 0;
    }

    void AddFunction(void* func)
    {
        current_area[current_index] = func;
        current_index++;
    }

    void PushAreas(const std::string& managedReceiverFunction)
    {
        size_t size = areas.size();
        void** pushing_areas = new void*[size];
        for (int i = 0; i < size; i++)
        {
            pushing_areas[i] = areas[i];
        }

        auto pushAreas = (push_areas_fn)GetFunction(managedReceiverFunction);
        if (pushAreas != nullptr)
        {
            pushAreas(pushing_areas);
        }

        delete[] pushing_areas;
    }

    static void ConsoleLog(const char* msg)
    {
        std::cout << msg;
    }
};
