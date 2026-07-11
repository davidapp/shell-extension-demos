# 09 · 预览处理器（IPreviewHandler）

对应教程：**《Windows Shell 扩展编程完全指南》第 ⑨ 章**。

在资源管理器「预览窗格」里渲染 `.myp` 文件内容（把内容当文本显示在只读 EDIT 里）。

## 涉及接口

- **`IInitializeWithStream`** —— 拿文件流。
- **`IPreviewHandler`** —— `SetWindow`/`SetRect`（父窗口与区域）、`DoPreview`（建子窗口渲染）、`Unload`、`SetFocus`/`QueryFocus`/`TranslateAccelerator`。
- **`IObjectWithSite`** —— 宿主设置的站点。

## 构建 / 安装（管理员）

```bat
cmake -B build -A x64
cmake --build build --config Release
```
```powershell
.\install.ps1
.\install.ps1 -Uninstall
```

测试：新建 `test.myp`（随便写点文本），在资源管理器里选中它并打开预览窗格（`Alt+P`）。

## 要点

- 预览处理器**跑在独立宿主 `prevhost.exe`** 里——靠给 CLSID 挂 `AppID = {534A1E02-D58F-44f0-B58B-36CBED287C7C}`（64 位 prevhost）实现隔离，崩溃不影响资源管理器。
- 三处注册：CLSID 的 `AppID` 值、`HKLM\...\PreviewHandlers`（值名=CLSID）、文件类型的 `shellex\{8895b1c6-…}`。
- `DoPreview` 里在 `SetWindow` 给的父窗口内建子窗口；`Unload` 要销毁窗口、释放流。

## 参考

- [IPreviewHandler](https://learn.microsoft.com/zh-cn/windows/win32/api/shobjidl_core/nn-shobjidl_core-ipreviewhandler) · [预览处理器](https://learn.microsoft.com/zh-cn/windows/win32/shell/preview-handlers)
- [RecipePreviewHandler 官方示例](https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/winui/shell/appshellintegration/RecipePreviewHandler)
