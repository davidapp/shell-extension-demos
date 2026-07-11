# 注册 / 反注册 拖放处理器示例。需以管理员身份运行 PowerShell。
#   .\install.ps1            注册
#   .\install.ps1 -Uninstall 反注册
param([switch]$Uninstall)

$ErrorActionPreference = 'Stop'
$dll = Join-Path $PSScriptRoot 'build\Release\DropExt.dll'
if (-not (Test-Path $dll)) { throw "找不到 $dll，请先 cmake --build build --config Release" }

if ($Uninstall) {
    regsvr32 /u /s $dll
    Write-Host "已反注册：$dll"
} else {
    regsvr32 /s $dll
    Write-Host "已注册：$dll"
    Write-Host "把任意文件拖到某个 .txt 文件上松手，会弹出被拖入文件的清单。"
}
