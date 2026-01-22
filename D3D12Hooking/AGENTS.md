# D3D12Hooking AGENTS

目的：在 **D3D12TriangleHelloWorld** 运行时，加载项目内修改过的 `d3d12.dll`，触发 RenderDoc 初始化（可在 UI 内截帧，但不主动启动 RenderDoc），且 **仅对本项目目录生效**。

## 实现原则（优化版）
- **最小修改系统 DLL 的思路**：不做系统级替换，只在应用目录放置改过的 `d3d12.dll`。
- **强制加载 helper.dll**：`d3d12.dll` 通过 Import 依赖 `helper.dll!DllCanUnloadNow`，让 Loader 自动加载 helper。
- **避免 DllMain 重活**：`helper.dll` 不在 `DllMain` 做初始化，仅通过 `HelperInitialize` 启动后台线程。
- **遵循 RenderDoc 官方流程**：不手动 `LoadLibrary`，只在检测到 `renderdoc.dll` 已被注入后调用 `RENDERDOC_GetAPI`。
- **保持原始导出**：`d3d12.dll` 仍保留原始导出并转发到系统真实 `d3d12.dll`，避免破坏 API 行为。
## RenderDoc 相关约定
- `D3D12CreateDevice` 前调用 `HelperInitialize`，确保 RenderDoc 有机会在设备创建前注入。
- `HelperSetDevice` 保存设备指针；当设备和窗口可用时，才调用 `SetActiveWindow`/Overlay。
- Overlay、热键、自动截帧均由环境变量控制，默认尽量少做动作以降低崩溃风险。
- 如使用 “Inject into Process”，可通过 `D3D12_HOOK_WAIT_RENDERDOC` 在创建设备前短暂等待。

## 约束
- 仅在 `D3D12Hooking` 子项目内实现，不做任何系统级别修改。
- 不修改系统目录下的 DLL，不改注册表，不做全局注入。
- 产物（改过的 `d3d12.dll`、`helper.dll`）仅放在可执行文件同目录。

## 交付物要求
- `helper.dll`：最小实现，负责 RenderDoc 初始化与 Hook 安装。
- 代理/改造的 `d3d12.dll`：保持导出完整性，新增 import 触发 `helper.dll` 加载。
- README（若新增文件）：说明使用步骤、依赖与验证方式（如何确认可截帧）。
