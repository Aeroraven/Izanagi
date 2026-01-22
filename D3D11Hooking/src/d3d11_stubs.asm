OPTION CASEMAP:NONE

EXTERN g_D3D11CreateDeviceForD3D12:QWORD
EXTERN g_D3DKMTCloseAdapter:QWORD
EXTERN g_D3DKMTDestroyAllocation:QWORD
EXTERN g_D3DKMTDestroyContext:QWORD
EXTERN g_D3DKMTDestroyDevice:QWORD
EXTERN g_D3DKMTDestroySynchronizationObject:QWORD
EXTERN g_D3DKMTPresent:QWORD
EXTERN g_D3DKMTQueryAdapterInfo:QWORD
EXTERN g_D3DKMTSetDisplayPrivateDriverFormat:QWORD
EXTERN g_D3DKMTSignalSynchronizationObject:QWORD
EXTERN g_D3DKMTUnlock:QWORD
EXTERN g_D3DKMTWaitForSynchronizationObject:QWORD
EXTERN g_EnableFeatureLevelUpgrade:QWORD
EXTERN g_OpenAdapter10:QWORD
EXTERN g_OpenAdapter10_2:QWORD
EXTERN g_CreateDirect3D11DeviceFromDXGIDevice:QWORD
EXTERN g_CreateDirect3D11SurfaceFromDXGISurface:QWORD
EXTERN g_D3D11CoreCreateDevice:QWORD
EXTERN g_D3D11CoreCreateLayeredDevice:QWORD
EXTERN g_D3D11CoreGetLayeredDeviceSize:QWORD
EXTERN g_D3D11CoreRegisterLayers:QWORD
EXTERN g_D3D11On12CreateDevice:QWORD
EXTERN g_D3DKMTCreateAllocation:QWORD
EXTERN g_D3DKMTCreateContext:QWORD
EXTERN g_D3DKMTCreateDevice:QWORD
EXTERN g_D3DKMTCreateSynchronizationObject:QWORD
EXTERN g_D3DKMTEscape:QWORD
EXTERN g_D3DKMTGetContextSchedulingPriority:QWORD
EXTERN g_D3DKMTGetDeviceState:QWORD
EXTERN g_D3DKMTGetDisplayModeList:QWORD
EXTERN g_D3DKMTGetMultisampleMethodList:QWORD
EXTERN g_D3DKMTGetRuntimeData:QWORD
EXTERN g_D3DKMTGetSharedPrimaryHandle:QWORD
EXTERN g_D3DKMTLock:QWORD
EXTERN g_D3DKMTOpenAdapterFromHdc:QWORD
EXTERN g_D3DKMTOpenResource:QWORD
EXTERN g_D3DKMTQueryAllocationResidency:QWORD
EXTERN g_D3DKMTQueryResourceInfo:QWORD
EXTERN g_D3DKMTRender:QWORD
EXTERN g_D3DKMTSetAllocationPriority:QWORD
EXTERN g_D3DKMTSetContextSchedulingPriority:QWORD
EXTERN g_D3DKMTSetDisplayMode:QWORD
EXTERN g_D3DKMTSetGammaRamp:QWORD
EXTERN g_D3DKMTSetVidPnSourceOwner:QWORD
EXTERN g_D3DKMTWaitForVerticalBlankEvent:QWORD
EXTERN g_D3DPerformance_BeginEvent:QWORD
EXTERN g_D3DPerformance_EndEvent:QWORD
EXTERN g_D3DPerformance_GetStatus:QWORD
EXTERN g_D3DPerformance_SetMarker:QWORD
EXTERN EnsureExportsLoadedForStubs:PROC

.code

D3D11CreateDeviceForD3D12 PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3D11CreateDeviceForD3D12]
    jmp rax
D3D11CreateDeviceForD3D12 ENDP

D3DKMTCloseAdapter PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTCloseAdapter]
    jmp rax
D3DKMTCloseAdapter ENDP

D3DKMTDestroyAllocation PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTDestroyAllocation]
    jmp rax
D3DKMTDestroyAllocation ENDP

D3DKMTDestroyContext PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTDestroyContext]
    jmp rax
D3DKMTDestroyContext ENDP

D3DKMTDestroyDevice PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTDestroyDevice]
    jmp rax
D3DKMTDestroyDevice ENDP

D3DKMTDestroySynchronizationObject PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTDestroySynchronizationObject]
    jmp rax
D3DKMTDestroySynchronizationObject ENDP

D3DKMTPresent PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTPresent]
    jmp rax
D3DKMTPresent ENDP

D3DKMTQueryAdapterInfo PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTQueryAdapterInfo]
    jmp rax
D3DKMTQueryAdapterInfo ENDP

D3DKMTSetDisplayPrivateDriverFormat PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTSetDisplayPrivateDriverFormat]
    jmp rax
D3DKMTSetDisplayPrivateDriverFormat ENDP

D3DKMTSignalSynchronizationObject PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTSignalSynchronizationObject]
    jmp rax
D3DKMTSignalSynchronizationObject ENDP

D3DKMTUnlock PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTUnlock]
    jmp rax
D3DKMTUnlock ENDP

D3DKMTWaitForSynchronizationObject PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTWaitForSynchronizationObject]
    jmp rax
D3DKMTWaitForSynchronizationObject ENDP

EnableFeatureLevelUpgrade PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_EnableFeatureLevelUpgrade]
    jmp rax
