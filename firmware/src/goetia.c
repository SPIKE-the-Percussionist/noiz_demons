/* SPDX-License-Identifier: GPL-3.0-only
 * goetia.c - Ars Goetia sigil roster for noiz_magick chaos mode.
 *
 * Generated from the sigil preview script; edit the script and regenerate rather than
 * hand-editing coordinates. Coordinates are in sigil units (about -21..21), centred on 0,0,
 * Y pointing down. goetia_draw() scales them to pixels.
 */
#include "goetia.h"

const char *const goetia_rank_name[GOETIA_RANK_COUNT] = {"KNIGHT", "PRESIDENT", "EARL", "MARQUIS", "DUKE", "PRINCE", "KING"};

/* Max reroll depth per rank, 0-255. No longer what a roll uses (each demon has its own: goetia_demon_depth_tbl
 * below); kept only as the fallback for a demon added to the roster without a depth of its own. */
const uint8_t goetia_rank_depth[GOETIA_RANK_COUNT] = {
    [GOETIA_KNIGHT] = 38,     /* ~15% */
    [GOETIA_PRESIDENT] = 64,  /* ~25% */
    [GOETIA_EARL] = 102,      /* ~40% */
    [GOETIA_MARQUIS] = 140,   /* ~55% */
    [GOETIA_DUKE] = 179,      /* ~70% */
    [GOETIA_PRINCE] = 217,    /* ~85% */
    [GOETIA_KING] = 255,      /* ~100% */
};

/* FURCAS - KNIGHT (tier 1): 2 base + 2 rank-mark strokes */
static const goetia_seg_t furcas_segs[] = {
    {   0,  -12,    0,   12},
    {  -7,    6,    7,    6},
    /* rank mark */
    {  -1,  -17,    1,  -17},
    {  -1,  -16,    1,  -16},
};

/* MARBAS - PRESIDENT (tier 2): 5 base + 2 rank-mark strokes */
static const goetia_seg_t marbas_segs[] = {
    {   0,  -12,    0,   13},
    { -12,    0,   12,    0},
    {  -3,   13,    3,   13},
    { -12,   -3,  -12,    3},
    {  12,   -3,   12,    3},
    /* rank mark */
    {  -5,  -16,    5,  -16},
    {   0,  -16,    0,  -13},
};

/* FURFUR - EARL (tier 3): 5 base + 4 rank-mark strokes */
static const goetia_seg_t furfur_segs[] = {
    {   0,   13,  -11,   -6},
    { -11,   -6,   11,   -7},
    {  11,   -7,    0,   13},
    {   0,   -7,    0,   13},
    {  -4,    4,    4,    4},
    /* rank mark */
    {   0,  -20,    3,  -17},
    {   3,  -17,    0,  -14},
    {   0,  -14,   -3,  -17},
    {  -3,  -17,    0,  -20},
};

/* SAMIGINA - MARQUIS (tier 4): 7 base + 2 rank-mark strokes */
static const goetia_seg_t samigina_segs[] = {
    {   0,  -11,   11,    8},
    {  11,    8,  -11,    9},
    { -11,    9,    0,  -11},
    {  -6,    5,    6,    5},
    {   0,  -11,    0,   13},
    { -11,    9,  -14,   13},
    {  11,    9,   14,   13},
    /* rank mark */
    {  -5,  -15,    0,  -20},
    {   0,  -20,    5,  -15},
};

/* AGARES - DUKE (tier 5): 19 base + 2 rank-mark strokes */
static const goetia_seg_t agares_segs[] = {
    {  12,   -5,   12,    5},
    {  12,    5,    5,   12},
    {   5,   12,   -5,   12},
    {  -5,   12,  -12,    5},
    { -12,    5,  -12,   -5},
    { -12,   -5,   -5,  -12},
    {  -5,  -12,    5,  -12},
    {   5,  -12,   12,   -5},
    {   6,   -2,    6,    2},
    {   6,    2,    2,    6},
    {   2,    6,   -2,    6},
    {  -2,    6,   -6,    2},
    {  -6,    2,   -6,   -2},
    {  -6,   -2,   -2,   -6},
    {  -2,   -6,    2,   -6},
    {   2,   -6,    6,   -2},
    {   0,   -6,    0,   13},
    { -13,    0,   -6,    0},
    {   6,    0,   13,    0},
    /* rank mark */
    {  -3,  -20,   -3,  -15},
    {   3,  -20,    3,  -15},
};

