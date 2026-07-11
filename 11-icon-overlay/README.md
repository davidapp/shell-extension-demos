# 示例 11 —— 图标叠加处理器（IShellIconOverlayIdentifier）

第 ⑪ 章配套代码。给符合条件的文件在图标**左下角**叠一个小标记——OneDrive / Dropbox / TortoiseGit 的同步状态角标就是这一类扩展。

## 接口

只实现一个接口 `IShellIconOverlayIdentifier`，三个方法：

- `GetOverlayInfo` —— 叠加图标从哪个文件、第几个图标取。
- `GetPriority` —— 多个叠加命中同一文件时的排序优先级（0 最高）。
- `IsMemberOf(path, attrib)` —— **核心**：这个文件要不要叠标记？`S_OK` 叠，`S_FALSE` 不叠。

本示例规则：**文件名里含 `demo`（不分大小写）就叠**。图标用系统 `shell32.dll` 占位，真实项目请换成自带的 `.ico`。

## 构建

```powershell
cmake -B build -A x64
cmake --build build --config Release
```

## 注册 / 卸载（需**管理员**）

```powershell
.\install.ps1
# 关键：叠加处理器要重启资源管理器才生效
Stop-Process -Name explorer -Force
.\install.ps1 -Uninstall
```

## 两个坑

- **只有前 15 个生效**：系统对叠加处理器有硬上限，注册键按名字排序取前 15 个。本示例名字前置一个空格（` DaiwDemoOverlay`）来抢靠前的槽位——这是 TortoiseGit / Dropbox 都在用的招。
- **`IsMemberOf` 必须快**：它对每个显示的文件都会被调用，任何阻塞（读文件、查网络）都会让整个资源管理器卡顿。真实扩展通常查一份内存缓存。

注册位置：`HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\ShellIconOverlayIdentifiers\ DaiwDemoOverlay`。
