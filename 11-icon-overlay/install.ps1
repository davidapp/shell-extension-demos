# 注册 / 反注册 图标叠加处理器示例。需以管理员身份运行 PowerShell。
#   .\install.ps1            注册
#   .\install.ps1 -Uninstall 反注册
# 注册后需重启资源管理器（或注销）才会生效；把某个文件改名带上 “demo” 即可看到左下角角标。
param([switch]$Uninstall)

$ErrorActionPreference = 'Stop'
$dll = Join-Path $PSScriptRoot 'build\Release\OverlayExt.dll'
if (-not (Test-Path $dll)) { throw "找不到 $dll，请先 cmake --build build --config Release" }

if ($Uninstall) {
    regsvr32 /u /s $dll
    Write-Host "已反注册。重启资源管理器后角标消失。"
} else {
    regsvr32 /s $dll
    Write-Host "已注册。请重启资源管理器：Stop-Process -Name explorer -Force"
    Write-Host "然后把某个文件改名为含 demo 的名字，图标左下角会出现叠加标记。"
}
