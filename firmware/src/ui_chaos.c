/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments
 * CHAOS beyond the operators (noiz_magick): with CHAOS on for the selected track (ui_fm6op.c fop_chaos; hold EDIT,
 * the CHS key: ui_layer.c, every engine), a knob on an ENV, LFO or FX page rolls that whole area instead of moving
 * one value: every parameter of every track page of the family, each uniformly random over its whole range (the
 * engine's own descriptor: track_desc), no weighting, no guardrails. Every detent is a new roll.
 *   ENV  ENV (ATK DEC SUS REL), ENV DEST (FLT PIT SHP, ESYNC). FM6 ignores these (its operator envelopes are the
 *        sound's: engine_t.ownenv): on FM6 the ENV roll changes nothing you hear.
 *   LFO  LFO (RATE WAVE PHASE FADE), LFO 2 (SYNC TRIG POL), LFO DEST (PIT FLT SHP AMP). Not the MOD page: its knobs
 *        edit as always.
 *   FX   FX (DIST CHOR DLY REV sends), INSERT, INSERT 2: the track's own. Not SLICER: an undo never brings its
 *        values back (ui.c param_kept, as LEVEL PAN), so a roll there could not be undone; its knobs edit as always.
 *        DLY, REVERB, CHORUS are shared by every track (song.g): on one of those pages the knobs roll that page
 *        only, and SAVE held does not undo it.
 * A gesture on the track's pages is one undo (SAVE held: the sound from before its first roll). The rolled values
 * are the track's like any edit: saved with presets and projects; recorded as automation while armed and playing.
 * Included by ui_graph.c after ui_fm6op.c. */

/* CHAOS on for the selected track */
static uint32_t chaos_on(void) { return (fop_chaos >> (song.sel % NTRK)) & 1u; }

/* the area page pg rolls: its family (FAM_ENV, FAM_LFO, FAM_FX), FAM_COUNT none */
static uint32_t chaos_area(const page_t *pg)
{
    if (pg->graph == GR_MOD || pg->graph == GR_SLCR)
        return FAM_COUNT;
    return pg->fam == FAM_ENV || pg->fam == FAM_LFO || pg->fam == FAM_FX ? pg->fam : FAM_COUNT;
}

/* *vp = a random value of d's range */
static void chaos_value(int16_t *vp, const param_desc_t *d)
{
    int32_t span;
    if (!d)
        return;
    span = (int32_t)d->max - (int32_t)d->min + 1;
    if (span > 1)
        *vp = (int16_t)(d->min + (int32_t)(rng() % (uint32_t)span));
}

/* area fam rolled on the selected track. shown: the page shown (a shared page: that page only), 0 none.
 * undo 1: the gesture's one undo copy taken here */
static void chaos_roll_area(uint32_t fam, const page_t *shown, int undo)
{
    track_t *t = TSEL;
    uint32_t i, s, id;
    chaos_seed();
    if (shown && shown->scope == SC_GLOBAL) {
        for (s = 0; s < 4u; s++)
            if ((id = shown->id[s]) != 0xFFu)
                chaos_value(&song.g[id], &GP[id]);
        return;
    }
    if (undo)
        edit_undo_take(t, 0x3000u | fam, UNDO_SOUND);    /* (one copy per gesture: same tag within 1.5 s) */
    for (i = 0; i < NPAGES; i++) {
        const page_t *pg = &PAGES[i];
        if (pg->fam != fam || pg->scope != SC_TRACK || chaos_area(pg) == FAM_COUNT)
            continue;
        for (s = 0; s < 4u; s++) {
            if ((id = pg->id[s]) == 0xFFu)
                continue;
            chaos_value(&t->p[id], track_desc(t, id));
            motion_capture(t, id, t->p[id]);             /* (the new base; automation while recording) */
        }
    }
    if (undo)
        motion_undo_done(t);
}

/* RLL: the area of the page shown; an FM6 operator page: the patch; any other page: everything the track has
 * (the FM6 patch, ENV, LFO, FX: one undo) */
static const char *chaos_roll_here(void)
{
    const page_t *pg = cur_page();
    uint32_t a = ui.home || ui.menu ? FAM_COUNT : chaos_area(pg);   /* (HOME, MENU: over the page, not on it) */
    if (a != FAM_COUNT) {
        chaos_roll_area(a, pg, 1);
        return pg->title;
    }
    if (!ui.home && !ui.menu && fop_page(pg->graph) && fop_ok()) {
        fop_roll();
        return "OPERATORS";
    }
    if (fop_ok())
        fop_roll();                                      /* (takes the undo copy: the sound as it was) */
    else
        edit_undo_take(TSEL, 0, UNDO_SOUND);
    chaos_roll_area(FAM_ENV, 0, 0);
    chaos_roll_area(FAM_LFO, 0, 0);
    chaos_roll_area(FAM_FX, 0, 0);
    motion_undo_done(TSEL);
    return "ALL";
}
