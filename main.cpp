#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include "And64InlineHook.hpp"

#define LOG_TAG "LongRangeRed"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Original Pointers
float (*orig_get_farClipPlane)(void* __this) = nullptr;
void (*orig_set_farClipPlane)(void* __this, float value) = nullptr;

// Hooked Functions
float hooked_get_farClipPlane(void* __this) {
    return 500.0f; // Pilitin nating maging 500 meters
}

void hooked_set_farClipPlane(void* __this, float value) {
    // Dedmahin natin pag binabago ng laro pabalik sa 1000m
}

// Helper para mag-patch ng memory
void MemoryPatch(void* address, const uint8_t* patch, size_t size) {
    uintptr_t pageStart = (uintptr_t)address & ~(0x1000 - 1);
    size_t pageSize = ((uintptr_t)address + size - pageStart + 0xFFF) & ~(0xFFF);
    mprotect((void*)pageStart, pageSize, PROT_READ | PROT_WRITE | PROT_EXEC);
    memcpy(address, patch, size);
    __builtin___clear_cache((char*)address, (char*)address + size);
}

__attribute__((constructor))
void on_load() {
    LOGI("Long Range Red Mod Loaded!");

    // 1. Setup libil2cpp.so (Para sa Far Clip Hooks)
    void* il2cpp_handle = dlopen("libil2cpp.so", RTLD_LAZY);
    if (il2cpp_handle) {
        Dl_info info;
        if (dladdr(il2cpp_handle, &info) != 0) {
            uintptr_t base = (uintptr_t)info.dli_fbase;
            
            // Hook Getter (0x502200C)
            void* target_get = (void*)(base + 0x502200C);
            A64HookFunction(target_get, (void*)hooked_get_farClipPlane, (void**)&orig_get_farClipPlane);
            
            // Hook Setter (0x5022028)
            void* target_set = (void*)(base + 0x5022028);
            A64HookFunction(target_set, (void*)hooked_set_farClipPlane, (void**)&orig_set_farClipPlane);
            
            LOGI("FarClip Hooks initialized!");
        }
    }

    // 2. Setup libunity.so (Para sa Red Wallhack)
    void* unity_handle = dlopen("libunity.so", RTLD_LAZY);
    if (unity_handle) {
        Dl_info info;
        if (dladdr(unity_handle, &info) != 0) {
            uintptr_t base = (uintptr_t)info.dli_fbase;
            
            // Patch Visibility (0x9677554) -> MOV X0, #1; RET
            uint8_t vis_patch[] = {0x20, 0x00, 0x80, 0xD2, 0xC0, 0x03, 0x5F, 0xD6};
            MemoryPatch((void*)(base + 0x9677554), vis_patch, sizeof(vis_patch));
            
            // Patch Color (0x8BC078C)
            uint8_t col_patch[] = {0x40, 0x00, 0x00, 0x1C, 0xC0, 0x03, 0x5F, 0xD6, 0x28, 0x6B, 0x6E, 0x4E};
            MemoryPatch((void*)(base + 0x8BC078C), col_patch, sizeof(col_patch));
            
            LOGI("Red Wallhack patches applied!");
        }
    }
}
