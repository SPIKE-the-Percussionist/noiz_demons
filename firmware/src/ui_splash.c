/* ui_splash.c - the NOIZ DEMONS boot splash (noiz_magick): about 2.25 s at power-on, then the normal UI.
 * Included by ui_draw.c (before ui_draw). ui_draw calls nzd_frame() every frame: while the splash plays it draws
 * it and the page is not; then (or at a new button / key press, or the UPDATE MODE countdown) it is over for good
 * and the page is drawn in full (ui.force). The stock Felucca card (draw_splash, main.c at power-on) is a plain
 * T_BG screen now, so the boot goes dark -> NOIZ DEMONS -> the page.
 * Blocky NOIZ / DEMONS letters (theme ink over a dark-red drop shadow) glitch in like a corrupted signal and snap
 * clean; two pixel Pazuzu demons, mirror twins, rise into the bottom corners together, blink, and breathe fire toward
 * each other, the two streams meeting under the title; then they sink back down and the credit fades in where they
 * stood (Felucca mod by / SPIKE the Percussionist); it holds a beat and hands off. NZD_SPEED sets the pace. Everything is filled rectangles snapped to a coarse grid (the low-res look),
 * drawn through a rect callback, so it costs nothing to keep and draws at any frame rate.
 * nzd_draw(t, ...) draws the whole 240x240 frame at t ms after the splash began (it clears to bg itself). */

#define NZD_SPEED 115u                           /* the whole animation's pace, % (100 = as designed) */
#define NZD_HOLD_MS 600u                         /* extra time the finished credit frame stays up, real ms */
#define NZD_MS (3500u * 100u / NZD_SPEED + NZD_HOLD_MS)

#ifndef NZD_RGB                               /* RGB565; match the panel's own colour macro if it differs */
#define NZD_RGB(r, g, b) ((uint16_t)((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3)))
#endif
#define NZD_SHADOW NZD_RGB(90, 10, 12)
#define NZD_EYE    NZD_RGB(255, 235, 90)
#define NZD_FIRE0  NZD_RGB(255, 240, 150)     /* fire, hot to cold */
#define NZD_FIRE1  NZD_RGB(255, 200, 40)
#define NZD_FIRE2  NZD_RGB(245, 120, 20)
#define NZD_FIRE3  NZD_RGB(210, 40, 20)
#define NZD_FIRE4  NZD_RGB(100, 18, 12)

typedef void (*nzd_rect_fn)(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t c);

/* ---- the letters: 7 rows each, '#' ink; widths vary (M is 7) ---- */
typedef struct { uint8_t w; const char *row[7]; } nzd_glyph_t;
static const nzd_glyph_t NZD_N = {6, {"##..##", "###.##", "######", "##.###", "##..##", "##..##", "##..##"}};
static const nzd_glyph_t NZD_O = {6, {".####.", "######", "##..##", "##..##", "##..##", "######", ".####."}};
static const nzd_glyph_t NZD_I = {6, {"######", "######", "..##..", "..##..", "..##..", "######", "######"}};
static const nzd_glyph_t NZD_Z = {6, {"######", "######", "...###", "..###.", ".###..", "######", "######"}};
static const nzd_glyph_t NZD_D = {6, {"#####.", "######", "##..##", "##..##", "##..##", "######", "#####."}};
static const nzd_glyph_t NZD_E = {6, {"######", "######", "##....", "#####.", "##....", "######", "######"}};
static const nzd_glyph_t NZD_M = {7, {"##...##", "###.###", "#######", "##.#.##", "##...##", "##...##", "##...##"}};
static const nzd_glyph_t NZD_S = {6, {".#####", "######", "###...", ".####.", "...###", "######", "#####."}};

static const nzd_glyph_t *const NZD_LINE1[4] = {&NZD_N, &NZD_O, &NZD_I, &NZD_Z};
static const nzd_glyph_t *const NZD_LINE2[6] = {&NZD_D, &NZD_E, &NZD_M, &NZD_O, &NZD_N, &NZD_S};

#define NZD_L1_CW 8                              /* NOIZ: 27 cells x 8 = 216 px wide, 7 x 9 = 63 tall */
#define NZD_L1_CH 9
#define NZD_L1_Y 14
#define NZD_L2_CW 5                              /* DEMONS: 42 cells x 5 = 210 px wide (the screen's limit), 7 x 8 = 56 tall */
#define NZD_L2_CH 8
#define NZD_L2_Y 88