/* VASSAGO - PRINCE (tier 6): 14 base + 6 rank-mark strokes */
static const goetia_seg_t vassago_segs[] = {
    {  12,   -5,   12,    5},
    {  12,    5,    5,   12},
    {   5,   12,   -5,   12},
    {  -5,   12,  -12,    5},
    { -12,    5,  -12,   -5},
    { -12,   -5,   -5,  -12},
    {  -5,  -12,    5,  -12},
    {   5,  -12,   12,   -5},
    {   0,   -8,    7,    4},
    {   7,    4,   -7,    4},
    {  -7,    4,    0,   -8},
    {   0,    0,    0,   13},
    {   0,    0,  -11,    7},
    {   0,    0,   11,    7},
    /* rank mark */
    {   0,  -21,    3,  -20},
    {   3,  -20,    3,  -16},
    {   3,  -16,    0,  -15},
    {   0,  -15,   -3,  -16},
    {  -3,  -16,   -3,  -20},
    {  -3,  -20,    0,  -21},
};

/* BAEL - KING (tier 7): 8 base + 3 rank-mark strokes */
static const goetia_seg_t bael_segs[] = {
    {   0,  -14,    8,   11},
    {   8,   11,  -13,   -4},
    { -13,   -4,   13,   -4},
    {  13,   -4,   -8,   11},
    {  -8,   11,    0,  -14},
    {  -8,   11,   -8,   16},
    {   8,   11,    8,   16},
    {  -4,    4,    4,    4},
    /* rank mark */
    {   0,  -14,    0,  -21},
    {  -4,  -14,   -6,  -20},
    {   4,  -14,    6,  -20},
};
static const goetia_pt_t bael_nodes[] = {
    {0, -14}, {13, -4}, {8, 11}, {-8, 11}, {-13, -4},
};

/* PAIMON - KING (tier 7): 8 base + 3 rank-mark strokes */
static const goetia_seg_t paimon_segs[] = {
    { -13,    4,   -7,   -6},
    {  -7,   -6,    0,    2},
    {   0,    2,    7,   -6},
    {   7,   -6,   13,    4},
    {   0,  -14,    0,   13},
    {  -6,   13,    6,   13},
    { -13,    4,  -13,   10},
    {  13,    4,   13,   10},
    /* rank mark */
    {   0,  -14,    0,  -21},
    {  -4,  -14,   -6,  -20},
    {   4,  -14,    6,  -20},
};
static const goetia_pt_t paimon_nodes[] = {
    {-7, -6}, {7, -6},
};

/* PURSON - KING (tier 7): 11 base + 3 rank-mark strokes */
static const goetia_seg_t purson_segs[] = {
    {   0,  -14,    0,  -12},
    {   0,  -12,   10,   -6},
    {  10,   -6,   10,    6},
    {  10,    6,    0,   12},
    {   0,   12,  -10,    6},
    { -10,    6,  -10,   -6},
    { -10,   -6,    0,  -12},
    {  -5,   -7,    5,   -3},
    {   5,   -3,   -5,    1},
    {  -5,    1,    5,    5},
    {   0,   12,    0,   16},
    /* rank mark */
    {   0,  -14,    0,  -21},
    {  -4,  -14,   -6,  -20},
    {   4,  -14,    6,  -20},
};
static const goetia_pt_t purson_nodes[] = {
    {-5, -7}, {5, 5},
};

/* BELIAL - KING (tier 7): 11 base + 3 rank-mark strokes */
static const goetia_seg_t belial_segs[] = {
    {   0,  -14,    0,    8},
    {   0,   -4,  -13,  -11},
    { -13,  -11,  -10,    3},
    { -10,    3,    0,    2},
    {   0,   -4,   13,  -11},
    {  13,  -11,   10,    3},
    {  10,    3,    0,    2},
    {   0,    8,    5,   12},
    {   5,   12,    0,   16},
    {   0,   16,   -5,   12},
    {  -5,   12,    0,    8},
    /* rank mark */
    {   0,  -14,    0,  -21},
    {  -4,  -14,   -6,  -20},
    {   4,  -14,    6,  -20},
};
static const goetia_pt_t belial_nodes[] = {
    {0, 12},
};

/* ASMODEUS - KING (tier 7): 9 base + 3 rank-mark strokes */
static const goetia_seg_t asmodeus_segs[] = {
    {   0,  -14,    0,    6},
    { -10,  -12,  -10,   -2},
    {  10,  -12,   10,   -2},
    { -10,   -2,   10,   -2},
    { -10,  -12,  -13,   -8},
    {  10,  -12,   13,   -8},
    {   0,    6,   -6,   10},
    {  -6,   10,    0,   14},
    {   0,   14,    6,   16},
    /* rank mark */
    {   0,  -14,    0,  -21},
    {  -4,  -14,   -6,  -20},
    {   4,  -14,    6,  -20},
};
static const goetia_pt_t asmodeus_nodes[] = {
    {-10, -12}, {10, -12},
};

