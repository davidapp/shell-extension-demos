# 05 · 逐文件图标处理器（IExtractIcon）

对应教程：**《Windows Shell 扩展编程完全指南》第 ⑤ 章**。

让同类型文件按**内容**显示不同图标（注册表只能给整类文件配一个固定图标，做不到逐文件）。本例给 `.txt` 按文件大小分三档，从 `shell32.dll` 里挑不同图标。

## 涉及接口

- **`IPersistFile::Load`** —— 图标处理器的经典初始化，拿到文件路径。
- **`IExtractIconW::GetIconLocation`** —— 返回“图标在哪个文件的第几个”，并置 `GIL_PERINSTANCE` 表示逐文件。
- **`IExtractIconW::Extract`** —— 返回 `S_FALSE`，让 Shell 按位置自行加载（也可自己返回 `HICON`）。

## 构建 / 安装（管理员）

```bat
cmake -B build -A x64
cmake --build build --config Release
```
```powershell
.\install.ps1              # 注册（.txt 的 ProgID 默认 txtfile）
.\install.ps1 -Uninstall
```

装好后，不同大小的 `.txt` 会显示不同图标（图标有缓存，必要时重启资源管理器）。

<!-- 若看不到效果：确认 .txt 的 ProgID 确实是 txtfile（命令行 `assoc .txt` 与 `ftype txtfile`）。若不是，改 dllmain.cpp 里的 kIconKey。 -->

## 要点

- **`GIL_PERINSTANCE` 是关键**：不置它，Shell 会把图标按“类型”缓存成一个，逐文件就失效了。
- 现代替代初始化方式是 `IInitializeWithItem`（拿 `IShellItem`）；`IExtractIcon` 已不再强制 `IPersistFile`，但后者仍是最省事、最通用的。

## 参考

- [IExtractIcon](https://learn.microsoft.com/zh-cn/windows/win32/api/shobjidl_core/nn-shobjidl_core-iextracticonw)
- [创建图标处理器](https://learn.microsoft.com/zh-cn/windows/win32/shell/how-to-create-icon-handlers)
