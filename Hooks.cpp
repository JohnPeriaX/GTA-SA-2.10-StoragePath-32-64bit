Update #include "../../vendor/patch/patch.h" to match your own file location. - เปลี่ยนเป็นตำแหน่งของคุณ

void (*NvUtilInit)();
void NvUtilInit_hook() {
    NvUtilInit();
    
    // Setup storage paths
    static char sStoragePath[] = "/storage/emulated/0/Android/Test/files/";
    uintptr_t addrs[] = {
        g_libGTASA + (VER_x32 ? 0x679698 : 0x850D50),
        g_libGTASA + (VER_x32 ? 0x679984 : 0x851328)
    };

    // Apply storage path
    for (auto addr : addrs) {
        Patch::UnFuck(addr);
        *(const char**)addr = sStoragePath;
    }
    g_pszStorage = sStoragePath;

    // Log and initialize
    Log::print("[INFO]: Storage path set to: %s", sStoragePath);
    CSettings::LoadSettings();
    
    // Crash reporting setup
    firebase::crashlytics::SetCustomKey("libGTASA.so", 
        (std::string("0x") + std::to_string(g_libGTASA)).c_str());
    firebase::crashlytics::SetCustomKey("libsamp.so", 
        (std::string("0x") + std::to_string(g_libSAMP)).c_str());
    firebase::crashlytics::SetCustomKey("storage.root", sStoragePath);
}

void InstallSpecialHooks()
{
    Patch::InlineHook("_Z10NvUtilInitv", &NvUtilInit_hook, &NvUtilInit);
}
