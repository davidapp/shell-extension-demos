# 06 · 属性处理器（IPropertyStore）

对应教程：**《Windows Shell 扩展编程完全指南》第 ⑥ 章**。

把文件元数据以“属性”形式暴露给属性系统。一个属性处理器就能同时供数据给：**详细信息视图的列、信息提示、分组、搜索、属性页**——这也是老“列处理器 `IColumnProvider`（Vista 已移除）”的现代替代。

本例给自定义扩展 `.shdemo` 暴露一个 `System.Comment` 属性，值由文件大小算出（只读）。

## 涉及接口

- **`IInitializeWithStream`** —— 以流拿到文件内容。
- **`IPropertyStore`** —— `GetCount` / `GetAt`（枚举属性键）、`GetValue`（取值）、`SetValue` / `Commit`（写；本例只读，返回 `STG_E_ACCESSDENIED`）。

## 构建 / 安装（管理员）

```bat
cmake -B build -A x64
cmake --build build --config Release
```
```powershell
.\install.ps1              # 注册（写 HKLM 属性系统）
.\install.ps1 -Uninstall
```

测试：新建一个 `test.shdemo` 文件，右键 → 属性，或在详细信息视图里看「备注」列（可能需把该列加入视图 / 重启资源管理器）。

## 要点

- 属性键是 **`PROPERTYKEY`**（`{FMTID}, PID`）。系统属性（如 `PKEY_Comment` = `System.Comment`）拿来即用，无需自定义 schema。
- 自定义属性需写一份 `.propdesc` schema 并用 `PSRegisterPropertySchema` 注册，才能有规范名、类型与本地化显示名。
- 注册在 `HKLM\...\PropertySystem\PropertyHandlers\.<ext>`，按扩展名绑定。

## 参考

- [IPropertyStore](https://learn.microsoft.com/zh-cn/windows/win32/api/propsys/nn-propsys-ipropertystore) · [属性处理器](https://learn.microsoft.com/zh-cn/windows/win32/properties/building-property-handlers)
- [PropertyHandler 官方示例](https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/winui/shell/appshellintegration/PropertyHandlers)
