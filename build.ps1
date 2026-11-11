$ErrorActionPreference='Stop'
$zig=Get-Command zig -ErrorAction Stop
New-Item -ItemType Directory (Join-Path $PSScriptRoot 'build') -Force | Out-Null
& $zig.Source cc -target x86_64-windows-gnu -shared -O2 -Wall -Wextra -Werror (Join-Path $PSScriptRoot 'src\AW2PreorderMenu.c') -luser32 -o (Join-Path $PSScriptRoot 'build\AW2PreorderMenu.asi')
if($LASTEXITCODE -ne 0){throw 'Compilation failed'}
