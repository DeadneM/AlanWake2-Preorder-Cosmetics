# Changelog

## v1.0.0 / E01b

- Makes the two pre-order cosmetic menu entries use Deluxe access.
- Hides the game process debug console by default while preserving existing plugins.
- Includes the E01b ASI confirmed working by the project owner; no new gameplay changes in this publication.
- Bundles Ultimate ASI Loader 10.0.2 x64 and an INI.

## Development record

- E01: cosmetic access confirmed working by the user.
- E01b: console hiding added; user confirmed “nickel”.
- No automatic validation of additional game versions.

## v1.1.0-rc1 (experimental)

- Original d3d12 ASI loader replaces the third-party binary in this prerelease.
- Skin ASI remains byte-identical to validated E01b.
- Installation ZIP reduced to four necessary files.
- Isolated runtime forwarding and ASI initialization checks passed; game validation pending.

## v1.1.0-rc2 (experimental)
- RC1 REJECTED: startup crash with Epic overlay, stack overflow confirmed from Windows report/dump.
- Resolve System32 runtime exports directly to avoid intercepted lookup recursion.
- Regression test simulates intercepted GetProcAddress; isolated forwarding tests pass.
- Skin ASI unchanged. Game test pending; validated loader restored locally.


## Stable v1.1.0 - 2026-11-12
User explicitly confirmed RC2 tested and validated and authorized stable publication. Same loader and ASI bytes. One tested Epic installation; executable 0.564.904.9.


## Nexus publication - 2026-11-12
Published v1.1.0 on Nexus Mods as xDeYuSx: https://www.nexusmods.com/alanwake2/mods/96
Main file version 1.1.0, 166.1 KB. Cover, header banner, installation/settings/uninstall description, GitHub link and AI disclosures included. Same ZIP as stable GitHub release.
