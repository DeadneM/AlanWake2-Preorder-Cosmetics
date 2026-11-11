# v1.1.0-rc1 — original loader, minimal package

Base: validated E01b skin ASI, unchanged.
New component: original d3d12.dll proxy/ASI loader, isolated on experiment/custom-loader.

Objective: replace the bundled third-party loader while keeping the two menu cosmetics functional and avoiding a debug window.
Success criterion: Alan Wake 2 starts, existing loose-file plugin remains functional, both cosmetics stay selectable, no debug console.

CONSTRUIT: custom x64 loader, strict warning-free Zig 0.14.1 build.
CONTRÔLÉ outside the game: all 18 ordinal exports resolve; an isolated sentinel ASI receives InitializeASI; D3D12GetDebugInterface and D3D12GetInterface failure responses match System32; successful root-signature serialization produces the same 68 bytes as System32.
EXPÉRIMENTAL: game startup, existing-plugin timing, Agility SDK path and rendering compatibility.
VALIDÉ by user: E01b skin ASI only, not this new loader.

The proxy uses an absolute System32 path for the Windows runtime. Runtime loading occurs on API calls; ASI loading runs on a worker without waits or LoadLibrary in its DllMain. x64 assembly stubs preserve integer/floating argument registers and stack arguments before tail-forwarding. Missing system exports fail fast rather than inventing signatures.

Minimal ZIP: d3d12.dll, plugins/AW2PreorderMenu.asi, plugins/AW2PreorderMenu.ini, README.txt. No artwork, source, symbols, hashes, debug logs or loader INI in the installation archive. Hashes and source remain on GitHub.

Rollback locally: restore backups/before-custom-loader/d3d12.dll and its d3d12.ini. Keep all existing plugins. No change to game EXE, saves or the skin ASI.

References:
https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-best-practices
https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-d3d12enableexperimentalfeatures
