## VkHooking 当前功能/状态

### 目标
在 **VkTriangleHelloWorld** 运行时加载项目内 `vulkan-1.dll`，通过 `helper-vulkan.dll`
实现调试日志、shader IR dump、调用栈等功能，并将所有 Vulkan 导出转发到系统
`vulkan-1.dll`。

### 已实现功能
1. **完整导出转发**
   - 从系统 `vulkan-1.dll` 自动生成导出列表（.def）和 stub（MASM），所有导出都会转发到系统 DLL。
   - `vkGetInstanceProcAddr/vkGetDeviceProcAddr` 优先返回本地导出地址，避免回到系统 DLL 导致 hook 失效。
2. **日志与目录**
   - 在 `%USERPROFILE%\Documents\Izanagi_Logs\{YYYYMMDD-HHMMSS}-vkhook\log` 创建日志目录。
   - 日志格式：`[HH:MM:SS.mmm][frameId][API]: message`。
   - 输出：`vkCreateInstance/vkCreateDevice/vkCreateSwapchainKHR/vkCreateGraphicsPipelines/vkCreateComputePipelines` 等。
3. **Shader IR 保存**
   - 在 `vkCreateShaderModule` 保存 SPIR-V 到 `{id}.vs/.ps/.cs`（自动解析 stage）。
4. **调用栈**
   - 在 `vkEnumeratePhysicalDevices` 与 `vkCreateDevice` 时记录调用栈。
5. **控制台输入**
   - `vkCreateDevice` 成功后启动非阻塞控制台线程。
   - 控制台输入 `c` 回车后，**下一帧开始**会写入一条日志（在 `vkAcquireNextImageKHR/2KHR` 触发帧开始）。

### 当前构建说明
- CMake 目标：`helper-vulkan.dll` + `vulkan-1.dll`（MASM stubs + /DEF 导出）。
- 需要 `dbghelp.lib` 用于调用栈。

### 运行/使用
将 `vulkan-1.dll` 与 `helper-vulkan.dll` 放在 `VkTriangleHelloWorld.exe` 旁边运行。
