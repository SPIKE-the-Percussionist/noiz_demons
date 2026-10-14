/* SPDX-License-Identifier: GPL-3.0-only
 * goetia.h - Ars Goetia sigil roster for noiz_magick chaos mode.
 *
 * Each demon has a rank. The rank sets how far a chaos roll may move a parameter
 * (goetia_rank_depth), and the demon's sigil is drawn in the graph strip (ui_goetia.c).
 * All tables are const, so they stay in flash. Included by ui_graph.c (one compilation unit: felucca.c).
 */
#ifndef GOETIA_H
#define GOETIA_H
#include <stdint.h>

/* Least to most chaotic. The order is a design choice, not from the source texts. */
typedef enum {
    GOETIA_KNIGHT = 0,
    GOETIA_PRESIDENT,
    GOETIA_EARL,
    GOETIA_MARQUIS,
    GOETIA_DUKE,
    GOETIA_PRINCE,
    GOETIA_KING,
    GOETIA_RANK_COUNT
} goetia_rank_t;

typedef struct { int8_t x0, y0, x1, y1; } goetia_seg_t;   /* one line stroke */
typedef struct { int8_t x, y; } goetia_pt_t;               /* one node dot */

typedef struct {
    const char         *name;
    uint8_t             rank;        /* goetia_rank_t */
    uint8_t             seg_count;
    uint8_t             mark_start;  /* segs[mark_start..] are the rank mark (accent colour) */
    const goetia_seg_t *segs;
    const goetia_pt_t  *nodes;       /* 0: none */
    uint8_t             node_count;
} goetia_demon_t;

extern const goetia_demon_t goetia_registry[];
extern const uint8_t        goetia_count;
extern const uint8_t        goetia_rank_depth[GOETIA_RANK_COUNT];
extern const char *const    goetia_rank_name[GOETIA_RANK_COUNT];

/* How far a roll by this demon may move a parameter, 0-255: its own step on the 30-step ladder
 * (goetia.c goetia_demon_depth_tbl), Furcas lowest, Belial alone at 255. 0: Furcas's. */
uint8_t goetia_demon_depth(const goetia_demon_t *d);

/* Sigil units span y -21..+19, x -14..+14: GOETIA_SPAN_Y units tall in all */
#define GOETIA_SPAN_Y 42

/* Drawing callbacks: the fork's own routines (gfx.c). Colours are RGB565. */
typedef void (*goetia_line_fn)(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color);
typedef void (*goetia_dot_fn)(int32_t x, int32_t y, int32_t r, uint16_t color);

/* A demon of the given rank; rnd is any random number. 0 if the rank has none. */
const goetia_demon_t *goetia_pick(goetia_rank_t rank, uint32_t rnd);

/* Draw a sigil centred at (cx, cy). scale_q4 = pixels per sigil unit x 16 (16 = 1 px per unit;
 * 43 fills the 122 px graph strip). dot may be 0. */
void goetia_draw(const goetia_demon_t *d, int32_t cx, int32_t cy, int32_t scale_q4,
                 goetia_line_fn line, goetia_dot_fn dot,
                 uint16_t base_color, uint16_t mark_color);

#endif
