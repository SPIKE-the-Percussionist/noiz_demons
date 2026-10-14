/* SPDX-License-Identifier: GPL-3.0-only
 * GOETIA (noiz_magick): every CHAOS roll summons a demon (goetia.c). A random rank first, then one of its demons; the
 * demon sets how far that roll may move each value (goetia.c goetia_demon_depth: a 30-step ladder): FURCAS nudges
 * every value a little around where it is, and only BELIAL, at the top, rolls each one uniformly over its whole
 * range (CHAOS as it was: no guardrails).
 *   chaos_roll_here (RLL) is one summoning for everything it rolls (goetia_hold); a knob detent is one per detent.
 * The sigil: with CHAOS (RITUAL) on for the selected track, HOME and every page show that track's demon (one is
 * picked at power-on, before any summoning) in place of the scope or graph, its
 * strokes glowing and the whole sigil swelling with an envelope of the synth's level before MASTER (snaps up on a hit, eases back),
 * and on any page a roll shows the demon for GOETIA_FLASH_MS over the page's own graph (ui_graph.c draw_graph).
 * Included by ui_graph.c before ui_fm6op.c (fop_roll summons). */
#include "goetia.c"

#define GOETIA_FLASH_MS 1500u
static uint8_t goetia_last[NTRK];          /* per track: its last demon's registry index + 1, 0 none */
static uint8_t goetia_depth = 255u;         /* the roll in progress: its demon's depth */
static uint8_t goetia_hold;                 /* 1: chaos_roll_here's one summoning covers the rolls it makes */
static uint32_t goetia_at;                  /* fm1_ms of the last summoning */
static uint8_t goetia_any;                  /* 1: a demon has been summoned since power-on */

static uint32_t chaos_on(void);             /* (ui_chaos.c) */

/* (each demon's depth: goetia.c goetia_demon_depth, a 30-step ladder from Furcas to Belial; the old
 * per-demon TEMPER nudges now only set each demon's place on it) */

static void goetia_summon(void)
{
    const goetia_demon_t *d;
    uint8_t prev = goetia_last[song.sel % NTRK], idx, tries;
    if (goetia_hold)
        return;
    /* no immediate repeat: reroll (rank and all) until a different demon comes up, so every roll changes
     * the sigil. 8 tries is plenty with 30 in the roster; the cap just guards against a one-demon roster */
    for (tries = 0; tries < 8; tries++) {
        d = goetia_pick((goetia_rank_t)(rng() % GOETIA_RANK_COUNT), rng());
        if (!d)
            return;
        idx = (uint8_t)(d - goetia_registry + 1);
        if (idx != prev)
            break;
    }
    goetia_last[song.sel % NTRK] = idx;
    goetia_depth = goetia_demon_depth(d);          /* the demon's own step on the ladder (goetia.c) */
    goetia_at = fm1_ms;
    goetia_any = 1;
}

/* a value in lo..hi rolled from cur at the summoned demon's depth: the whole range at 255, else within
 * +- depth / 510 of the range around cur (at least one step), held at the ends */
static int32_t goetia_roll(int32_t cur, int32_t lo, int32_t hi)
{
    int32_t span = hi - lo + 1, w, v;
    if (span <= 1)
        return lo;
    if (goetia_depth >= 255u || cur < lo || cur > hi)
        return lo + (int32_t)(rng() % (uint32_t)span);
    w = span * (int32_t)goetia_depth / 510;
    if (w < 1)
        w = 1;
    v = cur - w + (int32_t)(rng() % (uint32_t)(2 * w + 1));
    return v < lo ? lo : v > hi ? hi : v;
}

/* the selected track's demon, 0 none */
static const goetia_demon_t *goetia_cur(void)
{
    uint32_t tr = song.sel % NTRK;
    uint8_t i;
    if (!goetia_last[tr] && goetia_count)            /* nothing summoned on this track yet: a demon keeps watch from power-on */
        goetia_last[tr] = (uint8_t)(1u + (rng() ^ fm1_ms * 2654435761u) % (uint32_t)goetia_count);
    i = goetia_last[tr];
    return i && i <= goetia_count ? &goetia_registry[i - 1u] : 0;
}
static int goetia_flashing(void) { return goetia_any && fm1_ms - goetia_at < GOETIA_FLASH_MS; }

