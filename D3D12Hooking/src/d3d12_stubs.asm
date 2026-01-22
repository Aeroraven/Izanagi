OPTION CASEMAP:NONE

EXTERN g_ord99:QWORD
EXTERN g_SetAppCompatStringPointer:QWORD
EXTERN g_D3D12CoreCreateLayeredDevice:QWORD
EXTERN g_D3D12CoreGetLayeredDeviceSize:QWORD
EXTERN g_D3D12CoreRegisterLayers:QWORD
EXTERN g_D3D12DeviceRemovedExtendedData:QWORD
EXTERN g_D3D12GetInterface:QWORD
EXTERN g_D3D12PIXEventsReplaceBlock:QWORD
EXTERN g_D3D12PIXGetThreadInfo:QWORD
EXTERN g_D3D12PIXNotifyWakeFromFenceSignal:QWORD
EXTERN g_D3D12PIXReportCounter:QWORD
EXTERN g_GetBehaviorValue:QWORD
EXTERN EnsureExportsLoadedForStubs:PROC

.code

ord99 PROC
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
    mov rax, qword ptr [g_ord99]
    jmp rax
ord99 ENDP

SetAppCompatStringPointer PROC
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
    mov rax, qword ptr [g_SetAppCompatStringPointer]
    jmp rax
SetAppCompatStringPointer ENDP

D3D12CoreCreateLayeredDevice PROC
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
    mov rax, qword ptr [g_D3D12CoreCreateLayeredDevice]
    jmp rax
D3D12CoreCreateLayeredDevice ENDP

D3D12CoreGetLayeredDeviceSize PROC
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
    mov rax, qword ptr [g_D3D12CoreGetLayeredDeviceSize]
    jmp rax
D3D12CoreGetLayeredDeviceSize ENDP

D3D12CoreRegisterLayers PROC
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
    mov rax, qword ptr [g_D3D12CoreRegisterLayers]
    jmp rax
D3D12CoreRegisterLayers ENDP

D3D12DeviceRemovedExtendedData PROC
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
    mov rax, qword ptr [g_D3D12DeviceRemovedExtendedData]
    jmp rax
D3D12DeviceRemovedExtendedData ENDP

D3D12GetInterface PROC
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
    mov rax, qword ptr [g_D3D12GetInterface]
    jmp rax
D3D12GetInterface ENDP

D3D12PIXEventsReplaceBlock PROC
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
    mov rax, qword ptr [g_D3D12PIXEventsReplaceBlock]
    jmp rax
D3D12PIXEventsReplaceBlock ENDP

D3D12PIXGetThreadInfo PROC
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
    mov rax, qword ptr [g_D3D12PIXGetThreadInfo]
    jmp rax
D3D12PIXGetThreadInfo ENDP

D3D12PIXNotifyWakeFromFenceSignal PROC
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
    mov rax, qword ptr [g_D3D12PIXNotifyWakeFromFenceSignal]
    jmp rax
D3D12PIXNotifyWakeFromFenceSignal ENDP

D3D12PIXReportCounter PROC
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
    mov rax, qword ptr [g_D3D12PIXReportCounter]
    jmp rax
D3D12PIXReportCounter ENDP

GetBehaviorValue PROC
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
    mov rax, qword ptr [g_GetBehaviorValue]
    jmp rax
GetBehaviorValue ENDP

END
