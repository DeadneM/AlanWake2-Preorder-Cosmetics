# Technical record

Baseline: E01b, confirmed working by the user; public release v1.0.0 packages the same ASI bytes.

Static analysis of executable 0.564.904.9:
- is_deluxe RVA 0x1959ae0 reads +0x123 through global pointer RVA 0x38a48c8.
- is_preorder RVA 0x1959b20 reads +0x122. Neither global getter is patched.
- Sweater vector RVA 0x376ea48; revolver vector RVA 0x376e8e8.
- Two descriptors per vector, stride 0x38, key pointer +8, access category +0x20.
- Pre-order category 1, digital Deluxe category 2, inferred from neighboring constructors and native category checks.

The plugin checks PE timestamp/size, constructor anchors and strings, validates both live descriptors, then atomically updates their categories. If the second changes unexpectedly it attempts to restore the first. No executable code patches are used.

Construction checked with Zig 0.14.1, Windows x64, warnings treated as errors. DLL loading/export and unsupported-host refusal were checked outside the game. User then confirmed E01 cosmetic functionality and E01b console hiding. No claim of wider compatibility.

Reverting the ASI to E01 restores the pre-console-change build; removing the ASI and restarting reverts the descriptor changes.