static int32_t nzd_line_w(const nzd_glyph_t *const *gs, uint32_t n)
{
    int32_t w = 0;
    uint32_t i;
    for (i = 0; i < n; i++)
        w += gs[i]->w + (i ? 1 : 0);
    return w;
}

/* the GLITCH: the title as a corrupted signal. In 30 ms ticks it blinks out, jumps, tears rows sideways, drops
 * blocks, throws stray blocks around and swaps letters between the accent, the ink and a white flash. Strongest
 * for NZD_GLITCH_MS at the start, easing as it goes, then it snaps clean */
#define NZD_GLITCH_MS 900u
#define NZD_TICK_MS 30u
#define NZD_WHITE NZD_RGB(235, 255, 225)
static uint32_t nzd_hash(uint32_t x)
{
    x ^= x >> 16; x *= 0x7feb352du; x ^= x >> 15; x *= 0x846ca68bu; x ^= x >> 16;
    return x;
}
static uint32_t nzd_glitch_amt(uint32_t t)          /* 0 clean .. 100 full */
{
    uint32_t tick = t / NZD_TICK_MS;
    if (t >= NZD_GLITCH_MS || tick % 5u == 2u)       /* (a clean frame flashes through now and then: readable) */
        return 0u;
    return 65u - 45u * t / NZD_GLITCH_MS;            /* strong at first, calming toward the snap */
}
#define NZD_PCT(h, p) ((h) % 100u < (p))
static void nzd_line(const nzd_glyph_t *const *gs, uint32_t n, int32_t y, int32_t cw, int32_t ch, uint32_t t, uint32_t li0,
                     uint16_t ink, uint16_t accent, nzd_rect_fn rect)
{
    int32_t x0 = (240 - nzd_line_w(gs, n) * cw - 3) / 2, x = x0, r, k, dx, ldx = 0, ldy = 0;
    uint32_t i, tick = t / NZD_TICK_MS, amt = nzd_glitch_amt(t), seed = tick * 7919u + li0 * 131u, h;
    uint16_t c;
    if (amt) {
        if ((t < NZD_GLITCH_MS && tick % 4u == 0u) || NZD_PCT(nzd_hash(seed), amt * 22u / 100u))
            return;                                              /* blinks out */
        h = nzd_hash(seed + 1u);
        if (NZD_PCT(h, 45u)) {                                   /* the whole line jumps */
            ldx = ((int32_t)(nzd_hash(seed + 2u) % 41u) - 20) * (int32_t)amt / 100;
            ldy = ((int32_t)(nzd_hash(seed + 3u) % 13u) - 6) * (int32_t)amt / 100;
        }
    }
    for (i = 0; i < n; i++) {
        h = nzd_hash(seed + 100u + i);
        c = !amt ? ink : NZD_PCT(h, 8u) ? NZD_WHITE : NZD_PCT(h >> 8, amt / 2u) ? accent : ink;
        for (r = 0; r < 7; r++) {
            dx = ldx;
            h = nzd_hash(seed + 200u + (uint32_t)r);
            if (amt && NZD_PCT(h, amt * 65u / 100u))             /* a torn row */
                dx += ((int32_t)(nzd_hash(h) % 65u) - 32) * (int32_t)amt / 100;
            for (k = 0; k < gs[i]->w; k++) {
                if (gs[i]->row[r][k] != '#')
                    continue;
                if (amt && NZD_PCT(nzd_hash(seed + 300u + i * 64u + (uint32_t)(r * 8 + k)), amt * 15u / 100u))
                    continue;                                    /* a dropped block */
                rect(x + k * cw + dx + 3, y + r * ch + ldy + 3, cw, ch, NZD_SHADOW);
                rect(x + k * cw + dx, y + r * ch + ldy, cw, ch, c);
            }
        }
        x += (gs[i]->w + 1) * cw;
    }
    for (i = 0; amt && i < amt * 7u / 100u; i++) {               /* stray blocks around the line */
        h = nzd_hash(seed + 900u + i);
        rect(x0 + (int32_t)(h % (uint32_t)(x - x0)), y + (int32_t)((h >> 12) % (uint32_t)(7 * ch)),
             cw * (1 + (int32_t)((h >> 20) % 3u)), ch, (h >> 24) & 1u ? accent : ink);
    }
}

