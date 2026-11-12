# Alan Wake 2 — Pre-order Cosmetics

![Mod cover](assets/nexus-cover.png)

Makes Saga’s pre-order sweater and Alan’s pre-order revolver skin selectable from the existing **Cosmetics** menu for Deluxe owners.

## Stable v1.1.0

RC2 explicitly tested and validated by the project owner: successful startup, working cosmetics and hidden console. The release retains identical RC2 loader and E01b ASI bytes. RC1 remains rejected.

## Download and install

Download the installable ZIP from [Releases](https://github.com/DeadneM/AlanWake2-Preorder-Cosmetics/releases/latest). Close the game, then extract it directly beside `AlanWake2.exe`.

The package includes this project’s own x64 ASI loader as `d3d12.dll`, plus `plugins/AW2PreorderMenu.asi` and its INI. If you already use an ASI loader, keep one active loader and install the plugin/INI into its `plugins` folder. Back up conflicting files first. Do not overwrite another graphics proxy without checking compatibility. Preserve your other plugins.

Launch normally, then select the sweater and revolver skin in Cosmetics. A debug console created by an existing plugin is hidden by default; its plugin continues to run.

## Requirements and tested status

- Windows x64, Alan Wake 2 PC, Deluxe ownership.
- Target executable version **0.564.904.9**.
- Executable SHA256: `09135F128C07CFA4598B22DD4754A98B2C1F4FF816BFCB1E336ECFB4462B4163`.
- Cosmetics access, console hiding and RC2 custom loader were explicitly tested and confirmed working by the project owner. This is one installation’s validation, not a compatibility guarantee for other game builds.

## Settings

`plugins/AW2PreorderMenu.ini`:

```ini
[Mod]
Enabled=1
HideDebugConsole=1
```

Set `Enabled=0` to disable cosmetic changes on the next launch. Set `HideDebugConsole=0` to leave the attached debug console visible. Restart after changing either setting.

## How it works

The plugin checks the executable layout, waits for two known cosmetic menu descriptors, validates their identifiers, and changes their access category in memory from pre-order to Deluxe. It does not modify the game executable on disk, grant account ownership, call EOS entitlement APIs, or target story expansions or charms. The game can save cosmetic selections through its normal behavior.

The console option hides the console attached to the game process. It does not disable the plugin that created it. Diagnostic text is written to `plugins/AW2PreorderMenu.log`. `APPLIED` only reports that the descriptor changes were made.

## Uninstall

Close the game and remove `plugins/AW2PreorderMenu.asi`. Keep or remove its INI/log as desired. Remove the bundled loader only if no other plugin needs it; restore your previous loader from your backup if you replaced one.

## Build

Use Zig 0.14.1 on Windows, then run `build.ps1` for the ASI and `build-loader.ps1` for the original loader.

The loader forwards the 18 named/ordinal exports of the inspected Windows d3d12.dll to the real System32 DLL. It loads ASIs in the sibling `plugins` and `scripts` folders and calls exported `InitializeASI`. It does not implement all Ultimate ASI Loader options, root-folder ASIs, recursive discovery, DLL chaining, or file overrides.

## Credits

v1.1.0 uses an original loader written for this project; no Ultimate ASI Loader binary is included in that package. Earlier v1.0.0 used [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) by ThirteenAG under its MIT license.

Alan Wake 2 and its game assets belong to their respective owners. This is an unofficial mod. Promotional artwork is AI-generated illustration, not an in-game screenshot.
