# 04 · 缩略图处理器（IThumbnailProvider）

对应教程：**《Windows Shell 扩展编程完全指南》第 ④ 章**。

给 `.txt` 文件生成自定义缩略图：根据文件大小推导底色、居中画出字节数。演示现代缩略图接口 **`IThumbnailProvider`**（取代已过时的 `IExtractImage`）。

## 涉及接口

- **`IInitializeWithStream`** —— Shell 以“流”把文件内容交给我们（在隔离宿主 `dllhost.exe` 里运行，比拿文件路径更安全）。
- **`IThumbnailProvider::GetThumbnail(cx, phbmp, pdwAlpha)`** —— 返回一张边长 `cx` 的 32bpp `HBITMAP`。

## 构建 / 安装（管理员）

```bat
cmake -B build -A x64
cmake --build build --config Release
```
```powershell
.\install.ps1              # 注册
.\install.ps1 -Uninstall   # 卸载
```

装好后，在资源管理器里把 `.txt` 文件夹切到「大图标 / 超大图标」视图即可看到自定义缩略图。

<!-- 提示：缩略图有缓存。看不到更新可用「磁盘清理」清缩略图缓存，或删掉 %LocalAppData%\Microsoft\Windows\Explorer\thumbcache_*.db 后重启资源管理器。 -->

## 要点

- 缩略图必须是 **32bpp** 位图（`CreateDIBSection`，`biBitCount=32`，高度取负得到自顶向下位图）。
- `pdwAlpha` 声明位图是否含 alpha：`WTSAT_RGB`（忽略 alpha）/ `WTSAT_ARGB`（预乘 alpha）。本例用 `WTSAT_RGB`。
- 注册在文件类型的 `shellex\{E357FCCD-A995-4576-B01F-234630154E96}`（系统定义的缩略图处理器类别）下。
- 缩略图宿主是独立进程，方法可能耗时——但仍应尽快返回，避免拖慢资源管理器。

## 参考

- [IThumbnailProvider](https://learn.microsoft.com/zh-cn/windows/win32/api/thumbcache/nn-thumbcache-ithumbnailprovider)
- [IInitializeWithStream](https://learn.microsoft.com/zh-cn/windows/win32/api/propsys/nn-propsys-iinitializewithstream)
- [Thumbnail Handlers 官方示例](https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/winui/shell/appshellintegration/RecipeThumbnailProvider)