/* ---- the demon, drawn twice (as is, bottom-right; mirrored, bottom-left): 22 x 20 cells, a little Pazuzu facing
 * you: a snarling lion-dog head (short horns, a ruff, bulging eyes, a broad snout, a fanged snarl), the inner arm
 * raised beside the head, the outer arm hanging low with its claw open, four wings (a pair swept up behind the
 * shoulders, a pair swept down), a lean body, bird-taloned feet. No tail.
 * '#' body, 'w' wing, 'e' eye, 'm' mouth, 'f' fang ---- */
#define NZD_IMP_W 22
#define NZD_IMP_H 20
#define NZD_IMP_CELL 4
#define NZD_IMP_X (236 - NZD_IMP_W * NZD_IMP_CELL)
#define NZD_IMP_Y (238 - NZD_IMP_H * NZD_IMP_CELL)
#define NZD_MOUTH NZD_RGB(40, 6, 6)
#define NZD_FANG  NZD_RGB(250, 240, 220)
static const char *const NZD_IMP_SHUT[NZD_IMP_H] = {
    "w....................w",
    "ww.....#......#.....ww",
    "www.#..#.####.#....www",
    ".ww#.#.########...www.",
    "w.w###.#e####e#..www.w",
    "ww.ww#.#ee##ee#.www.ww",
    ".wwww#.###mm###.wwwww.",
    "..www#.#fmmmmf#.wwww..",
    ".w.w.#..######..w.w.w.",
    ".....#...####...w.....",
    ".....############.....",
    "...wwww.######.w#ww...",
    "wwwwwwww.####.ww#wwwww",
    "wwwwww..######..#wwwww",
    "ww.wwww.##..##.###w.ww",
    "w.ww.w..#....#.#w#ww.w",
    ".w..w...#....#...w..w.",
    "...w....#....#....w...",
    ".......###..###.......",
    "......#.#....#.#......",
};
static const char *const NZD_IMP_OPEN[NZD_IMP_H] = {     /* the jaw dropped, fangs top and bottom */
    "w....................w",
    "ww.....#......#.....ww",
    "www.#..#.####.#....www",
    ".ww#.#.########...www.",
    "w.w###.#e####e#..www.w",
    "ww.ww#.#ee##ee#.www.ww",
    ".wwww#.###mm###.wwwww.",
    "..www#.#fmmmmf#.wwww..",
    ".w.w.#.#mmmmmm#.w.w.w.",
    ".....#..#f##f#..w.....",
    ".....############.....",
    "...wwww.######.w#ww...",
    "wwwwwwww.####.ww#wwwww",
    "wwwwww..######..#wwwww",
    "ww.wwww.##..##.###w.ww",
    "w.ww.w..#....#.#w#ww.w",
    ".w..w...#....#...w..w.",
    "...w....#....#....w...",
    ".......###..###.......",
    "......#.#....#.#......",
};

#define NZD_IMP_AT 820u                          /* he rises into the corner .. */
#define NZD_IMP_RISE 200u
#define NZD_BLINK_AT 1080u                       /* .. blinks .. */
#define NZD_BLINK_MS 90u
#define NZD_OPEN_AT 1300u                        /* .. opens wide and puffs fire .. */
#define NZD_OPEN_MS 650u
#define NZD_PUFFS 12u
#define NZD_PUFF_EVERY 38u
#define NZD_PUFF_LIFE 600u
#define NZD_SINK_AT 2440u                       /* .. and when the fire is out, sinks back down */
#define NZD_SINK_MS 220u