/* IPOS - PRINCE (tier 6): 9 base + 6 rank-mark strokes */
static const goetia_seg_t ipos_segs[] = {
    {   0,   13,  -11,   -4},
    { -11,   -4,   11,   -4},
    {  11,   -4,    0,   13},
    {   0,   -4,    0,    6},
    {  -6,    2,    6,    2},
    {   0,   -4,   -4,  -13},
    {  -4,  -13,   -2,  -14},
    {   0,   -4,    4,  -13},
    {   4,  -13,    2,  -14},
    /* rank mark */
    {   0,  -21,    3,  -20},
    {   3,  -20,    3,  -16},
    {   3,  -16,    0,  -15},
    {   0,  -15,   -3,  -16},
    {  -3,  -16,   -3,  -20},
    {  -3,  -20,    0,  -21},
};
static const goetia_pt_t ipos_nodes[] = {
    {0, 13},
};

/* SITRI - PRINCE (tier 6): 16 base + 6 rank-mark strokes */
static const goetia_seg_t sitri_segs[] = {
    {   0,  -10,    6,  -13},
    {   6,  -13,    5,   -6},
    {   0,  -10,   -6,  -13},
    {  -6,  -13,   -5,   -6},
    {  -5,   -6,    5,   -6},
    {   5,   -6,    7,    4},
    {   7,    4,    0,   10},
    {   0,   10,   -7,    4},
    {  -7,    4,   -5,   -6},
    {  -3,   -1,   -1,   -1},
    {   1,   -1,    3,   -1},
    {   0,    2,    0,    5},
    {  -7,    4,  -13,    0},
    { -13,    0,  -11,    8},
    {   7,    4,   13,    0},
    {  13,    0,   11,    8},
    /* rank mark */
    {   0,  -21,    3,  -20},
    {   3,  -20,    3,  -16},
    {   3,  -16,    0,  -15},
    {   0,  -15,   -3,  -16},
    {  -3,  -16,   -3,  -20},
    {  -3,  -20,    0,  -21},
};

/* GAAP - PRINCE (tier 6): 10 base + 6 rank-mark strokes */
static const goetia_seg_t gaap_segs[] = {
    {   0,  -12,   11,    0},
    {  11,    0,    0,   12},
    {   0,   12,  -11,    0},
    { -11,    0,    0,  -12},
    {   0,  -12,    0,  -16},
    {   0,   12,    0,   13},
    {  11,    0,   14,    0},
    { -11,    0,  -14,    0},
    {   0,   -6,    0,    6},
    {  -6,    0,    6,    0},
    /* rank mark */
    {   0,  -21,    3,  -20},
    {   3,  -20,    3,  -16},
    {   3,  -16,    0,  -15},
    {   0,  -15,   -3,  -16},
    {  -3,  -16,   -3,  -20},
    {  -3,  -20,    0,  -21},
};
static const goetia_pt_t gaap_nodes[] = {
    {0, 0},
};

/* VALEFOR - DUKE (tier 5): 9 base + 2 rank-mark strokes */
static const goetia_seg_t valefor_segs[] = {
    {   0,  -13,    5,   -8},
    {   5,   -8,    0,   -3},
    {   0,   -3,   -5,   -8},
    {  -5,   -8,    0,  -13},
    {   0,   -3,    0,   13},
    {   0,    8,    5,    8},
    {   0,   12,    4,   12},
    {  -3,  -11,  -10,  -14},
    {   3,  -11,   10,  -14},
    /* rank mark */
    {  -3,  -20,   -3,  -15},
    {   3,  -20,    3,  -15},
};
static const goetia_pt_t valefor_nodes[] = {
    {0, -8},
};

/* BARBATOS - DUKE (tier 5): 11 base + 2 rank-mark strokes */
static const goetia_seg_t barbatos_segs[] = {
    {  -6,  -12,   -1,   -4},
    {  -1,   -4,   -1,    4},
    {  -1,    4,   -6,   12},
    {  -6,  -12,   -6,   12},
    { -13,    0,   13,    0},
    {  13,    0,    9,   -4},
    {  13,    0,    9,    4},
    { -10,    0,  -13,   -3},
    { -10,    0,  -13,    3},
    {   5,  -10,    8,   -7},
    {   8,   -7,   11,  -10},
    /* rank mark */
    {  -3,  -20,   -3,  -15},
    {   3,  -20,    3,  -15},
};

