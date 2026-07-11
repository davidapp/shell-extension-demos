# 02 · 现代命令模型（IExplorerCommand）

对应教程：**《Windows Shell 扩展编程完全指南》第 ② 章**。

和第 1 章一样右键弹「显示文件信息」，但改用现代的 **`IExplorerCommand`** 接口实现：命令的标题 / 图标 / 状态 / 执行拆成独立方法，选中项通过 `IShellItemArray` 传入，不再需要 `IShellExtInit`。注册也换成 `shell\<verb>` 下的 **`ExplorerCommandHandler`** 值。

> 从本章起，COM 地基（类工厂、引用计数、注册表工具）抽到了仓库根的 [`common/ShellExtBase.h`](../common/ShellExtBase.h)，示例只保留处理器本体。

## 构建 / 安装（管理员）

```bat
cmake -B build -A x64
cmake --build build --config Release
```
```powershell
.\install.ps1              # 注册（右键任意文件可见；Win11 在“显示更多选项”里）
.\install.ps1 -Uninstall   # 卸载
```

## 要点

- `GetTitle` 返回的字符串用 `SHStrDupW`（`CoTaskMemAlloc` 分配），Shell 负责释放。
- `Invoke` 收到 `IShellItemArray`，遍历它拿每个 `IShellItem` 的 `SIGDN_FILESYSPATH`。
- 想做**级联子菜单**：`GetFlags` 返回 `ECF_HASSUBCOMMANDS`，并在 `EnumSubCommands` 里返回子命令枚举器。
- `IExplorerCommand` **只支持进程内激活**，且方法在 UI 线程调用 —— 别在里面做网络/阻塞操作。

## 与 Win11 主菜单的关系

用注册表注册的 `IExplorerCommand` 仍落在 Win11 的「显示更多选项」里。要进 **Win11 主菜单**，需要 MSIX / 稀疏包 + 包清单声明（复用的正是本章这个 `IExplorerCommand` 实现）——见第 ③ 章。

## 参考

- [IExplorerCommand](https://learn.microsoft.com/zh-cn/windows/win32/api/shobjidl_core/nn-shobjidl_core-iexplorercommand)
- [ExplorerCommandVerb 官方示例](https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/winui/shell/appshellintegration/ExplorerCommandVerb)
- [在 Windows 11 扩展右键菜单与共享对话框](https://blogs.windows.com/windowsdeveloper/2021/07/19/extending-the-context-menu-and-share-dialog-in-windows-11/)
