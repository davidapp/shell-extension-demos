# 03 · Windows 11 新版右键菜单（MSIX / 稀疏包）

对应教程：**《Windows Shell 扩展编程完全指南》第 ③ 章**。

第 1、2 章的命令都只落在 Win11 的「显示更多选项」旧菜单里。本章**不写新处理器**，而是把第 2 章的 `IExplorerCommand` 用**稀疏包（sparse package）**赋予“包标识”，从而进入 Windows 11 的**主**右键菜单。

## 组成

| 文件 | 作用 |
| --- | --- |
| `dllmain.cpp` | 精简 COM 服务器；处理器直接复用 `../02-explorer-command/ExplorerCommand.cpp` |
| `app.manifest` | 嵌入 DLL 的并排清单（`msix` 元素绑定包标识）——**打包扩展必须有它** |
| `package/AppxManifest.xml` | 稀疏包清单：`com:SurrogateServer` 声明 DLL + `windows.fileExplorerContextMenus` 注册命令 |
| `build-package.ps1` | 一键：构建 → 打包 → 自签名 → 信任证书 → 注册 |

## 一键构建 + 注册（管理员 PowerShell，Win11 开发者模式）

```powershell
.\build-package.ps1
# 右键任意文件 → Win11 主菜单直接出现「显示文件信息」
.\build-package.ps1 -Uninstall
```

脚本做了 6 件事：构建带嵌入清单的 DLL → 组装外部位置目录（DLL + 占位资源）→ `makeappx pack /nv` 打稀疏包 → `New-SelfSignedCertificate` 自签 → `signtool` 签名并把公钥导入「受信任人」→ `Add-AppxPackage -ExternalLocation` 注册。

## 三个必踩的坑

1. **DLL 必须嵌入 `app.manifest`**（含 `msix` 标识，且 publisher/packageName/applicationId 与 `AppxManifest.xml` 完全一致），否则资源管理器不把它当已打包组件加载。CMake 用 `/MANIFEST:EMBED /MANIFESTINPUT` 完成。
2. **自签名证书要导入「受信任人」**（`Cert:\CurrentUser\TrustedPeople`），否则 `Add-AppxPackage` 报 `0x800B0109 CERT_E_UNTRUSTEDROOT`。
3. **`Publisher` 三处一致**：证书 Subject = `AppxManifest.xml` 的 `Identity/Publisher` = `app.manifest` 的 `msix/@publisher`。

## 参考

- [用外部位置为非打包应用授予包标识](https://learn.microsoft.com/zh-cn/windows/apps/desktop/modernize/grant-identity-to-nonpackaged-apps)
- [MakeAppx](https://learn.microsoft.com/zh-cn/windows/msix/package/create-app-package-with-makeappx-tool) · [创建证书](https://learn.microsoft.com/zh-cn/windows/msix/package/create-certificate-package-signing) · [SignTool 签名](https://learn.microsoft.com/zh-cn/windows/msix/package/sign-app-package-using-signtool)
- [PackageWithExternalLocation 官方示例](https://aka.ms/sparsepkgsample)