/* ASTAROTH - DUKE (tier 5): 8 base + 2 rank-mark strokes */
static const goetia_seg_t astaroth_segs[] = {
    { -11,  -12,   -7,   -4},
    {  -7,   -4,    0,   -1},
    {   0,   -1,    7,   -4},
    {   7,   -4,   11,  -12},
    {   0,   -1,    0,   14},
    {  -6,    3,    5,    6},
    {   5,    6,   -5,    9},
    {  -5,    9,    6,   12},
    /* rank mark */
    {  -3,  -20,   -3,  -15},
    {   3,  -20,    3,  -15},
};
static const goetia_pt_t astaroth_nodes[] = {
    {-11, -12}, {11, -12},
};

/* FOCALOR - DUKE (tier 5): 13 base + 2 rank-mark strokes */
static const goetia_seg_t focalor_segs[] = {
    {   0,  -13,    0,   13},
    {   0,   13,   -4,    9},
    {   0,   13,    4,    9},
    {   0,   -8,  -11,  -13},
    {   0,   -8,   11,  -13},
    {   0,   -4,   -9,   -8},
    {   0,   -4,    9,   -8},
    { -12,    4,   -8,    0},
    {  -8,    0,   -4,    4},
    {  -4,    4,    0,    0},
    {   0,    0,    4,    4},
    {   4,    4,    8,    0},
    {   8,    0,   12,    4},
    /* rank mark */
    {  -3,  -20,   -3,  -15},
    {   3,  -20,    3,  -15},
};

/* AMON - MARQUIS (tier 4): 10 base + 2 rank-mark strokes */
static const goetia_seg_t amon_segs[] = {
    {  -8,  -12,  -10,    0},
    { -10,    0,    0,    9},
    {   0,    9,   10,    0},
    {  10,    0,    8,  -12},
    {  -8,  -12,   -3,   -5},
    {   8,  -12,    3,   -5},
    {  -3,   -5,    3,   -5},
    {   0,    9,   -4,   14},
    {   0,    9,    0,   16},
    {   0,    9,    4,   14},
    /* rank mark */
    {  -5,  -15,    0,  -20},
    {   0,  -20,    5,  -15},
};
static const goetia_pt_t amon_nodes[] = {
    {-4, 0}, {4, 0},
};

/* PHENEX - MARQUIS (tier 4): 13 base + 2 rank-mark strokes */
static const goetia_seg_t phenex_segs[] = {
    {   0,  -12,    0,    5},
    {   0,  -12,    3,  -10},
    {   0,   -2,  -12,  -11},
    {   0,   -2,  -13,   -5},
    {   0,   -2,   12,  -11},
    {   0,   -2,   13,   -5},
    {  -8,   13,   -5,    6},
    {  -5,    6,   -2,   11},
    {  -2,   11,    0,    5},
    {   0,    5,    2,   11},
    {   2,   11,    5,    6},
    {   5,    6,    8,   13},
    {  -8,   13,    8,   13},
    /* rank mark */
    {  -5,  -15,    0,  -20},
    {   0,  -20,    5,  -15},
};
static const goetia_pt_t phenex_nodes[] = {
    {0, -12},
};

/* ANDRAS - MARQUIS (tier 4): 7 base + 2 rank-mark strokes */
static const goetia_seg_t andras_segs[] = {
    { -11,   -9,   11,    3},
    {  11,   -9,  -11,    3},
    {   0,  -13,    0,    7},
    {   0,  -13,   -2,  -10},
    {   0,  -13,    2,  -10},
    {  -7,    7,    7,    7},
    {   0,    7,    0,   13},
    /* rank mark */
    {  -5,  -15,    0,  -20},
    {   0,  -20,    5,  -15},
};
static const goetia_pt_t andras_nodes[] = {
    {0, 13},
};

/* SHAX - MARQUIS (tier 4): 11 base + 2 rank-mark strokes */
static const goetia_seg_t shax_segs[] = {
    {  -8,   -2,    0,   -6},
    {   0,   -6,    8,   -2},
    {   8,   -2,    0,    2},
    {   0,    2,   -8,   -2},
    {  -8,   -2,  -10,  -11},
    { -10,  -11,   -2,   -9},
    {  -2,    2,   -3,   13},
    {   2,    2,    4,   13},
    {   8,   -2,   12,    1},
    {  -6,   13,    0,   13},
    {   1,   13,    7,   13},
    /* rank mark */
    {  -5,  -15,    0,  -20},
    {   0,  -20,    5,  -15},
};
static const goetia_pt_t shax_nodes[] = {
    {-10, -11},
};

