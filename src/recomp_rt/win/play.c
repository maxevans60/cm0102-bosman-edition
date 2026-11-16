/* Play (web/README.md, "Play"): the Play button of a save in the page's Saved games loads that save. The runtime does
 * what a player does at the start screen: Restore Saved Game, the save's name typed in the File entry, Ok. The page
 * asks through rt_web_play; a run without a page sets RECOMP_PLAY (tests/smoke/play_run.py).
 *
 * The start screen (build 0x81bab0) opens the dialog for its button of action id 3, Restore Saved Game (0x81ca4f):
 * open_file_dialog 0x58f500 (0x81ca55..0x81caa6, then the call at 0x81ca02) with the title "Restore Saved Game"
 * 0xa80244, the extension ".sav" 0xa5aab8, game options + 0x113 (the name of the save loaded or saved last,
 * savename.c) as the File entry's first text, and the check check_saved_game_file 0x838e10. The dialog (build
 * 0x58f7c0, input 0x58ff80, pushed at 0x58f537..0x58f546) opens with its File entry taking keys. A probe of its
 * widgets (16 November 2026): the entry takes 255 characters; the list rows have action id 2, Cancel 3, Ok 4, Info 5.
 * Its input runs Ok for the button of id 4 (0x590724) and for Enter (0x59026e). Ok adds ".sav" to the name when it
 * has another extension (0x590849..0x59087e), and with the entry empty it shows "The file name entered is blank".
 * The entry refuses some characters and any that would make its text wider than the box
 * (docs/research/2026-10-13-input-inventory.md, the text entry), and a click outside it while it takes keys first ends
 * the typing (the same, "click inside"): the click on Ok may run Ok at once or only end the typing (a run of 16
 * November 2026 needed a second click).
 *
 * So at the start screen the runtime clicks the button of id 3. On the dialog it types the name, without ".sav", one
 * byte per input poll (see win_play); it reads the entry's text back (keys.c) and deletes a wrong character, or the
 * text the entry opened with, with Backspace. When the entry holds exactly the name it clicks Ok, and again, after
 * that click ended and two input polls ran, while the dialog stays with the entry no longer taking keys. A click goes
 * as the click of a key does (user32.c: down, an input poll, up), at the button's place in the game's own widget list
 * (keys.c), so it works at every picture size and in every language of the game; never while a wait for a held button
 * runs (user32.c, in_wait). The play ends when the dialog is gone: the game then loads the save as for a player's Ok,
 * with its own messages when the file does not load. Nothing changes unless a play is asked for.
 *
 * The page hears how it went through Module.rtPlay(code), one of PLAY_* below; a request it refuses at once is the
 * return value of rt_web_play. */
#include "win.h"
#include "cp1252.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define START_BUILD 0x81bab0u
#define DIALOG_BUILD 0x58f7c0u
#define RESTORE_ID 3                /* Restore Saved Game on the start screen (0x81ca4f) */
#define OK_ID 4                     /* Ok of the file dialog (0x590724) */
#define GAME_MADE 0xdd711cu         /* 1 once a game is made or loaded (waits.c) */
#define NAME_MAX 200                /* bytes of a name: the File entry takes 255, Save Game's name buffer is 200 */
#define START_WAIT_MS 300000u       /* the intro and the settings box may come first, at the player's pace */
#define STEP_WAIT_MS 30000u         /* from the last step that went forward */
#define TYPE_GAP_MS 40u             /* as the page's keyboard (user32.c, WEB_TYPE_GAP_MS) */
#define TYPE_TRIES 4                /* the same key this often without the entry taking it: it does not fit */
#define OK_TRIES 3

/* The outcomes the page hears (web/play.js); -1: the play goes on. */
enum { PLAY_OK, PLAY_LETTERS, PLAY_LONG, PLAY_IN_GAME, PLAY_NO_START, PLAY_NO_DIALOG, PLAY_NO_TYPING, PLAY_CLOSED,
       PLAY_BUSY };

static char wanted[NAME_MAX + 1];   /* the file name's bytes, without ".sav" */
static char shown[256];             /* the name as asked, for the log */
static enum { IDLE, AT_START, IN_DIALOG, TYPED } stage;
static uint64_t since, typed_at;
static uint32_t typed_poll, released_poll;
static size_t typed_len;            /* the entry's length when the last key went */
static int tries, clicks, seen, released;

static void end(int code, const char *what)
{
    stage = IDLE;
    fprintf(stderr, "recomp: play: %s: %s\n", shown, what);
#ifdef __EMSCRIPTEN__
    EM_ASM({ try { if (typeof Module.rtPlay === 'function') Module.rtPlay($0); } catch (e) { console.error(e); } }, code);
#else
    (void)code;
#endif
}

/* A request that is refused at once: the play in progress, if any, goes on. */
static int refuse(const char *name, int code, const char *what)
{
    fprintf(stderr, "recomp: play: %.200s: %s\n", name, what);
    return code;
}

