# 示例 10 —— 拖放处理器（IDropTarget）

第 ⑩ 章配套代码。让 `.txt` 文件成为**拖放目标**：把任意文件拖到某个 `.txt` 上松手，弹框列出被拖入的文件。

## 接口

- `IPersistFile` —— `Load` 拿到“被拖放到的目标文件”路径。
- `IDropTarget` —— `DragEnter` / `DragOver` 汇报拖放效果（这里统一 `DROPEFFECT_COPY`），`Drop` 里从 `IDataObject` 取 `CF_HDROP` 文件列表。

## 构建

```powershell
cmake -B build -A x64
cmake --build build --config Release
```

## 注册 / 反注册

```powershell
# 管理员 PowerShell
.\install.ps1
.\install.ps1 -Uninstall
```

注册位置：`HKCR\txtfile\shellex\DropHandler`（默认值 = CLSID）。

## 试一试

把任意文件拖到某个 `.txt` 文件图标上松手（不是拖进去打开，而是拖到图标上），会弹出被拖入文件的完整清单。

> 与右键“发送到”不同，拖放处理器只对“拖动”这一交互生效。