/* RAUM - EARL (tier 3): 13 base + 4 rank-mark strokes */
static const goetia_seg_t raum_segs[] = {
    {  -6,  -10,    2,   -6},
    {   2,   -6,   12,    8},
    {  12,    8,    0,    4},
    {   0,    4,   -6,   -4},
    {  -6,   -4,   -6,  -10},
    {  -6,  -10,  -13,   -7},
    { -13,   -7,   -6,   -4},
    {   0,    4,   -2,   12},
    {   4,    6,    4,   12},
    { -11,    0,   -8,    3},
    {  -8,    3,  -11,    6},
    { -11,    6,  -14,    3},
    { -14,    3,  -11,    0},
    /* rank mark */
    {   0,  -20,    3,  -17},
    {   3,  -17,    0,  -14},
    {   0,  -14,   -3,  -17},
    {  -3,  -17,    0,  -20},
};
static const goetia_pt_t raum_nodes[] = {
    {-4, -7},
};

/* BIFRONS - EARL (tier 3): 10 base + 4 rank-mark strokes */
static const goetia_seg_t bifrons_segs[] = {
    {  -7,   13,   -7,   -2},
    {  -7,   -2,   -4,   -6},
    {  -4,   -6,    4,   -6},
    {   4,   -6,    7,   -2},
    {   7,   -2,    7,   13},
    {   0,   -3,    0,    7},
    {  -3,    4,    3,    4},
    { -13,   13,   13,   13},
    { -11,   13,  -11,    5},
    {  11,   13,   11,    5},
    /* rank mark */
    {   0,  -20,    3,  -17},
    {   3,  -17,    0,  -14},
    {   0,  -14,   -3,  -17},
    {  -3,  -17,    0,  -20},
};
static const goetia_pt_t bifrons_nodes[] = {
    {-11, 3}, {11, 3},
};

/* HALPHAS - EARL (tier 3): 30 base + 4 rank-mark strokes */
static const goetia_seg_t halphas_segs[] = {
    {  -7,  -12,   -4,  -12},
    {  -4,  -12,   -4,   -9},
    {  -4,   -9,   -1,   -9},
    {  -1,   -9,   -1,  -12},
    {  -1,  -12,    1,  -12},
    {   1,  -12,    1,   -9},
    {   1,   -9,    4,   -9},
    {   4,   -9,    4,  -12},
    {   4,  -12,    7,  -12},
    {   7,  -12,    7,   -6},
    {   7,   -6,    4,   -4},
    {   4,   -4,    4,    7},
    {   4,    7,    7,    9},
    {   7,    9,    7,   11},
    {   7,   11,    8,   13},
    {   8,   13,   -8,   13},
    {  -7,   -6,   -7,  -12},
    {  -4,   -4,   -7,   -6},
    {  -4,    7,   -4,   -4},
    {  -7,    9,   -4,    7},
    {  -7,   11,   -7,    9},
    {  -8,   13,   -7,   11},
    {  -4,   -4,    4,   -4},
    {  -4,    7,    4,    7},
    { -12,   13,  -11,   -8},
    { -11,   -8,  -13,   -5},
    { -11,   -8,   -9,   -5},
    {  12,   13,   11,   -8},
    {  11,   -8,   13,   -5},
    {  11,   -8,    9,   -5},
    /* rank mark */
    {   0,  -20,    3,  -17},
    {   3,  -17,    0,  -14},
    {   0,  -14,   -3,  -17},
    {  -3,  -17,    0,  -20},
};

/* MALPHAS - EARL (tier 3): 20 base + 4 rank-mark strokes */
static const goetia_seg_t malphas_segs[] = {
    {   0,   -4,  -13,   -9},
    { -13,   -9,  -11,   -3},
    { -11,   -3,  -14,    1},
    { -14,    1,   -9,    1},
    {  -9,    1,  -11,    7},
    { -11,    7,   -4,    4},
    {  -4,    4,    0,    8},
    {   0,   -4,   13,   -9},
    {  13,   -9,   11,   -3},
    {  11,   -3,   14,    1},
    {  14,    1,    9,    1},
    {   9,    1,   11,    7},
    {  11,    7,    4,    4},
    {   4,    4,    0,    8},
    {   0,   -9,    0,    8},
    {   0,    8,   -4,   14},
    {   0,    8,    0,   15},
    {   0,    8,    4,   14},
    {  -2,  -11,    0,   -7},
    {   2,  -11,    0,   -7},
    /* rank mark */
    {   0,  -20,    3,  -17},
    {   3,  -17,    0,  -14},
    {   0,  -14,   -3,  -17},
    {  -3,  -17,    0,  -20},
};
static const goetia_pt_t malphas_nodes[] = {
    {0, -11},
};