static void nzd_imp(uint32_t t, uint16_t body, uint16_t wing, nzd_rect_fn rect)
{
    const char *const *g = t >= NZD_OPEN_AT && t < NZD_OPEN_AT + NZD_OPEN_MS ? NZD_IMP_OPEN : NZD_IMP_SHUT;
    int32_t r, k, shown, cut;
    uint32_t age = t - NZD_IMP_AT;
    char ch;
    int blink = t >= NZD_BLINK_AT && t < NZD_BLINK_AT + NZD_BLINK_MS;
    /* rising: only the rows above the ground line so far, the figure pushed up from below */
    shown = age >= NZD_IMP_RISE ? NZD_IMP_H : (int32_t)(age * NZD_IMP_H / NZD_IMP_RISE);
    if (t >= NZD_SINK_AT)                        /* sinking: the same, in reverse */
        shown = t - NZD_SINK_AT >= NZD_SINK_MS ? 0 : NZD_IMP_H - (int32_t)((t - NZD_SINK_AT) * NZD_IMP_H / NZD_SINK_MS);
    cut = NZD_IMP_H - shown;
    for (r = 0; r < shown; r++)
        for (k = 0; k < NZD_IMP_W; k++) {
            ch = g[r][k];
            if (ch == '.')
                continue;
            rect(NZD_IMP_X + k * NZD_IMP_CELL, NZD_IMP_Y + (r + cut) * NZD_IMP_CELL, NZD_IMP_CELL, NZD_IMP_CELL,
                 ch == 'e' ? (blink ? body : NZD_EYE) : ch == 'w' ? wing : ch == 'm' ? NZD_MOUTH :
                 ch == 'f' ? NZD_FANG : body);
        }
}

/* the fire: puffs leave the mouth one after another, drift up-left toward the title, swell, cool and fade.
 * Positions snap to a 3 px grid (chunky). Direction ~(-0.88, -0.48): clear of his own face; a sway per puff */
static void nzd_fire(uint32_t t, nzd_rect_fn rect)
{
    static const int8_t SWAY[NZD_PUFFS] = {0, 9, -7, 5, -10, 7, -3, 11, -8, 4, -5, 8};
    int32_t mx = NZD_IMP_X + 7 * NZD_IMP_CELL, my = NZD_IMP_Y + 9 * NZD_IMP_CELL;   /* the open mouth's left corner */
    int32_t x, y, d, s, age, born;
    uint32_t i;
    uint16_t c;
    for (i = 0; i < NZD_PUFFS; i++) {
        born = (int32_t)(NZD_OPEN_AT + 60u + i * NZD_PUFF_EVERY);
        age = (int32_t)t - born;
        if (age < 0 || age >= (int32_t)NZD_PUFF_LIFE)
            continue;
        d = age * 95 / (int32_t)NZD_PUFF_LIFE + 6;                         /* distance out, px */
        x = mx - d * 88 / 100 + SWAY[i] * age / (int32_t)NZD_PUFF_LIFE;
        y = my - d * 48 / 100 - SWAY[i] * age / (2 * (int32_t)NZD_PUFF_LIFE);
        s = 9 + age * 18 / (int32_t)NZD_PUFF_LIFE;                         /* swells 9 -> 27 px .. */
        if (age > (int32_t)NZD_PUFF_LIFE * 3 / 4)                          /* .. then shrinks away */
            s = s * ((int32_t)NZD_PUFF_LIFE - age) * 4 / (int32_t)NZD_PUFF_LIFE;
        s = s / 3 * 3;
        if (s < 3)
            continue;
        c = age < 90 ? NZD_FIRE0 : age < 190 ? NZD_FIRE1 : age < 300 ? NZD_FIRE2 : age < 420 ? NZD_FIRE3 : NZD_FIRE4;
        x = x / 3 * 3 - s / 2 / 3 * 3;
        y = y / 3 * 3 - s / 2 / 3 * 3;
        rect(x, y, s, s, c);
        if (s >= 9 && age < 300)                                           /* a hotter core */
            rect(x + s / 3 / 3 * 3, y + s / 3 / 3 * 3, s / 3 / 3 * 3 + 3, s / 3 / 3 * 3 + 3,
                 age < 150 ? NZD_FIRE0 : NZD_FIRE1);
    }
    if (t >= NZD_OPEN_AT && t < NZD_OPEN_AT + NZD_OPEN_MS - 80u)            /* the flame at the lips */
        rect(mx - 3, my - 3, 6, 6, (t / 60u) & 1u ? NZD_FIRE0 : NZD_FIRE1);
}

/* ---- the credit: two small lines where the twins stood, fading in from the background to a dim ink. A tiny
 * 8-row font of its own (row 7: the descender), only the letters it needs; 2 px a cell, to stay readable ---- */
