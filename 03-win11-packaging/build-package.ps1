# 构建并注册 Win11 右键菜单稀疏包（自签名，仅供本机开发测试）。
# 需在【管理员】PowerShell、且开了「开发者模式」的 Windows 11 上运行。
#   .\build-package.ps1             # 构建 + 打包 + 签名 + 注册
#   .\build-package.ps1 -Uninstall  # 卸载
param([switch]$Uninstall)
$ErrorActionPreference = "Stop"
$root      = $PSScriptRoot
$pkgName   = "DavidApp.ShellExtDemos.Win11Menu"
$publisher = "CN=DavidApp Shell Ext Demos"   # 必须与 AppxManifest.xml 的 Publisher 一致

if ($Uninstall) {
    Get-AppxPackage $pkgName -ErrorAction SilentlyContinue | Remove-AppxPackage
    Write-Host "已卸载 $pkgName（可能需重启资源管理器）。"
    return
}

# 1) 构建带嵌入标识清单的 DLL
cmake -S $root -B "$root\build" -A x64 | Out-Null
cmake --build "$root\build" --config Release | Out-Null
$dll = "$root\build\Release\w11menu.dll"
if (-not (Test-Path $dll)) { throw "构建失败：找不到 $dll" }

# 2) 组装“外部位置”目录：DLL + 占位资源（1x1 png，context-menu 用不到，仅满足清单）
$ext = "$root\ext"; $extAssets = "$ext\Assets"
New-Item -ItemType Directory -Force -Path $extAssets | Out-Null
Copy-Item $dll "$ext\w11menu.dll" -Force
$png = [Convert]::FromBase64String("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNkYPhfDwAChwGA60e6kgAAAABJRU5ErkJggg==")
foreach ($a in @("StoreLogo.png","Square150x150Logo.png","Square44x44Logo.png")) {
    [IO.File]::WriteAllBytes((Join-Path $extAssets $a), $png)
}

# 3) 找 Windows SDK 工具
$sdkBin = Get-ChildItem "C:\Program Files (x86)\Windows Kits\10\bin" -Directory |
    Sort-Object Name -Descending | ForEach-Object { Join-Path $_.FullName "x64" } |
    Where-Object { Test-Path (Join-Path $_ "makeappx.exe") } | Select-Object -First 1
if (-not $sdkBin) { throw "找不到 Windows SDK 的 makeappx/signtool。" }
$makeappx = Join-Path $sdkBin "makeappx.exe"
$signtool = Join-Path $sdkBin "signtool.exe"

# 4) 打稀疏包（/nv 跳过对清单里文件路径的校验）
$msix = "$root\$pkgName.msix"
& $makeappx pack /o /d "$root\package" /nv /p $msix

# 5) 自签名证书 → 签名 → 导入到「受信任人」（否则报 CERT_E_UNTRUSTEDROOT / 0x800B0109）
$cert = New-SelfSignedCertificate -Type Custom -Subject $publisher `
    -KeyUsage DigitalSignature -FriendlyName "ShellExtDemos Dev Cert" `
    -CertStoreLocation "Cert:\CurrentUser\My" `
    -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3", "2.5.29.19={text}")
$pfx = "$root\devcert.pfx"; $cer = "$root\devcert.cer"
$pwd = ConvertTo-SecureString "devpwd" -Force -AsPlainText
Export-PfxCertificate -Cert $cert -FilePath $pfx -Password $pwd | Out-Null
& $signtool sign /fd SHA256 /a /f $pfx /p "devpwd" $msix
Export-Certificate -Cert $cert -FilePath $cer | Out-Null
Import-Certificate -FilePath $cer -CertStoreLocation "Cert:\CurrentUser\TrustedPeople" | Out-Null

# 6) 注册（外部位置 = 放了 DLL 的 ext 目录）
Add-AppxPackage -Path $msix -ExternalLocation $ext
Write-Host "已注册。右键任意文件，应能在 Win11【主】右键菜单直接看到「显示文件信息」（可能需重启资源管理器）。"
