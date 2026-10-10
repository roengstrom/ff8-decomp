#ifndef BC_OBJECT9_H
#define BC_OBJECT9_H


void* func_800B02AC(u8*);

u8* func_800B0328(u8*);

u8* func_800B0360(s32);

void* func_800B0398(u8*);

u8 func_800B0414(u32, u8*);

u8* func_800B04A0(u32, u8*);

static s32 func_800B054C(u32);

void func_800B0574(s32, u32);

void func_800B0600(s32, s32);

s32 func_800B0668(s32, s32);

void func_800B06DC(u16);

void func_800B0754(s32, s32, s32, u16);

s32 func_800B0794(s32, s32);

void func_800B0808(s32, u8*);

void func_800B08AC(s32, s32);

void func_800B095C(s32);

void func_800B0980(s32, s16, s32, s16);

void func_800B09F0(s32);

void func_800B0C08(void);

s8 func_800B0C68(s32, s32);

s32 func_800B0CC4(s32, s32);

s32 func_800B0D8C(s32, s32);

s32 func_800B0DDC(s32);

s32 func_800B0E30(s32);

s32 func_800B0F3C(s32);

s32 func_800B0F7C(s32);

u16 func_800B0F9C(s32);

u16 func_800B1050(s32);

u16 func_800B1104(s32);

s32 func_800B115C(s32, s32, s32*, u16*);

void func_800B13A0(s32, s32*, s32*, u16*);

s32 func_800B1438(s32);

void func_800B1564(s32, s32*, s32*, s16*);




/** @brief Build a task pool of @p count entries of @p stride bytes. */
void func_800B2A00(void *header, void *data, s32 stride, s32 count);

/**
 * @brief Take one task off @p pool and install @p task as its per-frame step.
 * @return The task's storage, or NULL if the pool is full.
 */
void *func_800B2A84(void *pool, void *task);

/**
 * @brief Run one frame of every task in @p pool.
 * @return The number of tasks still live afterwards.
 */
s32 func_800B2B68(void *pool);

/** @brief Take @p size bytes off the scratchpad stack. */
void *func_800B3698(s32 size);
/** @brief Give the last @p size bytes taken back to the scratchpad stack. */
void func_800B36B8(s32 size);

/**
 * @brief Take one task off the shared pool and install @p task on it.
 * @return The task's storage, or NULL if the pool is full.
 */
void *func_800B2C58(void *task);

/** @brief Submit @p m to the GTE as the pose for the next model. */
void func_800B3650(MATRIX *m);

/**
 * @brief Read the point @p id out of @p slot's model into @p out.
 *
 * @param slot One 0x9C-byte battle entity record; the effect overlays hold a
 *             struct view of it, battle indexes @c D_800EF2D0 by stride.
 */
void func_800B3960(void *slot, s32 id, s32 arg2, SVECTOR *out);

#endif /* BC_OBJECT9_H */
