# 07 · 信息提示处理器（IQueryInfo）

对应教程：**《Windows Shell 扩展编程完全指南》第 ⑦ 章**。

鼠标悬停在 `.txt` 文件上时，弹出自定义提示（路径 + 大小）。

## 涉及接口

- **`IPersistFile::Load`** —— 拿文件路径。
- **`IQueryInfo::GetInfoTip`** —— 返回提示文字（`CoTaskMemAlloc` 分配，`SHStrDupW` 最方便）。

## 构建 / 安装（管理员）

```bat
cmake -B build -A x64
cmake --build build --config Release
```
```powershell
.\install.ps1
.\install.ps1 -Uninstall
```

注册在 `txtfile\shellex\{00021500-0000-0000-C000-000000000046}`（信息提示处理器的固定类别）。

> 更省事的替代：不写代码，直接在 ProgID 下设 `InfoTip = prop:System.ItemNameDisplay;System.Size` 之类的属性列表字符串，让 Shell 用属性系统自动生成提示——见教程第 ⑥ 章的属性处理器。

## 参考

- [IQueryInfo](https://learn.microsoft.com/zh-cn/windows/win32/api/shlobj_core/nn-shlobj_core-iqueryinfo)
- [信息提示定制](https://learn.microsoft.com/zh-cn/windows/win32/shell/handlers#infotip-customization)
