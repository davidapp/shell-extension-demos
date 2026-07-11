# 01 · 右键菜单扩展（IContextMenu）

对应教程：**《Windows Shell 扩展编程完全指南》第 ① 章**。

一个最小的进程内 Shell 扩展：右键任意文件 → 「**显示文件信息**」→ 弹框显示该文件的完整路径与字节大小。它把 COM/DLL 的地基走通，是后面所有章节的模板。

## 涉及接口

| 接口 | 作用 |
| --- | --- |
| `IUnknown` + 类工厂 | COM 对象的基本盘 |
| `IShellExtInit` | Shell 把「被右键的文件」通过 `IDataObject` 交给我们 |
| `IContextMenu` | `QueryContextMenu` 加菜单项、`InvokeCommand` 执行、`GetCommandString` 提供 verb/帮助 |

## 文件一览

| 文件 | 内容 |
| --- | --- |
| `Guid.h` | 本扩展的 CLSID（真实项目请重新生成） |
| `dllmain.cpp` | `DllMain` / `DllGetClassObject` / `DllCanUnloadNow` + 自注册 `DllRegisterServer`/`DllUnregisterServer` |
| `ClassFactory.*` | 类工厂 |
| `ContextMenuHandler.*` | 处理器本体（三个接口的实现） |
| `ContextMenuExt.def` | 导出四个标准入口 |
| `ModuleRef.h` | DLL 级引用计数 |

## 构建

```bat
cmake -B build -A x64
cmake --build build --config Release
```

> 必须 **x64**（现代资源管理器是 64 位进程）。

## 安装 / 测试 / 卸载（管理员）

```powershell
# 注册（管理员 PowerShell）
.\install.ps1
# —— 右键任意文件，Win11 在「显示更多选项」里能看到「显示文件信息」——
# 卸载
.\install.ps1 -Uninstall
```

也可直接 `regsvr32 .\build\Release\ContextMenuExt.dll`（`/u` 卸载）。

<!-- 提示：装扩展前后如果资源管理器没刷新，可在任务管理器里重启「Windows 资源管理器」。 -->

## Windows 11 说明

用注册表注册的经典 `IContextMenu` 处理器，在 **Win11 只出现在「显示更多选项」的旧菜单**里。要让命令进入 **Win11 主右键菜单**，需要改用 `IExplorerCommand` 并通过 **MSIX / 稀疏包**注册 —— 见教程第 ② / ③ 章。

## 参考文档

- [创建快捷（右键）菜单处理器](https://learn.microsoft.com/zh-cn/windows/win32/shell/context-menu-handlers)
- [IContextMenu](https://learn.microsoft.com/zh-cn/windows/win32/api/shobjidl_core/nn-shobjidl_core-icontextmenu)
- [IShellExtInit](https://learn.microsoft.com/zh-cn/windows/win32/api/shobjidl_core/nn-shobjidl_core-ishellextinit)
- [注册 Shell 扩展处理器](https://learn.microsoft.com/zh-cn/windows/win32/shell/handlers#registering-shell-extension-handlers)
