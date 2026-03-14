#ifndef CD_H
#define CD_H

#include "common.h"

/** @brief CD-ROM file descriptor (sector + size pair).
 *
 *  Used by func_8003882C / func_80038868 to identify data on disc.
 *  The master file table at 0x80097400 is an array of these descriptors,
 *  loaded from a hard-coded bootstrap descriptor (g_fileTableDesc).
 */
typedef struct {
    u32 sector; /**< 0x00: Starting CD sector number. */
    u32 size;   /**< 0x04: Data size in bytes. */
} CdFileDesc;   /* 0x08 = 8 bytes */

/** @brief Indices into the master CD file table (g_cdFileTable).
 *
 *  The table has 133 entries (1064 bytes), loaded from CD sector 826
 *  into RAM at 0x80097400 by func_80011E18(). Each entry is a CdFileDesc
 *  (sector + size pair). Entries marked UNKNOWN have no known references yet.
 */
enum CdFileId {
    CD_FILE_INIT_OVERLAY       =   0, /**< Init overlay (raw read) */
    CD_FILE_DISPLAY_INIT       =   1, /**< Display init overlay (LZSS) */
    CD_FILE_FIELD_ENGINE       =   2, /**< Field engine (LZSS) */
    CD_FILE_BATTLE_SCENE_TABLE =   3, /**< Battle scene table */
    CD_FILE_UNKNOWN_4          =   4,
    CD_FILE_UNKNOWN_5          =   5,
    CD_FILE_UNKNOWN_6          =   6,
    CD_FILE_UNKNOWN_7          =   7,
    CD_FILE_UNKNOWN_8          =   8,
    CD_FILE_UNKNOWN_9          =   9,
    CD_FILE_UNKNOWN_10         =  10,
    CD_FILE_UNKNOWN_11         =  11,
    CD_FILE_UNKNOWN_12         =  12,
    CD_FILE_UNKNOWN_13         =  13,
    CD_FILE_UNKNOWN_14         =  14,
    CD_FILE_UNKNOWN_15         =  15,
    CD_FILE_UNKNOWN_16         =  16,
    CD_FILE_UNKNOWN_17         =  17,
    CD_FILE_UNKNOWN_18         =  18,
    CD_FILE_UNKNOWN_19         =  19,
    CD_FILE_UNKNOWN_20         =  20,
    CD_FILE_UNKNOWN_21         =  21,
    CD_FILE_UNKNOWN_22         =  22,
    CD_FILE_BATTLE_ENGINE      =  23, /**< Battle engine overlay (LZSS) */
    CD_FILE_BATTLE_RENDER      =  24, /**< Battle render overlay (raw read) */
    CD_FILE_BATTLE_CODE        =  25, /**< Battle code overlay (raw read) */
    CD_FILE_FIELD_ENGINE_ALT   =  26, /**< Alt field engine (LZSS) */
    CD_FILE_SOUND_HEADER       =  27, /**< Sound bank header */
    CD_FILE_SOUND_SAMPLE_1     =  28, /**< Sound sample bank 1 */
    CD_FILE_SOUND_SAMPLE_2     =  29, /**< Sound sample bank 2 */
    CD_FILE_UNKNOWN_30         =  30,
    CD_FILE_UNKNOWN_31         =  31,
    CD_FILE_UNKNOWN_32         =  32,
    CD_FILE_UNKNOWN_33         =  33,
    CD_FILE_UNKNOWN_34         =  34,
    CD_FILE_UNKNOWN_35         =  35,
    CD_FILE_UNKNOWN_36         =  36,
    CD_FILE_UNKNOWN_37         =  37,
    CD_FILE_UNKNOWN_38         =  38,
    CD_FILE_UNKNOWN_39         =  39,
    CD_FILE_UNKNOWN_40         =  40,
    CD_FILE_UNKNOWN_41         =  41,
    CD_FILE_UNKNOWN_42         =  42,
    CD_FILE_UNKNOWN_43         =  43,
    CD_FILE_UNKNOWN_44         =  44,
    CD_FILE_UNKNOWN_45         =  45,
    CD_FILE_UNKNOWN_46         =  46,
    CD_FILE_UNKNOWN_47         =  47,
    CD_FILE_UNKNOWN_48         =  48,
    CD_FILE_UNKNOWN_49         =  49,
    CD_FILE_UNKNOWN_50         =  50,
    CD_FILE_UNKNOWN_51         =  51,
    CD_FILE_UNKNOWN_52         =  52,
    CD_FILE_UNKNOWN_53         =  53,
    CD_FILE_UNKNOWN_54         =  54,
    CD_FILE_UNKNOWN_55         =  55,
    CD_FILE_UNKNOWN_56         =  56,
    CD_FILE_UNKNOWN_57         =  57,
    CD_FILE_UNKNOWN_58         =  58,
    CD_FILE_UNKNOWN_59         =  59,
    CD_FILE_UNKNOWN_60         =  60,
    CD_FILE_UNKNOWN_61         =  61,
    CD_FILE_UNKNOWN_62         =  62,
    CD_FILE_UNKNOWN_63         =  63,
    CD_FILE_UNKNOWN_64         =  64,
    CD_FILE_UNKNOWN_65         =  65,
    CD_FILE_UNKNOWN_66         =  66,
    CD_FILE_UNKNOWN_67         =  67,
    CD_FILE_UNKNOWN_68         =  68,
    CD_FILE_UNKNOWN_69         =  69,
    CD_FILE_UNKNOWN_70         =  70,
    CD_FILE_UNKNOWN_71         =  71,
    CD_FILE_UNKNOWN_72         =  72,
    CD_FILE_UNKNOWN_73         =  73,
    CD_FILE_UNKNOWN_74         =  74,
    CD_FILE_UNKNOWN_75         =  75,
    CD_FILE_UNKNOWN_76         =  76,
    CD_FILE_UNKNOWN_77         =  77,
    CD_FILE_UNKNOWN_78         =  78,
    CD_FILE_UNKNOWN_79         =  79,
    CD_FILE_UNKNOWN_80         =  80,
    CD_FILE_UNKNOWN_81         =  81,
    CD_FILE_UNKNOWN_82         =  82,
    CD_FILE_UNKNOWN_83         =  83,
    CD_FILE_UNKNOWN_84         =  84,
    CD_FILE_UNKNOWN_85         =  85,
    CD_FILE_UNKNOWN_86         =  86,
    CD_FILE_UNKNOWN_87         =  87,
    CD_FILE_UNKNOWN_88         =  88,
    CD_FILE_UNKNOWN_89         =  89,
    CD_FILE_UNKNOWN_90         =  90,
    CD_FILE_UNKNOWN_91         =  91,
    CD_FILE_UNKNOWN_92         =  92,
    CD_FILE_UNKNOWN_93         =  93,
    CD_FILE_UNKNOWN_94         =  94,
    CD_FILE_UNKNOWN_95         =  95,
    CD_FILE_UNKNOWN_96         =  96,
    CD_FILE_UNKNOWN_97         =  97,
    CD_FILE_UNKNOWN_98         =  98,
    CD_FILE_UNKNOWN_99         =  99,
    CD_FILE_UNKNOWN_100        = 100,
    CD_FILE_UNKNOWN_101        = 101,
    CD_FILE_UNKNOWN_102        = 102,
    CD_FILE_UNKNOWN_103        = 103,
    CD_FILE_UNKNOWN_104        = 104,
    CD_FILE_UNKNOWN_105        = 105,
    CD_FILE_UNKNOWN_106        = 106,
    CD_FILE_UNKNOWN_107        = 107,
    CD_FILE_UNKNOWN_108        = 108,
    CD_FILE_UNKNOWN_109        = 109,
    CD_FILE_UNKNOWN_110        = 110,
    CD_FILE_UNKNOWN_111        = 111,
    CD_FILE_UNKNOWN_112        = 112,
    CD_FILE_UNKNOWN_113        = 113,
    CD_FILE_UNKNOWN_114        = 114,
    CD_FILE_UNKNOWN_115        = 115,
    CD_FILE_UNKNOWN_116        = 116,
    CD_FILE_UNKNOWN_117        = 117,
    CD_FILE_UNKNOWN_118        = 118,
    CD_FILE_UNKNOWN_119        = 119,
    CD_FILE_UNKNOWN_120        = 120,
    CD_FILE_UNKNOWN_121        = 121,
    CD_FILE_UNKNOWN_122        = 122,
    CD_FILE_UNKNOWN_123        = 123,
    CD_FILE_UNKNOWN_124        = 124,
    CD_FILE_UNKNOWN_125        = 125,
    CD_FILE_UNKNOWN_126        = 126,
    CD_FILE_UNKNOWN_127        = 127,
    CD_FILE_BATTLE_DATA        = 128, /**< Battle state data (raw, copied to D_80078E00) */
    CD_FILE_TEXTURES           = 129, /**< Texture data (uploaded to VRAM) */
    CD_FILE_MODELS             = 130, /**< Battle entity/model data */
    CD_FILE_LOOKUP_TABLE       = 131, /**< Lookup table (0x200 bytes) */
    CD_FILE_EXTRA_DATA         = 132, /**< Additional data (func_8002C100) */
    CD_FILE_COUNT              = 133
};

#endif /* CD_H */
