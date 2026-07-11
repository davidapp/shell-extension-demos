#pragma once
#include <windows.h>

// DLL 级引用计数：只要还有存活的对象或被 LockServer 锁住，DllCanUnloadNow 就返回 S_FALSE，
// 系统便不会卸载本 DLL。
extern long g_cDllRef;

inline void DllAddRef() { InterlockedIncrement(&g_cDllRef); }
inline void DllRelease() { InterlockedDecrement(&g_cDllRef); }
