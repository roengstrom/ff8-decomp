#include "common.h"
#include "battle_anim.h"
#include "btl_anim.h"
#include "gamepad.h"
#include "input/vibration.h"

/* --- Private types --- */

/** @brief The (value, duration) keyframes that drive one of the pad's two motor values. */
typedef struct {
    /* 0x00 */ u8 *start; /**< Start of the stream's data. */
    /* 0x04 */ u8 *end; /**< End of the stream's data. */
    /* 0x08 */ s16 cursor; /**< Current read position (-1 = not started). */
    /* 0x0A */ u16 length; /**< Stream length in bytes. */
    /* 0x0C */ u8 enabled; /**< 1 if the stream has data, 0 if empty. */
    /* 0x0D */ u8 pad0D[3];
} MotorStream;

/** @brief One of the four slots a vibration plays in. */
typedef struct {
    /* 0x00 */ MotorStream streams[2]; /**< One per motor value. */
    /* 0x20 */ u16 serial; /**< Bumped by every start (wraps at 0x400 to 1), so old ids stop matching. */
    /* 0x22 */ s8 priority; /**< Priority of the playing vibration; 0 = the slot is free. */
    /* 0x23 */ u8 index; /**< Slot index (0-3). */
} VibrationSlot;

/** @brief A vibration pattern in a packed data block: two stream lengths, then the streams. */
typedef struct {
    /* 0x00 */ u16 len1; /**< Length of stream 0. */
    /* 0x02 */ u16 len2; /**< Length of stream 1. */
    /* 0x04 */ u8 data[1]; /**< Stream 0's data[len1], then stream 1's data[len2]. */
} VibrationPattern;

/* --- Externs (sorted by address) --- */

extern s32 g_motorPeak0; /* 0x80083870 — largest stream 0 value of the playing vibrations */
extern s32 g_motorPeak1; /* 0x80083874 — largest stream 1 value of the playing vibrations */
extern VibrationSlot g_vibrationTable[]; /* 0x80083878 — the four vibration slots */

/* --- Private functions --- */

static VibrationSlot *getVibrationTable(void);
static VibrationSlot *findVibrationSlot(s32 threshold);
static s32 stepMotorStream(MotorStream *stream);

/**
 * @brief Get the vibration table.
 * @return Pointer to the four vibration entries.
 */
static VibrationSlot *getVibrationTable(void) {
    return g_vibrationTable;
}

/**
 * @brief Find the slot a new vibration should take.
 *
 * Scans the 4 slots of the vibration table for a free or low-priority one.
 * If any slot has priority 0 (free), returns it immediately (first-fit).
 * Otherwise, returns the slot with the lowest priority that is
 * still <= threshold. Returns NULL if no suitable slot is found.
 *
 * @param threshold Maximum priority to consider as a candidate.
 * @return Pointer to the slot to take, or NULL if none found.
 */
static VibrationSlot *findVibrationSlot(s32 threshold) {
    VibrationSlot* ptr;
    s32 best;
    s32 i;

    ptr = getVibrationTable();
    ptr++; ptr--; /* Regalloc: boost ptr priority */
    best = 0xFF;

    for (i = 0; i < 4; i++, ptr++) {
        if (ptr->priority == 0) {
            return ptr;
        }
        if (threshold >= ptr->priority && ptr->priority < best) {
            best = i;
        }
    }

    if (best != 0xFF) {
        return &getVibrationTable()[best];
    }
    return NULL;
}


/**
 * @brief Check if any vibration is playing.
 *
 * Scans the 4 slots of the vibration table. Returns 1 immediately if any
 * slot has a non-zero priority, or 0 if all are free.
 *
 * @return 1 if any vibration is playing, 0 otherwise.
 */
