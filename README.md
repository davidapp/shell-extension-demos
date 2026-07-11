# shell-extension-demos

《Windows Shell 扩展编程完全指南》配套示例代码 —— 面向 **Windows 10 / 11** 的 Shell 扩展（右键菜单、缩略图、图标、属性、预览、拖放……）最小可运行示例。

- 配套教程：<https://daiw.net/manual/windows-shell-ext>
- 每个示例一个独立目录，自带 `CMakeLists.txt`，可单独构建、安装、卸载。

## 目录结构

| 目录 | 对应章节 | 处理器 / 接口 |
| --- | --- | --- |
| [`01-context-menu/`](01-context-menu/) | ① 右键菜单扩展 | `IContextMenu` + `IShellExtInit` |
| …（后续章节陆续补齐） | | |

## 环境要求

- **Windows 10 / 11**。强烈建议在**虚拟机**里安装 / 调试：进程内扩展一旦崩溃或死锁，会拖垮资源管理器（Explorer）甚至整个桌面。
- **Visual Studio 2022**，勾选「使用 C++ 的桌面开发」工作负载 + 较新 Windows SDK。
- **CMake ≥ 3.20**（VS2022 已内置）。

## 构建（以 `01-context-menu` 为例）

```bat
cd 01-context-menu
cmake -B build -A x64
cmake --build build --config Release
```

> 现代资源管理器是 **64 位**进程，务必构建 **x64**，否则 Explorer 加载不了你的 DLL。
> 也可以直接用 VS2022「打开本地文件夹」打开示例目录，它会识别 CMake 工程。

## 安装 / 卸载（需**管理员**）

示例都用自注册（`regsvr32`）方式安装：

```powershell
# 注册（在管理员 PowerShell / 命令提示符里）
regsvr32 .\build\Release\ContextMenuExt.dll
# 卸载
regsvr32 /u .\build\Release\ContextMenuExt.dll
```

或运行各目录下的 `install.ps1`（会自动找到刚构建出的 DLL）。装完右键任意文件就能看到效果 —— 在 **Windows 11** 上位于「**显示更多选项**」里的经典菜单（进入 Win11 主菜单需要 MSIX 打包，见教程第 ③ 章）。

## 许可

MIT
