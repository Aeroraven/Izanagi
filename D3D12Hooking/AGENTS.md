# D3D12Hooking AGENTS

目的：在 **D3D12TriangleHelloWorld** 运行时，加载项目内修改过的 `d3d12.dll`，触发 RenderDoc 初始化（可在 UI 内截帧，但不主动启动 RenderDoc），且 **仅对本项目目录生效**。

## 实现原则（基于 D3D11 方案推断）
- **最小修改系统 DLL 的思路**：不做系统级替换，只在应用目录放置改过的 `d3d12.dll`。
- **强制加载 helper.dll**：在改过的 `d3d12.dll` 的 Import Directory 中加入 `helper.dll!DllCanUnloadNow`（或其他稳定导入），让 Loader 自动加载 helper。
- **Hook/初始化由 helper.dll 完成**：`helper.dll` 的 `DllMain` 中加载 `renderdoc.dll`（在 PATH），调用 RenderDoc API 初始化，并安装需要的 Hook。
- **保持原始导出**：`d3d12.dll` 仍保留原始导出并转发到系统真实 `d3d12.dll`，避免破坏 API 行为。

## 约束
- 仅在 `D3D12Hooking` 子项目内实现，不做任何系统级别修改。
- 不修改系统目录下的 DLL，不改注册表，不做全局注入。
- 产物（改过的 `d3d12.dll`、`helper.dll`）仅放在可执行文件同目录。

## 交付物要求
- `helper.dll`：最小实现，负责 RenderDoc 初始化与 Hook 安装。
- 代理/改造的 `d3d12.dll`：保持导出完整性，新增 import 触发 `helper.dll` 加载。
- README（若新增文件）：说明使用步骤、依赖与验证方式（如何确认可截帧）。
