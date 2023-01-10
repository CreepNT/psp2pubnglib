/*
Copyright (C) 2023 CreepNT

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static BOOL s_hasConfig = FALSE;

static BOOL s_IsNotSysApp = FALSE;
static BOOL s_IsFgApp = TRUE;
static BOOL s_IsFgPatch = FALSE;
static BOOL s_IsBgApp = FALSE;
static BOOL s_IsBgPatch = FALSE;

static WCHAR PathToConfig[MAX_PATH];

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        if (GetEnvironmentVariableW(L"PSP2PUB_NGLIB_CFG", PathToConfig, ARRAYSIZE(PathToConfig))) {
            HANDLE hFile = CreateFileW(PathToConfig, GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
            if (hFile != INVALID_HANDLE_VALUE) {
                DWORD configSz = 0;
                CHAR config[5];
                if (ReadFile(hFile, &config, 5, &configSz, NULL) && configSz == 5) {
                    do {
                        if (config[0] != '0' && config[0] != '1')
                            break;
                        if (config[1] != '0' && config[1] != '1')
                            break;
                        if (config[2] != '0' && config[2] != '1')
                            break;
                        if (config[3] != '0' && config[3] != '1')
                            break;
                        if (config[4] != '0' && config[4] != '1')
                            break;

                        s_IsNotSysApp = (config[0] == '1');
                        s_IsFgApp = (config[1] == '1');
                        s_IsFgPatch = (config[2] == '1');
                        s_IsBgApp = (config[3] == '1');
                        s_IsBgPatch = (config[4] == '1');
                        s_hasConfig = TRUE;
                    } while (0);
                }
                CloseHandle(hFile);
            }
        }
    }
    return TRUE;
}

typedef struct {
    DWORD id;
    LPCCH name;
} CategoryComboInfo;

static CategoryComboInfo comboInfo[] = {
    //From psp2pubprx
    { '@', "PS Vita Application" },
    { 'B', "PS Vita Application Patch"},
    { 'C', "PS Vita Additional Content"},

    //Guess from function at RVA 0x8C26C
    //Names from Checker, found by Graphene
    { 'A', "*PS Vita System Application" },  
    { 'F', "+PS Vita Non-Game Application" },
    { 'G', "+PS Vita BG Application" },
    { 'H', "+PS Vita Non-Game Application Patch" },
    { 'I', "+PS Vita BG Application Patch" },

    //{ 0, "PS3 'DG'" },
    //{ 2, "PS3 'GD'" },
    //{ 4, "PS3 Game Executable" },
    //{ 8, "PS3 Game Data" },
};

UINT32 WINAPI CNglibVersion(VOID) {
    //SDK 3.5x ships with Publishing Tools version 2.31
    return 0x2310000;
}

LPVOID WINAPI CNglibCategoryComboInfo(VOID) {
    return comboInfo;
}

UINT32 WINAPI CNglibCategoryComboInfoSize(VOID) {
    return ARRAYSIZE(comboInfo);
}

BOOL WINAPI CNglibSysfileGenecatIsPsp2NotSysApp(CHAR category) {
    return (s_hasConfig) ? s_IsNotSysApp : (category == '@');
}

BOOL WINAPI CNglibSysfileGenecatIsPsp2FgApp(CHAR category) {
    return (s_hasConfig) ? s_IsFgApp : (category == '@' || category == 'A' || category == 'F');
}

BOOL WINAPI CNglibSysfileGenecatIsPsp2FgPatch(CHAR category) {
    return (s_hasConfig) ? s_IsFgPatch : (category == 'B' || category == 'H');
}

BOOL WINAPI CNglibSysfileGenecatIsPsp2BgApp(CHAR category) {
    return (s_hasConfig) ? s_IsBgApp : (category == 'G');
}

BOOL WINAPI CNglibSysfileGenecatIsPsp2BgPatch(CHAR category) {
    return (s_hasConfig) ? s_IsBgPatch : (category == 'I');
}