s32 isVibrating(void) {
    VibrationSlot* ptr = getVibrationTable();
    s32 i;

    for (i = 0; i < 4; i++, ptr++) {
        if (ptr->priority != 0) {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief Check if the vibration @p cmd, an id from startVibration, is still playing.
 *
 * Returns 0 if @p cmd is zero. Otherwise, looks up the slot at index
 * (cmd & 3), checks that it is playing (non-zero priority), then compares its
 * serial against (cmd >> 4). Every start bumps the slot's serial, so the id
 * of a vibration whose slot was taken over no longer matches.
 *
 * @param cmd Vibration id: bits [1:0] = slot index, bits [15:4] = serial.
 * @return 1 if playing and the serial matches, 0 otherwise.
 */
s32 isVibrationPlaying(s32 cmd) {
    VibrationSlot *base;
    VibrationSlot *entry;

    if (cmd == 0) {
        return 0;
    }
    base = getVibrationTable();
    entry = &base[cmd & 3];
    if (entry->priority != 0) {
        if (entry->serial == (cmd >> 4)) {
            return 1;
        }
    }
    return 0;
}


/**
 * @brief Stop one vibration, or all of them.
 *
 * If @p id is -1, clears all 4 slots' priority and zeroes the pad's
 * motor values (setPadMotors). Otherwise, if @p id is still playing
 * (isVibrationPlaying), clears just that slot's priority.
 *
 * @param id Vibration id from startVibration, or -1 to stop all.
 */
void stopVibration(s32 id) {
    VibrationSlot* ptr = getVibrationTable();
    s32 i;

    if (id == -1) {
        for (i = 0; i < 4; i++, ptr++) {
            ptr->priority = 0;
        }
        setPadMotors(0, 0, 0);
    } else {
        if (isVibrationPlaying(id)) {
            ptr += id & 3;
            ptr->priority = 0;
        }
    }
}


/**
 * @brief Start vibration pattern @p idx of the packed block at @p data.
 *
 * Takes a slot via findVibrationSlot, then loads the pattern's two streams
 * from the packed block at @p data. The block contains an offset table
 * followed by variable-length VibrationPatterns: two u16 stream lengths, then
 * the two streams' data.
 *
 * @param data Pointer to the packed block (offset table + streams).
 * @param idx Index into the offset table.
 * @param priority Priority of the vibration, kept in the slot.
 * @return Vibration id (serial << 4 | index), 0 if no slot was free, -1 if no data.
 */
s32 startVibration(u8 *data, s32 idx, s32 priority) {
    VibrationSlot *cmd;
    s32 offset;
    VibrationPattern *hdr;
    u16 len1, len2;
    u8 *base;
    u8 *block2;

    cmd = findVibrationSlot(priority);
    if (cmd == NULL) {
        return 0;
    }

    offset = ((s32 *)data)[idx];
    data += offset;
    if (offset == 0) {
        return -1;
    }

    hdr = (VibrationPattern *)data;
    len1 = hdr->len1;
    len2 = hdr->len2;
    base = hdr->data;
    block2 = hdr->data + len1;

    if (len1 != 0) {
        cmd->streams[0].start = base;
        cmd->streams[0].end = base + len1;
        cmd->streams[0].cursor = -1;
        cmd->streams[0].enabled = 1;
        cmd->streams[0].length = len1;
    } else {
        cmd->streams[0].enabled = 0;
    }

    if (len2 != 0) {
        cmd->streams[1].start = block2;
        cmd->streams[1].end = block2 + len2;
        cmd->streams[1].cursor = -1;
        cmd->streams[1].enabled = 1;
        cmd->streams[1].length = len2;
    } else {
        cmd->streams[1].enabled = 0;
    }

    cmd->priority = priority;
    cmd->serial++;
    if (cmd->serial >= 0x400) {
        cmd->serial = 1;
    }

    return (cmd->serial << 4) | cmd->index;
}


/**
 * @brief Read and interpolate the next value from a motor stream.
 *
 * Reads keyframe pairs (value, duration) from the stream. Linearly
 * interpolates between the current value and the next over the
 * duration, advances the cursor each call, and moves to the next
 * keyframe pair when the duration expires. Returns the interpolated
 * result doubled and clamped to 0-255, or -1 if the stream is
 * disabled or exhausted (0xFF duration marker).
 *
 * Stream format: [val0][dur0][val1][dur1]...[0xFF]
 *
 * @param stream Pointer to a MotorStream.
 * @return Interpolated value (0-255), or -1 if stream ended.
 * @see https://decomp.me/scratch/oOOHt
 */
static s32 stepMotorStream(MotorStream *stream) {
    u8 duration;
    u8 *ptr;
    u8 *end;

    s32 cursor;
    u16 cursor_u;

    s32 val1;
    s32 val2;
    s32 steps;
    s32 v1;

    s32 cond;

    if (!stream->enabled) {
        return -1;
    }

    ptr = stream->start;
    end = stream->end;

    cursor = stream->cursor;
    cursor_u = stream->cursor;

    if (!(ptr < end)) {
        stream->enabled = 0;
        return -1;
    }

    duration = ptr[1];

    /*
     * This must be the same variable that later becomes ptr[0],
     * so GCC keeps it in a1:
     *
     *   andi a1, v1, 0xFF
     *   beq  a1, v0, ...
     *   slt  v0, a3, a1
     */
    val1 = duration;

    if (val1 == 0xFF) {
        return -1;
    }

    cond = cursor < val1;

    val1 = ptr[0];
    val2 = ptr[2];

    cursor++;

    if (cond) {
        goto interpolate;
    }

    val1 = val2;
    stream->start = ptr + 2;
    stream->cursor = 0;

    goto clamp;

interpolate:
    steps = (u8)(duration + 1);

    val1 = val1 * (steps - cursor);
    val1 += val2 * cursor;
    val1 /= steps;

    stream->cursor = cursor_u + 1;

clamp:
    val1 <<= 1;

    if (val1 >= 0) {
        if (val1 < 0x100) {
            v1 = val1;
        } else {
            v1 = 0xFF;
        }
    } else {
        v1 = 0;
    }

    return v1;
}


/**
 * @brief Step the four vibration slots' motor streams by one tick.
 *
 * Every playing vibration reads the next value of both its streams
 * (stepMotorStream); one whose streams have both ended is stopped.
 * The largest positive values of stream 0 and stream 1 over the playing
 * vibrations are kept in g_motorPeak0 and g_motorPeak1, then clamped to 0-255
 * and passed to setPadMotors for pad port 0 (0 and 0 when none is playing).
 */
void stepVibrations(void) {
    VibrationSlot *entry;
    s32 i;
    s32 val0;
    s32 val1;
    s32 running;
    s32 anyRunning; /* never initialised: an initialiser adds an instruction the original lacks; harmless, as both peaks start at 0 */
    s32 peak0;
    s32 peak1;

    entry = getVibrationTable();
    g_motorPeak0 = 0;
    g_motorPeak1 = 0;
    for (i = 0; i < 4; i++, entry++) {
        running = 0;
        if (entry->priority != 0) {
            val0 = stepMotorStream(&entry->streams[0]);
            val1 = stepMotorStream(&entry->streams[1]);
            if (val0 == -1 && val1 == -1) {
                entry->priority = 0;
            } else {
                if (val0 > 0 && val0 > g_motorPeak0) {
                    g_motorPeak0 = val0;
                }
                running = 1;
                if (val1 > 0 && val1 > g_motorPeak1) {
                    g_motorPeak1 = val1;
                }
            }
        }
        anyRunning |= running;
    }
    if (anyRunning) {
        peak0 = g_motorPeak0;
        peak1 = g_motorPeak1;
    } else {
        peak0 = 0;
        peak1 = 0;
    }
    peak0 = peak0 < 0 ? 0 : (peak0 > 255 ? 255 : peak0);
    peak1 = peak1 < 0 ? 0 : (peak1 > 255 ? 255 : peak1);
    setPadMotors(0, peak0, peak1);
}


/**
 * @brief Advance the vibration clock and step the vibrations.
 *
 * Accumulates @p delta into g_engine.vibrationClock. For every 4 units accumulated,
 * calls stepVibrations() once. The remainder is stored back.
 *
 * @param delta Amount to add to the clock.
 */
void advanceVibrationClock(s32 delta) {
    s32 counter = g_engine.vibrationClock;
    counter += delta;
top:
    if (counter >= 4) {
        stepVibrations();
        counter -= 4;
        goto top;
    }
    g_engine.vibrationClock = counter;
}


/**
 * @brief Reset the 4 vibration slots and the clock that paces them.
 *
 * Sets each slot's index to its number, clears its priority, and sets its
 * serial to 1. Zeroes g_engine.vibrationClock.
 */
void initVibration(void) {
    VibrationSlot* ptr = getVibrationTable();
    s32 i;

    for (i = 0; i < 4; i++, ptr++) {
        ptr->index = i;
        ptr->priority = 0;
        ptr->serial = 1;
    }

    g_engine.vibrationClock = 0;
}
