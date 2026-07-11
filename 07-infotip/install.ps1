# 安装 / 卸载本示例。需【管理员】PowerShell。
param([switch]$Uninstall)
$ErrorActionPreference = "Stop"
$dll = Get-ChildItem -Path $PSScriptRoot -Recurse -Filter InfotipExt.dll -ErrorAction SilentlyContinue |
    Sort-Object LastWriteTime -Descending | Select-Object -First 1
if (-not $dll) { Write-Error "找不到 InfotipExt.dll，请先构建。"; exit 1 }
Write-Host ("使用: {0}" -f $dll.FullName)
if ($Uninstall) { regsvr32 /u "$($dll.FullName)" } else { regsvr32 "$($dll.FullName)" }
