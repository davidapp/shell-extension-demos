// 第 3 章不写新处理器，而是把第 2 章的 IExplorerCommand 复用进来、
// 编成一个带“包标识”的 DLL，再用稀疏包送进 Win11 主右键菜单。
#include <initguid.h>
#include "Guid.h"               // 复用 02-explorer-command 的 CLSID
#include "ExplorerCommand.h"    // 复用 02-explorer-command 的处理器
#include "common/ShellExtBase.h"
#include <windows.h>
#include <new>

HINSTANCE g_hInst = nullptr;

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        g_hInst = hInst;
        DisableThreadLibraryCalls(hInst);
    }
    return TRUE;
}

// 打包后由包清单的 com:SurrogateServer 激活，仍走标准的 DllGetClassObject。
// 无需 DllRegisterServer——注册由稀疏包完成。
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (rclsid != CLSID_FileInfoExplorerCommand)
        return CLASS_E_CLASSNOTAVAILABLE;
    auto* pFactory = new (std::nothrow) ClassFactory<FileInfoCommand>();
    if (!pFactory) return E_OUTOFMEMORY;
    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();
    return hr;
}

STDAPI DllCanUnloadNow()
{
    return (g_cDllRef == 0) ? S_OK : S_FALSE;
}
