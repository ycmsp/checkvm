#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef int (*FN_IsVM)();
typedef int (*FN_DetectEx)(int*,int*,int*,int*,int*,int*,char*,int);

void log(const char* fmt, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    printf("%s", buf);
    fflush(stdout);
}

void test_dll(const char* dllName) {
    log("========================================\n");
    log("  LoadLibrary(\"%s\")\n", dllName);
    log("========================================\n\n");

    HMODULE hDll = LoadLibraryA(dllName);
    if (!hDll) {
        log("  [FAIL] LoadLibrary failed! GetLastError=%lu\n\n", GetLastError());
        return;
    }
    log("  [OK] Loaded successfully\n\n");

    FN_IsVM IsVM = (FN_IsVM)GetProcAddress(hDll, "IsVirtualMachine");
    FN_DetectEx DetectEx = (FN_DetectEx)GetProcAddress(hDll, "DetectVmEx");

    log("[Test 1] IsVirtualMachine()\n");
    if (!IsVM) {
        log("  [FAIL] GetProcAddress for IsVirtualMachine failed\n\n");
    } else {
        int r = IsVM();
        const char* label = (r == 1) ? "VIRTUAL MACHINE" : (r == 0) ? "PHYSICAL MACHINE" : "ERROR";
        log("  Return: %d  =>  %s\n\n", r, label);
    }

    log("[Test 2] DetectVmEx()\n");
    if (!DetectEx) {
        log("  [FAIL] GetProcAddress for DetectVmEx failed\n\n");
    } else {
        int isVm = -1, c1 = -1, c2 = -1, c3 = -1, c4 = -1, c5 = -1;
        char json[512] = {0};
        int ret = DetectEx(&isVm, &c1, &c2, &c3, &c4, &c5, json, sizeof(json));
        log("  Return: %d (0=success)\n", ret);
        if (ret == 0) {
            log("  isVm     = %d  (%s)\n", isVm, isVm ? "VIRTUAL" : "PHYSICAL");
            log("  cond1..5 = [%d, %d, %d, %d, %d]\n", c1, c2, c3, c4, c5);
            if (json[0]) log("  JSON     : %s\n", json);
        }
        log("\n");
    }

    FreeLibrary(hDll);
    log("  [OK] FreeLibrary done\n\n");
}

int main() {
    log("\n");
    log("  CheckVM DLL Call Demo\n");
    log("  =====================\n\n");

    test_dll("checkvm.dll");

    if (GetFileAttributesA("checkvm-x86.dll") != INVALID_FILE_ATTRIBUTES) {
        test_dll("checkvm-x86.dll");
    } else {
        log("  (checkvm-x86.dll not found, skipping x86 test)\n\n");
    }
    return 0;
}
