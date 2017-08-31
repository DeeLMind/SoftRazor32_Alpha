#pragma once

#define ASM __declspec(naked)
#define WIN32_LEAN_AND_MEAN
#define ISOLATION_AWARE_ENABLED 1

#include <SDKDDKVer.h>
#include <windows.h>
#include <commctrl.h>
#include <OleAuto.h>
#include <tchar.h>