/* draw_graph: the sigil in place of the graph? With RITUAL on for the selected track: always, on HOME and every
 * page (noiz_magick: the sigil is the only display; RITUAL off brings back the scope and the pages' own graphs) */
static int goetia_shown(void)
{
    return chaos_on() && goetia_cur() != 0;
}
/* graph_signature: redraws while the sigil glows (every other frame) and when a flash ends */
static uint32_t goetia_sig(void)
{
    if (!goetia_shown())
        return 0;
    return (ui.frame / 2u) * 2654435761u ^ (uint32_t)goetia_last[song.sel % NTRK] * 40503u ^ 0x60E71Au;
}

/* the strokes 2 px wide, the nodes discs */
static void goetia_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t c)
{
    cv_line(x0, y0, x1, y1, c);
    cv_line(x0 + 1, y0, x1 + 1, y1, c);
    cv_line(x0, y0 + 1, x1, y1 + 1, c);
}
static void goetia_dot(int32_t x, int32_t y, int32_t r, uint16_t c)
{
    int32_t j, w;
    for (j = -r; j <= r; j++) {
        for (w = r; w > 0 && w * w + j * j > r * r; w--)
            ;
        cv_rect(x - w, y + j, 2 * w + 1, 1, c);
    }
}

/* the synth's level now, 0..100: the loudest sample of the mix BEFORE MASTER since the last look (fx.c
 * goetia_take_mix), so the volume knob and MENU > DISPLAY > SCOPE do not change it. Reading clears the meter */
static int32_t goetia_level(void)
{
    int32_t peak = goetia_mix_peak;
    goetia_mix_peak = 0;
    return peak >= 12000 ? 100 : peak * 100 / 12000;
}

/* ---- audio-reactive pulse ----
 * An envelope of the level, 0..100: jumps straight up to a louder level (the hit), then falls back exponentially,
 * losing about two thirds of its height every GOETIA_DECAY_MS. Integer only; costs one goetia_level per redraw.
 *   GOETIA_DECAY_MS   longer = slower, smoother settle; shorter = twitchier
 *   GOETIA_REST_PCT   the sigil's size when silent, in % of full; full size on the loudest hits */
#define GOETIA_DECAY_MS 220
#define GOETIA_REST_PCT 48
static int32_t goetia_env256;               /* the envelope x256 (0..25600) */
static uint32_t goetia_env_ms;              /* fm1_ms of its last update */

static int32_t goetia_pulse(void)
{
    int32_t lvl = goetia_level() * 256, dt = (int32_t)(fm1_ms - goetia_env_ms);
    goetia_env_ms = fm1_ms;
    if (dt < 0 || dt > GOETIA_DECAY_MS)        /* first call, or a long gap: just fall the whole way */
        dt = GOETIA_DECAY_MS;
    goetia_env256 -= goetia_env256 * dt / GOETIA_DECAY_MS;
    if (lvl > goetia_env256)                   /* attack: instant */
        goetia_env256 = lvl;
    return goetia_env256 / 256;
}

/* the sigil on the graph canvas (h px tall): name top left, rank top right, strokes from DIM toward THEME and the
 * sigil from GOETIA_REST_PCT up to full size with the pulse (a flash: full THEME, full size), the rank mark in the
 * accent */
static void graph_goetia(uint32_t h)
{
    const goetia_demon_t *d = goetia_cur();
    int32_t room = (int32_t)h - 8, pulse = 100, glow, scale;
    if (!d)
        return;
    if (!goetia_flashing())                          /* (pulses on every page now; a summoning's flash: full size) */
        pulse = goetia_pulse();
    glow = 35 + pulse * 65 / 100;
    cv_oy = 0;
    if (h >= 100u) {                                 /* (MENU > LARGE's 62 px strip: the sigil only) */
        cv_text(12, 4, &AF_S, d->name, T_MID);
        cv_text_r(228, 4, &AF_S, goetia_rank_name[d->rank % GOETIA_RANK_COUNT], T_ACCENT, T_SURF);
        room -= 6;
    }
    scale = room * 16 / GOETIA_SPAN_Y;
    scale = scale * (GOETIA_REST_PCT + pulse * (100 - GOETIA_REST_PCT) / 100) / 100;
    if (scale < 1)
        scale = 1;
    goetia_draw(d, 120, (int32_t)h / 2 + (h >= 100u ? 4 : 1), scale, goetia_line, goetia_dot,
                ux_mix(T_DIM, T_THEME, glow), T_ACCENT);
}