/* BUER - PRESIDENT (tier 2): 30 base + 2 rank-mark strokes */
static const goetia_seg_t buer_segs[] = {
    {   1,   -2,    2,    1},
    {   2,    1,    0,    2},
    {   0,    2,   -2,    1},
    {  -2,    1,   -1,   -2},
    {  -1,   -2,    1,   -2},
    {   1,   -2,    6,   -3},
    {   6,   -3,    5,   -9},
    {   5,   -9,    8,  -11},
    {   8,  -11,    5,  -10},
    {   8,  -11,    8,   -8},
    {   2,    1,    5,    4},
    {   5,    4,   10,    2},
    {  10,    2,   12,    4},
    {  12,    4,   10,    2},
    {  12,    4,    9,    5},
    {   0,    2,   -2,    6},
    {  -2,    6,    2,   10},
    {   2,   10,    0,   13},
    {   0,   13,    2,   11},
    {   0,   13,   -2,   11},
    {  -2,    1,   -6,   -1},
    {  -6,   -1,   -9,    5},
    {  -9,    5,  -12,    4},
    { -12,    4,   -9,    5},
    { -12,    4,  -10,    2},
    {  -1,   -2,   -2,   -6},
    {  -2,   -6,   -7,   -7},
    {  -7,   -7,   -8,  -11},
    {  -8,  -11,   -8,   -8},
    {  -8,  -11,   -5,  -10},
    /* rank mark */
    {  -5,  -16,    5,  -16},
    {   0,  -16,    0,  -13},
};
static const goetia_pt_t buer_nodes[] = {
    {0, 0},
};

/* GLASYA-LABOLAS - PRESIDENT (tier 2): 12 base + 2 rank-mark strokes */
static const goetia_seg_t glasya_labolas_segs[] = {
    {   0,  -11,    0,   -5},
    {  -5,   -5,    5,   -5},
    {  -2,   -5,    0,   10},
    {   2,   -5,    0,   10},
    {  -5,   -5,  -13,  -11},
    { -13,  -11,  -10,   -6},
    { -10,   -6,  -13,   -2},
    { -13,   -2,   -6,   -4},
    {   5,   -5,   13,  -11},
    {  13,  -11,   10,   -6},
    {  10,   -6,   13,   -2},
    {  13,   -2,    6,   -4},
    /* rank mark */
    {  -5,  -16,    5,  -16},
    {   0,  -16,    0,  -13},
};
static const goetia_pt_t glasya_labolas_nodes[] = {
    {0, -11}, {0, 14},
};

/* FORAS - PRESIDENT (tier 2): 13 base + 2 rank-mark strokes */
static const goetia_seg_t foras_segs[] = {
    {   0,    8,    0,  -11},
    {   0,   -6,   -6,  -10},
    {   0,   -6,    6,  -10},
    {   0,   -1,   -9,   -4},
    {   0,   -1,    9,   -4},
    {   0,    4,   -7,    2},
    {   0,    4,    7,    2},
    {  -5,    8,    5,    8},
    {   5,    8,    8,   11},
    {   8,   11,    0,   16},
    {   0,   16,   -8,   11},
    {  -8,   11,   -5,    8},
    {  -8,   11,    8,   11},
    /* rank mark */
    {  -5,  -16,    5,  -16},
    {   0,  -16,    0,  -13},
};

/* HAAGENTI - PRESIDENT (tier 2): 17 base + 2 rank-mark strokes */
static const goetia_seg_t haagenti_segs[] = {
    {  -5,   -3,  -11,   -5},
    { -11,   -5,  -13,  -10},
    { -13,  -10,  -10,  -13},
    {   5,   -3,   11,   -5},
    {  11,   -5,   13,  -10},
    {  13,  -10,   10,  -13},
    {  -5,   -3,   -7,    5},
    {  -7,    5,    0,   11},
    {   0,   11,    7,    5},
    {   7,    5,    5,   -3},
    {   5,   -3,   -5,   -3},
    {   0,    1,    3,    4},
    {   3,    4,    0,    7},
    {   0,    7,   -3,    4},
    {  -3,    4,    0,    1},
    {  -7,    5,  -11,    3},
    {   7,    5,   11,    3},
    /* rank mark */
    {  -5,  -16,    5,  -16},
    {   0,  -16,    0,  -13},
};
static const goetia_pt_t haagenti_nodes[] = {
    {0, 4},
};

