#ifndef ITEM_H
#define ITEM_H

#include "common.h"

/* Declarations exported by item.c. */

/** @brief PRNG used throughout the game; returns a fresh random word. */
extern s32 func_80023D04(void);

/** @brief Add @p delta to the owned quantity of card/item @p itemId; returns the new count. */
extern s32 modifyItemQuantity(s32 itemId, s32 delta);

/** @brief Copies of Triple Triad card @p card the player has, or -1 if never obtained. */
extern s32 GetCardCount(s32 card);

/** @brief Non-zero if card/item @p cardId is present in the collection. */
extern s32 isItemPresent(s32 cardId);

/** @brief Look up a card's display-name string by card index. */
extern u8 *func_80023A54(s32 cardId);

/** @brief Give Triple Triad card @p cardId to the player; returns 0, or -1 when a monster card already has 100 copies. */
extern s32 markItemPresent(s32 cardId);

#endif /* ITEM_H */