#define NZD_CREDIT_AT 2680u
#define NZD_CREDIT_FADE 260u
#define NZD_CREDIT_PX 2                          /* a font cell, px */
#define NZD_CREDIT_Y1 178                        /* "Felucca mod by" */
#define NZD_CREDIT_Y2 198                        /* "SPIKE the Percussionist" */
typedef struct { char c; uint8_t w; const char *row[8]; } nzd_tiny_t;
static const nzd_tiny_t NZD_TINY[] = {
    {' ', 2, {"..", "..", "..", "..", "..", "..", "..", ".."}},
    {'o', 4, {"....", "....", ".##.", "#..#", "#..#", "#..#", ".##.", "...."}},
    {'n', 4, {"....", "....", "###.", "#..#", "#..#", "#..#", "#..#", "...."}},
    {'e', 4, {"....", "....", ".##.", "#..#", "####", "#...", ".###", "...."}},
    {'l', 3, {"##.", ".#.", ".#.", ".#.", ".#.", ".#.", "###", "..."}},
    {'u', 4, {"....", "....", "#..#", "#..#", "#..#", "#..#", ".###", "...."}},
    {'c', 4, {"....", "....", ".###", "#...", "#...", "#...", ".###", "...."}},
    {'a', 4, {"....", "....", ".##.", "...#", ".###", "#..#", ".###", "...."}},
    {'m', 5, {".....", ".....", "##.#.", "#.#.#", "#.#.#", "#.#.#", "#.#.#", "....."}},
    {'d', 4, {"...#", "...#", ".###", "#..#", "#..#", "#..#", ".###", "...."}},
    {'b', 4, {"#...", "#...", "###.", "#..#", "#..#", "#..#", "###.", "...."}},
    {'y', 4, {"....", "....", "#..#", "#..#", "#..#", ".###", "...#", ".##."}},
    {'t', 4, {".#..", ".#..", "####", ".#..", ".#..", ".#..", "..##", "...."}},
    {'h', 4, {"#...", "#...", "###.", "#..#", "#..#", "#..#", "#..#", "...."}},
    {'r', 4, {"....", "....", "#.##", "##..", "#...", "#...", "#...", "...."}},
    {'s', 4, {"....", "....", ".###", "#...", ".##.", "...#", "###.", "...."}},
    {'i', 1, {"#", ".", "#", "#", "#", "#", "#", "."}},
    {'F', 4, {"####", "#...", "#...", "###.", "#...", "#...", "#...", "...."}},
    {'S', 4, {".###", "#...", "#...", ".##.", "...#", "...#", "###.", "...."}},
    {'P', 4, {"###.", "#..#", "#..#", "###.", "#...", "#...", "#...", "...."}},
    {'I', 3, {"###", ".#.", ".#.", ".#.", ".#.", ".#.", "###", "..."}},
    {'K', 4, {"#..#", "#.#.", "##..", "#...", "##..", "#.#.", "#..#", "...."}},
    {'E', 4, {"####", "#...", "#...", "###.", "#...", "#...", "####", "...."}},
};
static const nzd_tiny_t *nzd_tiny(char c)
{
    uint32_t i;
    for (i = 0; i < sizeof NZD_TINY / sizeof NZD_TINY[0]; i++)
        if (NZD_TINY[i].c == c)
            return &NZD_TINY[i];
    return &NZD_TINY[0];
}
static void nzd_text(const char *s, int32_t y, uint16_t c, nzd_rect_fn rect)
{
    const char *p;
    int32_t w = -1, x, r, k;
    for (p = s; *p; p++)
        w += nzd_tiny(*p)->w + 1;
    x = (240 - w * NZD_CREDIT_PX) / 2;
    for (p = s; *p; p++) {
        const nzd_tiny_t *g = nzd_tiny(*p);
        for (r = 0; r < 8; r++)
            for (k = 0; k < g->w; k++)
                if (g->row[r][k] == '#')
                    rect(x + k * NZD_CREDIT_PX, y + r * NZD_CREDIT_PX, NZD_CREDIT_PX, NZD_CREDIT_PX, c);
        x += (g->w + 1) * NZD_CREDIT_PX;
    }
}
/* a .. b by pct (RGB565) */
static uint16_t nzd_mix(uint16_t a, uint16_t b, uint32_t pct)
{
    int32_t ar = a >> 11, ag = (a >> 5) & 63, ab = a & 31, br = b >> 11, bg = (b >> 5) & 63, bb = b & 31, q = (int32_t)pct;
    return (uint16_t)(((ar + (br - ar) * q / 100) << 11) | ((ag + (bg - ag) * q / 100) << 5) | (ab + (bb - ab) * q / 100));
}
static void nzd_credit(uint32_t t, nzd_rect_fn rect, uint16_t bg, uint16_t ink)
{
    uint32_t pct;
    uint16_t c;
    if (t < NZD_CREDIT_AT)
        return;
    pct = t - NZD_CREDIT_AT >= NZD_CREDIT_FADE ? 100u : (t - NZD_CREDIT_AT) * 100u / NZD_CREDIT_FADE;
    c = nzd_mix(bg, nzd_mix(bg, ink, 60u), pct);  /* a dim ink: a quiet credit, not a second title */
    nzd_text("Felucca mod by", NZD_CREDIT_Y1, c, rect);
    nzd_text("SPIKE the Percussionist", NZD_CREDIT_Y2, c, rect);
}

