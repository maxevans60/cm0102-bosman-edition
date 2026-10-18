/* Test of the cup round types and the cup table (0.6.0 slice 21, src/recomp_rt/win/season_cups.c).
 * The first checks use rounds as the original makes them (docs/research/2026-10-17-cup-round-record.md, section 4;
 * values from create_round_table of EngFaCup 0x56f320, EngCcCup 0x56c570, SpaCup 0x84d220): a type must leave a
 * round that already is that type as it is, so a row never changes what the base game does by mistake. */
#include "season_cups.h"

#include <stdio.h>
#include <string.h>

static int failures;

#define CHECK(cond, ...) do { if (!(cond)) { failures++; printf("FAIL line %d: ", __LINE__); printf(__VA_ARGS__); printf("\n"); } } while (0)

static void same(const char *what, char type, SeasonCupRound r)
{
    SeasonCupRound before = r;
    int changed = season_cups_apply(type, &r);
    CHECK(!changed && r.first == before.first && r.deciding == before.deciding && r.replays == before.replays &&
              r.legs == before.legs && r.days == before.days,
          "%s: type %c changed the round of the original (first 0x%x deciding 0x%x replays %d legs %d days %d)", what,
          type, r.first, r.deciding, r.replays, r.legs, r.days);
}

int main(void)
{
    /* EngFaCup: round 1 (first round proper) replays once, the replay has extra time and penalties; the semi-final
     * and the final are settled on the day. */
    same("FA Cup round 1", 'R', (SeasonCupRound){0, 3, 1, 1, 10});
    same("FA Cup semi-final", 'E', (SeasonCupRound){3, 0, 0, 1, 10});
    /* EngCcCup: the early rounds hold the seeded draw bit 0x400 and 0x200 on top of extra time and penalties; the
     * semi-final is over two legs with the League Cup away goals rule. */
    same("League Cup round 1", 'E', (SeasonCupRound){0x403, 3, 0, 1, 0});
    same("League Cup quarter-final", 'E', (SeasonCupRound){0x203, 0, 0, 1, 0});
    same("League Cup semi-final", 'H', (SeasonCupRound){0x4200, 0x4003, 0, 2, 21});
    /* SpaCup: rounds over two legs with away goals. */
    same("Copa del Rey round 4", 'L', (SeasonCupRound){0, 3, 0, 2, 7});

    /* A change keeps the draw bits. */
    SeasonCupRound r = {0x403, 3, 0, 1, 0};
    CHECK(season_cups_apply('P', &r) && r.first == 0x401 && r.legs == 1 && r.replays == 0, "P on 0x403 gave 0x%x", r.first);
    r = (SeasonCupRound){0x4200, 0x4003, 0, 2, 21};
    CHECK(season_cups_apply('N', &r) && r.first == 0x200 && r.deciding == 7 && r.legs == 2, "N on the League Cup semi-final");
    r = (SeasonCupRound){0, 3, 1, 1, 10};
    CHECK(season_cups_apply('E', &r) && r.first == 3 && r.replays == 0, "E on a replay round");
    r = (SeasonCupRound){3, 0, 0, 1, 10};
    CHECK(season_cups_apply('L', &r) && r.legs == 2 && r.days == 7 && r.first == 0 && r.deciding == 3, "L on a single match");
    r = (SeasonCupRound){0, 3, 0, 2, 14};
    CHECK(season_cups_apply('E', &r) && r.legs == 1 && r.first == 3 && r.days == 14, "E on a two-leg round");
    r = (SeasonCupRound){0x4200, 0x4003, 0, 2, 21};
    CHECK(season_cups_apply('Q', &r) && r.first == 0x200 && r.deciding == 5 && r.legs == 2, "Q on the League Cup semi-final");
    r = (SeasonCupRound){3, 0, 0, 1, 7};
    CHECK(season_cups_apply('S', &r) && r.deciding == 1 && r.replays == 1 && r.first == 0 && r.days == 10, "S on a single match");
    r = (SeasonCupRound){3, 0, 0, 1, 7};
    CHECK(season_cups_apply('R', &r) && r.days == 10 && r.deciding == 3, "R sets the replay days");
    CHECK(!season_cups_apply('.', &r) && !season_cups_apply('x', &r), "a letter that is no type changed a round");

    /* The table: the latest row not later than the season. */
    CHECK(season_cup_row_count > 10, "the table holds %d rows", season_cup_row_count);
    CHECK(!season_cups_row("eng_fa_cup", 2015), "FA Cup 2015 has a row");
    const SeasonCupRow *row = season_cups_row("eng_fa_cup", 2019);
    CHECK(row && row->season == 2018 && row->rounds == 9 && !strcmp(row->types, "RRRRREEEE"), "FA Cup 2019 row");
    row = season_cups_row("eng_fa_cup", 2099);
    CHECK(row && row->season == 2024 && !strcmp(row->types, "REEEEEEEE"), "FA Cup 2099 row (the rule stays)");
    CHECK(!season_cups_row("eng_no_such_cup", 2020), "a cup that is not in the table has a row");
    for (int i = 0; i < season_cup_row_count; i++) {
        const SeasonCupRow *x = &season_cup_rows[i];
        CHECK(x->season >= 2002 && (int)strlen(x->types) == x->rounds, "row %d (%s %d) is bad", i, x->file, x->season);
    }
    if (failures) {
        printf("%d failures\n", failures);
        return 1;
    }
    printf("season cups: all checks pass\n");
    return 0;
}
