Update #include "../../vendor/patch/patch.h" to match your own file location. - เปลี่ยนเป็นตำแหน่งของคุณ

void (*NvUtilInit)();
void NvUtilInit_hook()
{
    NvUtilInit();

	uintptr_t addr1 = g_libGTASA + (VER_x32 ? 0x679698 : 0x850D50); // StorageRootBuffer
	uintptr_t addr2 = g_libGTASA + (VER_x32 ? 0x679984 : 0x851328); // StorageBaseRootBuffer

    char* original = *(char**)addr1; // อ่านค่าปัจจุบันให้ถูกต้อง
    if (!original) original = *(char**)addr2;
    g_pszStorageOriginal = original; // เก็บค่าเดิมไว้เป็น fallback
    g_pszStorage = original;         // ชี้ไปยังค่าเดิมก่อน

    // Storage paths
    static char sStorageChosen[256] = "/storage/emulated/0/Android/Test/files/";
    
    // Always use the chosen storage path
    const char* picked = sStorageChosen;

    // Apply chosen root into lib and globals
    Patch::UnFuck(addr1);
    *(char**)addr1 = (char*)picked;
    Patch::UnFuck(addr2);
    *(char**)addr2 = (char*)picked;
    g_pszStorage = (char*)picked;

    Log::print("[INFO]: StorageRoot original: %s", (g_pszStorageOriginal ? g_pszStorageOriginal : "<null>"));
    Log::print("[INFO]: StorageRoot override: %s", g_pszStorage);

    CSettings::LoadSettings();

    firebase::crashlytics::SetCustomKey("build data", __DATE__);
    firebase::crashlytics::SetCustomKey("build time", __TIME__);

    char str[100];

    sprintf(str, "0x%x", g_libGTASA);
    firebase::crashlytics::SetCustomKey("libGTASA.so", str);
    if (g_pszStorageOriginal) firebase::crashlytics::SetCustomKey("storage.root.original", g_pszStorageOriginal);
    if (g_pszStorage) firebase::crashlytics::SetCustomKey("storage.root", g_pszStorage);

    sprintf(str, "0x%x", g_libSAMP);
    firebase::crashlytics::SetCustomKey("libsamp.so", str);
}

void InstallSpecialHooks()
{
    Patch::InlineHook("_Z10NvUtilInitv", &NvUtilInit_hook, &NvUtilInit);
}