const goetia_demon_t goetia_registry[] = {
    {"FURCAS", GOETIA_KNIGHT, 4, 2, furcas_segs, 0, 0},
    {"MARBAS", GOETIA_PRESIDENT, 7, 5, marbas_segs, 0, 0},
    {"BUER", GOETIA_PRESIDENT, 32, 30, buer_segs, buer_nodes, 1},
    {"GLASYA-LABOLAS", GOETIA_PRESIDENT, 14, 12, glasya_labolas_segs, glasya_labolas_nodes, 2},
    {"FORAS", GOETIA_PRESIDENT, 15, 13, foras_segs, 0, 0},
    {"HAAGENTI", GOETIA_PRESIDENT, 19, 17, haagenti_segs, haagenti_nodes, 1},
    {"FURFUR", GOETIA_EARL, 9, 5, furfur_segs, 0, 0},
    {"RAUM", GOETIA_EARL, 17, 13, raum_segs, raum_nodes, 1},
    {"BIFRONS", GOETIA_EARL, 14, 10, bifrons_segs, bifrons_nodes, 2},
    {"HALPHAS", GOETIA_EARL, 34, 30, halphas_segs, 0, 0},
    {"MALPHAS", GOETIA_EARL, 24, 20, malphas_segs, malphas_nodes, 1},
    {"SAMIGINA", GOETIA_MARQUIS, 9, 7, samigina_segs, 0, 0},
    {"AMON", GOETIA_MARQUIS, 12, 10, amon_segs, amon_nodes, 2},
    {"PHENEX", GOETIA_MARQUIS, 15, 13, phenex_segs, phenex_nodes, 1},
    {"ANDRAS", GOETIA_MARQUIS, 9, 7, andras_segs, andras_nodes, 1},
    {"SHAX", GOETIA_MARQUIS, 13, 11, shax_segs, shax_nodes, 1},
    {"AGARES", GOETIA_DUKE, 21, 19, agares_segs, 0, 0},
    {"VALEFOR", GOETIA_DUKE, 11, 9, valefor_segs, valefor_nodes, 1},
    {"BARBATOS", GOETIA_DUKE, 13, 11, barbatos_segs, 0, 0},
    {"ASTAROTH", GOETIA_DUKE, 10, 8, astaroth_segs, astaroth_nodes, 2},
    {"FOCALOR", GOETIA_DUKE, 15, 13, focalor_segs, 0, 0},
    {"VASSAGO", GOETIA_PRINCE, 20, 14, vassago_segs, 0, 0},
    {"IPOS", GOETIA_PRINCE, 15, 9, ipos_segs, ipos_nodes, 1},
    {"SITRI", GOETIA_PRINCE, 22, 16, sitri_segs, 0, 0},
    {"GAAP", GOETIA_PRINCE, 16, 10, gaap_segs, gaap_nodes, 1},
    {"BAEL", GOETIA_KING, 11, 8, bael_segs, bael_nodes, 5},
    {"PAIMON", GOETIA_KING, 11, 8, paimon_segs, paimon_nodes, 2},
    {"PURSON", GOETIA_KING, 14, 11, purson_segs, purson_nodes, 2},
    {"BELIAL", GOETIA_KING, 14, 11, belial_segs, belial_nodes, 1},
    {"ASMODEUS", GOETIA_KING, 12, 9, asmodeus_segs, asmodeus_nodes, 2},
};

const uint8_t goetia_count = sizeof(goetia_registry) / sizeof(goetia_registry[0]);

/* Max reroll depth per DEMON, 0-255: one entry per goetia_registry row, in the same order (the lookup is by row).
 * A 30-step ladder from Furcas (38, ~15%) up to Belial alone at the top (255 = 100%, no guardrails): rank by rank,
 * and within a rank from the calmest to the wildest by each demon's temper (the lore). Every step is the same
 * size, so tune any one entry by ear. Only 255 rolls the whole range uniformly. */
