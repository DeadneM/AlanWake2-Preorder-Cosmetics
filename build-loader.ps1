$ErrorActionPreference='Stop'
$zig=Get-Command zig -ErrorAction Stop
New-Item -ItemType Directory (Join-Path $PSScriptRoot 'build') -Force | Out-Null
& $zig.Source cc -target x86_64-windows-gnu -shared -O2 -Wall -Wextra -Werror (Join-Path $PSScriptRoot 'loader\loader.c') (Join-Path $PSScriptRoot 'loader\proxy.S') (Join-Path $PSScriptRoot 'loader\d3d12.def') -o (Join-Path $PSScriptRoot 'build\d3d12.dll')
if($LASTEXITCODE -ne 0){throw 'Loader compilation failed'}
