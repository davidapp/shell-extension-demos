#pragma once
#include <windows.h>
#include <shobjidl.h>   // IExplorerCommand, IShellItemArray, EXPCMDSTATE/FLAGS

// 现代命令模型：一个 IExplorerCommand 对象就是“一条命令”。
// 相比 IContextMenu，它把“标题 / 图标 / 状态 / 执行”拆成独立方法，
// 选中项通过 IShellItemArray 参数传入（无需 IShellExtInit）。
class FileInfoCommand : public IExplorerCommand
{
public:
    FileInfoCommand();

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // IExplorerCommand
    IFACEMETHODIMP GetTitle(IShellItemArray* items, LPWSTR* ppszName) override;
    IFACEMETHODIMP GetIcon(IShellItemArray* items, LPWSTR* ppszIcon) override;
    IFACEMETHODIMP GetToolTip(IShellItemArray* items, LPWSTR* ppszInfotip) override;
    IFACEMETHODIMP GetCanonicalName(GUID* pguid) override;
    IFACEMETHODIMP GetState(IShellItemArray* items, BOOL fOkToBeSlow, EXPCMDSTATE* pState) override;
    IFACEMETHODIMP Invoke(IShellItemArray* items, IBindCtx* pbc) override;
    IFACEMETHODIMP GetFlags(EXPCMDFLAGS* pFlags) override;
    IFACEMETHODIMP EnumSubCommands(IEnumExplorerCommand** ppEnum) override;

private:
    ~FileInfoCommand();
    long m_cRef;
};