EnableFeatureLevelUpgrade ENDP

OpenAdapter10 PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_OpenAdapter10]
    jmp rax
OpenAdapter10 ENDP

OpenAdapter10_2 PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_OpenAdapter10_2]
    jmp rax
OpenAdapter10_2 ENDP

CreateDirect3D11DeviceFromDXGIDevice PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_CreateDirect3D11DeviceFromDXGIDevice]
    jmp rax
CreateDirect3D11DeviceFromDXGIDevice ENDP

CreateDirect3D11SurfaceFromDXGISurface PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_CreateDirect3D11SurfaceFromDXGISurface]
    jmp rax
CreateDirect3D11SurfaceFromDXGISurface ENDP

D3D11CoreCreateDevice PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3D11CoreCreateDevice]
    jmp rax
D3D11CoreCreateDevice ENDP

D3D11CoreCreateLayeredDevice PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3D11CoreCreateLayeredDevice]
    jmp rax
D3D11CoreCreateLayeredDevice ENDP

D3D11CoreGetLayeredDeviceSize PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3D11CoreGetLayeredDeviceSize]
    jmp rax
D3D11CoreGetLayeredDeviceSize ENDP

D3D11CoreRegisterLayers PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3D11CoreRegisterLayers]
    jmp rax
D3D11CoreRegisterLayers ENDP

D3D11On12CreateDevice PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3D11On12CreateDevice]
    jmp rax
D3D11On12CreateDevice ENDP

D3DKMTCreateAllocation PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTCreateAllocation]
    jmp rax
D3DKMTCreateAllocation ENDP

D3DKMTCreateContext PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTCreateContext]
    jmp rax
D3DKMTCreateContext ENDP

D3DKMTCreateDevice PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTCreateDevice]
    jmp rax
D3DKMTCreateDevice ENDP

D3DKMTCreateSynchronizationObject PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTCreateSynchronizationObject]
    jmp rax
D3DKMTCreateSynchronizationObject ENDP

D3DKMTEscape PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTEscape]
    jmp rax
D3DKMTEscape ENDP

D3DKMTGetContextSchedulingPriority PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTGetContextSchedulingPriority]
    jmp rax
D3DKMTGetContextSchedulingPriority ENDP

D3DKMTGetDeviceState PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTGetDeviceState]
    jmp rax
D3DKMTGetDeviceState ENDP

D3DKMTGetDisplayModeList PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTGetDisplayModeList]
    jmp rax
D3DKMTGetDisplayModeList ENDP

D3DKMTGetMultisampleMethodList PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTGetMultisampleMethodList]
    jmp rax
D3DKMTGetMultisampleMethodList ENDP

D3DKMTGetRuntimeData PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTGetRuntimeData]
    jmp rax
D3DKMTGetRuntimeData ENDP

D3DKMTGetSharedPrimaryHandle PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTGetSharedPrimaryHandle]
    jmp rax
D3DKMTGetSharedPrimaryHandle ENDP

D3DKMTLock PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTLock]
    jmp rax
D3DKMTLock ENDP

D3DKMTOpenAdapterFromHdc PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTOpenAdapterFromHdc]
    jmp rax
D3DKMTOpenAdapterFromHdc ENDP

D3DKMTOpenResource PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTOpenResource]
    jmp rax
D3DKMTOpenResource ENDP

D3DKMTQueryAllocationResidency PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTQueryAllocationResidency]
    jmp rax
D3DKMTQueryAllocationResidency ENDP

D3DKMTQueryResourceInfo PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTQueryResourceInfo]
    jmp rax
D3DKMTQueryResourceInfo ENDP

D3DKMTRender PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTRender]
    jmp rax
D3DKMTRender ENDP

D3DKMTSetAllocationPriority PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTSetAllocationPriority]
    jmp rax
D3DKMTSetAllocationPriority ENDP

D3DKMTSetContextSchedulingPriority PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTSetContextSchedulingPriority]
    jmp rax
D3DKMTSetContextSchedulingPriority ENDP

D3DKMTSetDisplayMode PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTSetDisplayMode]
    jmp rax
D3DKMTSetDisplayMode ENDP

D3DKMTSetGammaRamp PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTSetGammaRamp]
    jmp rax
D3DKMTSetGammaRamp ENDP

D3DKMTSetVidPnSourceOwner PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTSetVidPnSourceOwner]
    jmp rax
D3DKMTSetVidPnSourceOwner ENDP

D3DKMTWaitForVerticalBlankEvent PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DKMTWaitForVerticalBlankEvent]
    jmp rax
D3DKMTWaitForVerticalBlankEvent ENDP

D3DPerformance_BeginEvent PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DPerformance_BeginEvent]
    jmp rax
D3DPerformance_BeginEvent ENDP

D3DPerformance_EndEvent PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DPerformance_EndEvent]
    jmp rax
D3DPerformance_EndEvent ENDP

D3DPerformance_GetStatus PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DPerformance_GetStatus]
    jmp rax
D3DPerformance_GetStatus ENDP

D3DPerformance_SetMarker PROC
    push rcx
    push rdx
    push r8
    push r9
    sub rsp, 28h
    call EnsureExportsLoadedForStubs
    add rsp, 28h
    pop r9
    pop r8
    pop rdx
    pop rcx
    mov rax, qword ptr [g_D3DPerformance_SetMarker]
    jmp rax
D3DPerformance_SetMarker ENDP

END
