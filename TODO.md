# TODO

## Properly type g_gfData in gf.h

Currently `extern GfData g_gfData` cannot be placed in gf.h because four source
files (`game.c`, `gf_curve.c`, `gf_anim.c`, `bc_object1.c`) declare it locally
as `extern u8 g_gfData[]` and use raw pointer arithmetic. The fix requires:

1. **Add missing entry struct types to gf.h**
   - `levelCurve12` entries (stride 12) — no struct defined
   - `statTable24` entries (stride 24, ability id < 0x21) — no struct defined
   - `statTable4` entries (stride 4, ability id ≥ 0x21) — no struct defined
   - `elementData24` entries (stride 24) — no struct defined
   - Change `statTable8` and all `abilityRangeI`–`abilityRangeO` fields in
     `GfData` from raw `u8[]` to properly typed `AbilityEntry[]`

2. **Fix `GfJunctionEntry` — expand `pad12[0x2A]`**
   - gf_curve.c accesses offsets +0x1D, +0x1E, +0x20, +0x21, +0x24 within the
     entry; these are currently unnamed inside `pad12`

3. **Fix `GfAbilityTableEntry` offset in GfData**
   - `ptrAbilityTable132` (+0x88) points to 0xF8C, but the `abilityTable132`
     field in `GfData` starts at 0xF7A (18-byte discrepancy)
   - Increase `junctionDataTrailing` by 18 bytes and change `abilityTable132`
     to `GfAbilityTableEntry abilityTable132[16]` starting at 0xF8C

4. **Add `extern GfData g_gfData;` to gf.h**

5. **Refactor all raw pointer accesses** in `game.c`, `gf_curve.c`, `gf_anim.c`,
   and `bc_object1.c` to use struct field access; verify matches after each file
