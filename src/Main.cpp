#include <iostream>
#include <RED4ext/RED4ext.hpp>

#include "Runtime.hpp"
#include "Natives.hpp"

Runtime* runtime = nullptr;
bool runtimeStarted = false;

RED4EXT_C_EXPORT bool RED4EXT_CALL Load(RED4ext::PluginHandle aHandle, const RED4ext::IRED4ext* aInterface)
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    std::filesystem::path current_dir = std::filesystem::current_path();
    std::cout << "Starting .NET Runtime on " << current_dir << "..." << std::endl;

    std::string workingDir = current_dir.string();
    std::string runtimeVersion = "5.0.9";
    std::string runtimeDir = (current_dir / "red4ext" / "dotnet").string();
    std::string runtimeDll = "runtime/RED4ext.NET";
    std::string runtimeFriendlyName = "RED4ext.NET.Runtime.Bridge, RED4ext.NET";

    runtime = new Runtime(runtimeVersion, runtimeDir, runtimeDll, runtimeFriendlyName);

    if (const int result = runtime->Load(); result != DOTNET_RUNTIME_SUCCESS)
    {
        std::cout << "Could not start Runtime! Err: " << result << std::endl;
        return false;
    }

    Natives::GenerateBindings(runtime);

    runtime->PushAreas("BindFunctions");
    runtime->Start(workingDir);
    runtimeStarted = true;

    std::cout << ".NET Runtime successfully started!" << std::endl;

    return true;
}

RED4EXT_C_EXPORT void RED4EXT_CALL PostLoad()
{
    /*
     * This function is called after all plugins are loaded. Use this to get interfaces to plugins you depends on.
     */
}

RED4EXT_C_EXPORT void RED4EXT_CALL Unload()
{
    if (runtimeStarted)
    {
        runtime->Stop();
    }
}

RED4EXT_C_EXPORT void RED4EXT_CALL Query(RED4ext::PluginInfo* aInfo)
{
    aInfo->name = L"RED4ext.NET";
    aInfo->author = L"DasDarki";
    aInfo->version = RED4EXT_SEMVER(1, 0, 0);
    aInfo->runtime = RED4EXT_RUNTIME_LATEST;
    aInfo->sdk = RED4EXT_SDK_LATEST;
}

RED4EXT_C_EXPORT uint32_t RED4EXT_CALL Supports()
{
    return RED4EXT_API_VERSION_LATEST;
}
