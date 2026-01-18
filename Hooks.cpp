#include "../../vendor/patch/patch.h"

void (*NvUtilInit)();
void NvUtilInit_hook() {
    NvUtilInit();

    // 1. Setup Path
    static const char* path = "/storage/emulated/0/Android/Test/files/";
    g_pszStorage = path;

    for (auto off : { VER_x32 ? 0x6D687C : 0x8B46A8, VER_x32 ? 0x6796A0 : 0x850D50 }) {
        uintptr_t addr = g_libGTASA + off;
        Patch::UnFuck(addr);
        *(const char**)addr = path;
    }

    Log::print("[INFO] Storage: %s", path);
    CSettings::LoadSettings();

    // 2. Crashlytics
    using namespace firebase::crashlytics;
    SetCustomKey("storage.root", path);
    SetCustomKey("libGTASA.so", ("0x" + std::to_string(g_libGTASA)).c_str());
    SetCustomKey("libsamp.so",  ("0x" + std::to_string(g_libSAMP)).c_str());
}

void InstallSpecialHooks() {
    Patch::InlineHook("_Z10NvUtilInitv", &NvUtilInit_hook, &NvUtilInit);
}
