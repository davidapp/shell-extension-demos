# 安装 / 卸载本示例。需【管理员】PowerShell。
#   .\install.ps1              # 注册
#   .\install.ps1 -Uninstall   # 注销
param([switch]$Uninstall)
$ErrorActionPreference = "Stop"

$dll = Get-ChildItem -Path $PSScriptRoot -Recurse -Filter ThumbnailExt.dll -ErrorAction SilentlyContinue |
    Sort-Object LastWriteTime -Descending | Select-Object -First 1
if (-not $dll) {
    Write-Error "找不到 ThumbnailExt.dll，请先构建：cmake -B build -A x64; cmake --build build --config Release"
    exit 1
}
Write-Host ("使用: {0}" -f $dll.FullName)
if ($Uninstall) { regsvr32 /u "$($dll.FullName)" } else { regsvr32 "$($dll.FullName)" }