static nzd_rect_fn nzd_mirror_to;
static void nzd_rect_mirror(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t c) { nzd_mirror_to(240 - x - w, y, w, h, c); }

/* the whole frame at t ms. wing: the imp's wing shade (a darker accent works) */
static void nzd_draw(uint32_t t, nzd_rect_fn rect, uint16_t bg, uint16_t ink, uint16_t accent, uint16_t wing)
{
    t = t * NZD_SPEED / 100u;                    /* (every timing below is at 100 %) */
    rect(0, 0, 240, 240, bg);
    nzd_line(NZD_LINE1, 4, NZD_L1_Y, NZD_L1_CW, NZD_L1_CH, t, 0u, ink, accent, rect);
    nzd_line(NZD_LINE2, 6, NZD_L2_Y, NZD_L2_CW, NZD_L2_CH, t, 4u, ink, accent, rect);
    if (t >= NZD_IMP_AT)
        nzd_imp(t, accent, wing, rect);
    nzd_fire(t, rect);
    nzd_mirror_to = rect;                        /* his twin: the same, mirrored into the bottom-left */
    if (t >= NZD_IMP_AT)
        nzd_imp(t, accent, wing, nzd_rect_mirror);
    nzd_fire(t, nzd_rect_mirror);
    nzd_credit(t, rect, bg, ink);
}

/* ---- on the screen: the canvas holds 240 x 124 at most (CV_MAX), so a frame is two 120-row strips, each drawn
 * whole into the canvas and blitted (no flicker). Every rect is clipped to the strip here ---- */
static int32_t nzd_strip_y;
static void nzd_rect_cv(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t c)
{
    int32_t y0 = nzd_strip_y, y1 = nzd_strip_y + 120;
    if (x < 0) { w += x; x = 0; }
    if (x + w > 240) w = 240 - x;
    if (y < y0) { h -= y0 - y; y = y0; }
    if (y + h > y1) h = y1 - y;
    if (w <= 0 || h <= 0)
        return;
    cv_rect(x, y - y0, w, h, c);
}

static uint8_t nzd_state;                        /* 0 not begun, 1 playing, 2 over */
static uint32_t nzd_t0;                          /* fm1_ms of its first frame */
static uint32_t nzd_held;                        /* buttons | keys held last frame (a NEW press skips it) */

/* ui_draw, every frame: 1 = the splash drew this frame (draw no page) */
static int nzd_frame(void)
{
    uint32_t t, now, y;
    if (nzd_state == 2u)
        return 0;
    now = fm1_in.buttons | fm1_in.notes;
    if (!nzd_state) {
        nzd_state = 1;
        nzd_t0 = fm1_ms;
        nzd_held = now;                          /* (anything held from power-on does not count as a press) */
    }
    t = fm1_ms - nzd_t0;
    if (t >= NZD_MS || ui.uboot || (now & ~nzd_held)) {
        nzd_state = 2;
        ui.force = 1;                            /* the whole page from scratch */
        return 0;
    }
    nzd_held = now;
    for (y = 0; y < 240u; y += 120u) {
        nzd_strip_y = (int32_t)y;
        cv_oy = 0;
        cv_begin(240, 120, T_BG);
        nzd_draw(t, nzd_rect_cv, T_BG, T_THEME, T_ACCENT,     /* the wing: half accent, half background */
                 (uint16_t)(((T_BG >> 1) & 0x7BEFu) + ((T_ACCENT >> 1) & 0x7BEFu)));
        cv_blit(0, y);
    }
    return 1;
}
