# 08 · 属性表处理器（IShellPropSheetExt）

对应教程：**《Windows Shell 扩展编程完全指南》第 ⑧ 章**。

往 `.txt` 文件的「属性」对话框里加一个自定义标签页，显示文件路径与大小。

## 涉及接口

- **`IShellExtInit`** —— 拿被右键 / 查看属性的文件。
- **`IShellPropSheetExt::AddPages`** —— 用 `CreatePropertySheetPage` 造页并交给 Shell；页是一个对话框（`PropSheet.rc` 里的模板 + `DlgProc`）。

## 构建 / 安装（管理员）

```bat
cmake -B build -A x64
cmake --build build --config Release
```
```powershell
.\install.ps1
.\install.ps1 -Uninstall
```

装好后右键 `.txt` → 属性，多出一个「Shell 示例」标签页。

## 要点

- 页对话框模板用 `WS_CHILD | DS_CONTROL`（子对话框），标题经 `PROPSHEETPAGE.pszTitle` 给。
- 用 `PSP_USECALLBACK` + `pfnCallback` 管理生命周期：`PSPCB_ADDREF` 里 `DllAddRef` 保活，`PSPCB_RELEASE` 里释放并回收传给页的数据。
- `.rc` 里只放 ASCII（避免 rc.exe 中文编码问题），中文标题/正文在运行时 `SetDlgItemText` 填。

## 参考

- [IShellPropSheetExt](https://learn.microsoft.com/zh-cn/windows/win32/api/shobjidl_core/nn-shobjidl_core-ishellpropsheetext) · [属性表处理器](https://learn.microsoft.com/zh-cn/windows/win32/shell/propsheet-handlers)
