实现`VkHooking`子项目：在 **VulkanTriangleHelloWorld** 运行时，加载项目内修改过的 `vulkan-1.dll`，实现一些调试功能。
实现一个`vulkan-1.dll`和`helper-vulkan.dll`，其中`vulkan-1.dll`转发请求到相应的系统vulkan.dll中，`helper-vulkan.dll`用于实现一些hook的功能。

功能要求如下：
1. 在vulkan启动时，在用户的文档目录的Izanagi_Log目录下新建`{date}-{time}-vkhook`目录，其下包含`log`。
   1. 创建设备和创建Swapchain等函数需要输出到log下
2. 在创建Shader时，保存相应的shader ir到`{date}-{time}-vkhook\{id}.ps/vs/cs`。
3. 注意转发所有GetProc/GetxxxAddr之类的函数，返回的函数指针应该是项目内的`vulkan-1`而不是系统的`vulkan-1`，但不要陷入死循环