static const uint8_t goetia_demon_depth_tbl[] = {
     38,  /* FURCAS          ~ 15%  step  1/30: the gentle baseline */
     60,  /* MARBAS          ~ 24%  step  4/30: reveals hidden things */
     45,  /* BUER            ~ 18%  step  2/30: teaches philosophy, heals the sick */
     75,  /* GLASYA-LABOLAS  ~ 29%  step  6/30: author of bloodshed */
     53,  /* FORAS           ~ 21%  step  3/30: logic and the virtue of herbs */
     68,  /* HAAGENTI        ~ 27%  step  5/30: transmutes metal to gold */
    113,  /* FURFUR          ~ 44%  step 11/30: raises storms and lightning */
    105,  /* RAUM            ~ 41%  step 10/30: steals treasure, destroys cities */
     83,  /* BIFRONS         ~ 33%  step  7/30: moves the dead between graves */
     90,  /* HALPHAS         ~ 35%  step  8/30: builds towers, arms them */
     98,  /* MALPHAS         ~ 38%  step  9/30: builds houses, throws them down */
    128,  /* SAMIGINA        ~ 50%  step 13/30: souls of the drowned */
    135,  /* AMON            ~ 53%  step 14/30: reconciles friends, breathes fire */
    120,  /* PHENEX          ~ 47%  step 12/30: a sweet-voiced poet */
    150,  /* ANDRAS          ~ 59%  step 16/30: sows discord, kills */
    143,  /* SHAX            ~ 56%  step 15/30: steals sight and hearing */
    180,  /* AGARES          ~ 71%  step 20/30: causes earthquakes */
    173,  /* VALEFOR         ~ 68%  step 19/30: tempts men to steal */
    158,  /* BARBATOS        ~ 62%  step 17/30: understands birdsong */
    165,  /* ASTAROTH        ~ 65%  step 18/30: answers of past and future */
    188,  /* FOCALOR         ~ 74%  step 21/30: drowns men, sinks warships */
    195,  /* VASSAGO         ~ 76%  step 22/30: good-natured, finds the lost */
    203,  /* IPOS            ~ 80%  step 23/30: gives wit and courage */
    210,  /* SITRI           ~ 82%  step 24/30: inflames love and lust */
    218,  /* GAAP            ~ 85%  step 25/30: stirs love and hate */
    233,  /* BAEL            ~ 91%  step 27/30: the first King */
    240,  /* PAIMON          ~ 94%  step 28/30: comes with trumpets */
    225,  /* PURSON          ~ 88%  step 26/30: speaks of hidden things */
    255,  /* BELIAL          ~100%  step 30/30: the most lawless of all */
    248,  /* ASMODEUS        ~ 97%  step 29/30: wrath and ruin */
};

uint8_t goetia_demon_depth(const goetia_demon_t *dm)
{
    uint32_t i;
    if (!dm)
        return goetia_demon_depth_tbl[0];
    i = (uint32_t)(dm - goetia_registry);
    if (i >= sizeof goetia_demon_depth_tbl)          /* (a demon added without a depth: its rank's) */
        return goetia_rank_depth[dm->rank % GOETIA_RANK_COUNT];
    return goetia_demon_depth_tbl[i];
}

const goetia_demon_t *goetia_pick(goetia_rank_t rank, uint32_t rnd)
{
    uint8_t i, matches = 0, want;
    for (i = 0; i < goetia_count; i++)
        if (goetia_registry[i].rank == (uint8_t)rank)
            matches++;
    if (!matches)
        return 0;
    want = (uint8_t)(rnd % matches);
    for (i = 0; i < goetia_count; i++)
        if (goetia_registry[i].rank == (uint8_t)rank && want-- == 0)
            return &goetia_registry[i];
    return 0;
}

void goetia_draw(const goetia_demon_t *d, int32_t cx, int32_t cy, int32_t scale_q4,
                 goetia_line_fn line, goetia_dot_fn dot,
                 uint16_t base_color, uint16_t mark_color)
{
    uint8_t i;
    if (!d || !line)
        return;
#define GOETIA_PX(c, v) ((c) + (((int32_t)(v) * scale_q4 + ((v) < 0 ? -8 : 8)) / 16))
    for (i = 0; i < d->seg_count; i++) {
        const goetia_seg_t *s = &d->segs[i];
        line(GOETIA_PX(cx, s->x0), GOETIA_PX(cy, s->y0), GOETIA_PX(cx, s->x1), GOETIA_PX(cy, s->y1),
             i < d->mark_start ? base_color : mark_color);
    }
    if (dot) {
        int32_t r = scale_q4 >= 32 ? scale_q4 / 16 : 1;
        for (i = 0; i < d->node_count; i++)
            dot(GOETIA_PX(cx, d->nodes[i].x), GOETIA_PX(cy, d->nodes[i].y), r, base_color);
    }
#undef GOETIA_PX
}