int win_play(const char *utf8)
{
    const char *name = utf8 ? utf8 : "";
    if (stage != IDLE) return refuse(name, PLAY_BUSY, "another save is opening");
    size_t n = strlen(name);
    if (n > 4 && !SDL_strcasecmp(name + n - 4, ".sav")) n -= 4;
    if (n > NAME_MAX) return refuse(name, PLAY_LONG, "the name is too long for the game's File entry");
    char base[NAME_MAX + 1];
    memcpy(base, name, n);
    base[n] = 0;
    /* The game's list of saves holds each file name as the host gives it, in UTF-8, one character per byte (a probe of
     * "Köln 2001.sav", 16 November 2026: the row read "KÃ¶ln 2001"), and a click on the row puts those bytes in the
     * File entry; the path code passes a valid UTF-8 sequence through as it is (cp1252.h, cp1252_to_utf8). The runtime
     * types the same bytes, so a name in any script opens as the row would open it. The entry refuses control
     * characters and " * / < > | { } [ ] ~, and : \ ? in this entry (its flags have neither 0x8000 nor 0x10000). */
    int bad = !base[0] || !utf8_ok(base);
    for (const unsigned char *p = (const unsigned char *)base; *p; p++)
        bad |= *p < 0x20 || *p == 0x7f || strchr("\"*/<>|{}[]~:\\?", *p) != NULL;
    if (bad) return refuse(name, PLAY_LETTERS, "the name has characters the game's File entry does not take");
    if (rd32(g_mem, GAME_MADE)) return refuse(name, PLAY_IN_GAME, "a game is in play");
    snprintf(shown, sizeof shown, "%s", name);
    memcpy(wanted, base, n + 1);
    stage = AT_START;
    since = SDL_GetTicks64();
    fprintf(stderr, "recomp: play: %s: waiting for the start screen\n", shown);
    return -1;
}

/* On the dialog: one key towards an entry that holds the name; 1 once it does. */
static int type_name(uint64_t now)
{
    const char *text = keys_typing_text();
    if (!text) return 0;
    if (!strcmp(text, wanted)) return 1;
    if (waits_poll_count() == typed_poll || now - typed_at < TYPE_GAP_MS) return 0;   /* the last key is not in yet */
    size_t have = strlen(text);
    int adding = have < strlen(wanted) && !strncmp(text, wanted, have);
    if (typed_len != have) { tries = 0; since = now; }  /* the entry took the last key: a step forward */
    else if (adding && ++tries >= TYPE_TRIES) {
        end(PLAY_NO_TYPING, "the File entry does not take the name (too wide for it)");
        return 0;
    }
    typed_len = have;
    win_char(adding ? (uint8_t)wanted[have] : 0x08);
    typed_poll = waits_poll_count();
    typed_at = now;
    return 0;
}

void win_play_poll(void)
{
    static int env_read;
    if (!env_read) {
        env_read = 1;
        const char *e = getenv("RECOMP_PLAY");
        if (e && *e) {
            char name[512];
            snprintf(name, sizeof name, "%s", e);
            unsetenv("RECOMP_PLAY");                     /* not again after Restart Game (host/posix.c keeps the env) */
            win_play(name);
        }
    }
    if (stage == IDLE) return;
    uint64_t now = SDL_GetTicks64();
    int x, y;
    if (stage == AT_START) {
        if (rd32(g_mem, GAME_MADE)) { end(PLAY_IN_GAME, "a game was made or loaded first"); return; }
        if (now - since > START_WAIT_MS) { end(PLAY_NO_START, "the start screen did not come"); return; }
        if (!waits_screen_up(START_BUILD) || waits_in_box() || win_click_busy() || !keys_button(RESTORE_ID, &x, &y) ||
            !win_click(x, y))
            return;
        fprintf(stderr, "recomp: play: %s: Restore Saved Game\n", shown);
        stage = IN_DIALOG;
        since = now;
        typed_poll = waits_poll_count();
        typed_at = 0;
        typed_len = (size_t)-1;
        tries = clicks = seen = released = 0;
        return;
    }
    if (rd32(g_mem, GAME_MADE)) { end(PLAY_OK, "Ok"); return; }     /* the save is loaded */
    if (seen && waits_top_build() != DIALOG_BUILD && !waits_in_box()) {     /* the dialog is gone */
        if (stage == TYPED) end(PLAY_OK, "Ok");
        else end(PLAY_CLOSED, "Restore Saved Game was closed");
        return;
    }
    if (now - since > STEP_WAIT_MS) {
        if (!seen) end(PLAY_NO_DIALOG, "Restore Saved Game did not open");
        else if (stage == TYPED) end(PLAY_NO_DIALOG, "Ok did not load it");
        else end(PLAY_NO_TYPING, "the File entry did not take the name");
        return;
    }
    if (!waits_screen_up(DIALOG_BUILD) || waits_in_box()) return;
    seen = 1;
    if (stage == IN_DIALOG) {
        if (!type_name(now) || stage == IDLE || win_click_busy() || !keys_button(OK_ID, &x, &y) || !win_click(x, y)) return;
        stage = TYPED;                                  /* this click may only end the typing */
        clicks = 1;
        released = 0;
        since = now;
        return;
    }
    /* TYPED: the dialog stays. Once the last click has ended and two input polls ran with the entry no longer taking
     * keys, Ok again. */
    if (win_click_busy()) { released = 0; return; }
    if (!released) { released = 1; released_poll = waits_poll_count(); return; }
    if (keys_typing_text() || waits_poll_count() - released_poll < 2 || clicks >= OK_TRIES) return;
    if (!keys_button(OK_ID, &x, &y) || !win_click(x, y)) return;
    clicks++;
    released = 0;
    fprintf(stderr, "recomp: play: %s: Ok again\n", shown);
}

#ifdef __EMSCRIPTEN__
/* The page's Play button (web/play.js): the file name of the save, in UTF-8. -1: on its way (Module.rtPlay tells the
 * end); else the PLAY_* code of a request refused at once. */
EMSCRIPTEN_KEEPALIVE int rt_web_play(const char *utf8) { return win_play(utf8); }

/* 1 once a game is made or loaded: the page then restarts the game before a play. */
EMSCRIPTEN_KEEPALIVE int rt_web_in_game(void) { return rd32(g_mem, GAME_MADE) != 0; }
#endif
