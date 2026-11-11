.intel_syntax noprefix
.text
.global proxy_SetAppCompatStringPointer
proxy_SetAppCompatStringPointer:
 mov r11d, 0
 jmp proxy_dispatch
.global proxy_D3D12CreateDevice
proxy_D3D12CreateDevice:
 mov r11d, 1
 jmp proxy_dispatch
.global proxy_D3D12GetDebugInterface
proxy_D3D12GetDebugInterface:
 mov r11d, 2
 jmp proxy_dispatch
.global proxy_D3D12CoreCreateLayeredDevice
proxy_D3D12CoreCreateLayeredDevice:
 mov r11d, 3
 jmp proxy_dispatch
.global proxy_D3D12CoreGetLayeredDeviceSize
proxy_D3D12CoreGetLayeredDeviceSize:
 mov r11d, 4
 jmp proxy_dispatch
.global proxy_D3D12CoreRegisterLayers
proxy_D3D12CoreRegisterLayers:
 mov r11d, 5
 jmp proxy_dispatch
.global proxy_D3D12CreateRootSignatureDeserializer
proxy_D3D12CreateRootSignatureDeserializer:
 mov r11d, 6
 jmp proxy_dispatch
.global proxy_D3D12CreateVersionedRootSignatureDeserializer
proxy_D3D12CreateVersionedRootSignatureDeserializer:
 mov r11d, 7
 jmp proxy_dispatch
.global proxy_D3D12DeviceRemovedExtendedData
proxy_D3D12DeviceRemovedExtendedData:
 mov r11d, 8
 jmp proxy_dispatch
.global proxy_D3D12EnableExperimentalFeatures
proxy_D3D12EnableExperimentalFeatures:
 mov r11d, 9
 jmp proxy_dispatch
.global proxy_D3D12GetInterface
proxy_D3D12GetInterface:
 mov r11d, 10
 jmp proxy_dispatch
.global proxy_D3D12PIXEventsReplaceBlock
proxy_D3D12PIXEventsReplaceBlock:
 mov r11d, 11
 jmp proxy_dispatch
.global proxy_D3D12PIXGetThreadInfo
proxy_D3D12PIXGetThreadInfo:
 mov r11d, 12
 jmp proxy_dispatch
.global proxy_D3D12PIXNotifyWakeFromFenceSignal
proxy_D3D12PIXNotifyWakeFromFenceSignal:
 mov r11d, 13
 jmp proxy_dispatch
.global proxy_D3D12PIXReportCounter
proxy_D3D12PIXReportCounter:
 mov r11d, 14
 jmp proxy_dispatch
.global proxy_D3D12SerializeRootSignature
proxy_D3D12SerializeRootSignature:
 mov r11d, 15
 jmp proxy_dispatch
.global proxy_D3D12SerializeVersionedRootSignature
proxy_D3D12SerializeVersionedRootSignature:
 mov r11d, 16
 jmp proxy_dispatch
.global proxy_GetBehaviorValue
proxy_GetBehaviorValue:
 mov r11d, 17
 jmp proxy_dispatch
proxy_dispatch:
 sub rsp, 168
 mov [rsp+32], rcx
 mov [rsp+40], rdx
 mov [rsp+48], r8
 mov [rsp+56], r9
 movdqu [rsp+64], xmm0
 movdqu [rsp+80], xmm1
 movdqu [rsp+96], xmm2
 movdqu [rsp+112], xmm3
 mov ecx, r11d
 call resolve_export
 mov rcx, [rsp+32]
 mov rdx, [rsp+40]
 mov r8, [rsp+48]
 mov r9, [rsp+56]
 movdqu xmm0, [rsp+64]
 movdqu xmm1, [rsp+80]
 movdqu xmm2, [rsp+96]
 movdqu xmm3, [rsp+112]
 add rsp, 168
 jmp rax
