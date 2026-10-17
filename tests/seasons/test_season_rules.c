/* Test of the rules table reader (0.6.0 slice 12, src/recomp_rt/win/season_rules.c).
 *
 * Usage: season_rules_test <rules.tsv made by tools/seasons/make_rules.py> <scratch folder>
 * ctest makes the table first (test season_rules_table). The checks: the row count, the values of known
 * rows, the lookup by season and file (also for a row with two files), and that a damaged or missing file
 * gives an empty table. */
#include "season_rules.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures;

#define CHECK(cond, ...) do { if (!(cond)) { failures++; fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } } while (0)

static void write_file(const char *path, const char *text)
{
    FILE *f = fopen(path, "wb");
    if (!f) { perror(path); exit(2); }
    fputs(text, f);
    fclose(f);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: season_rules_test <rules.tsv> <scratch folder>\n");
        return 2;
    }
    int n = season_rules_load(argv[1]);
    CHECK(n == 7101, "row count %d, want 7101", n);
    CHECK(season_rules_count() == n, "count");

    const SeasonRule *r = season_rules_find(2001, "eng_prm", 0);
    CHECK(r && !strcmp(r->id, "eng-28"), "2001 eng_prm is %s", r ? r->id : "none");
    if (r) {
        CHECK(r->season == 2001 && r->tier == 1 && !strcmp(r->tier_code, "1"), "eng-28 season and tier");
        CHECK(r->clubs == 20 && r->up == SEASON_RULE_NA && r->down == 3, "eng-28 clubs %d up %d down %d",
              r->clubs, r->up, r->down);
        CHECK(r->subs_used == 3 && r->bench == 5, "eng-28 subs %d bench %d", r->subs_used, r->bench);
        CHECK(!strcmp(r->name, "FA Premier League") && !strcmp(r->nation, "England"), "eng-28 name");
        CHECK(!strcmp(r->clubs_text, "20") && *r->sources, "eng-28 texts");
    }
    CHECK(!season_rules_find(2001, "eng_prm", 1), "one eng_prm row in 2001");
    r = season_rules_find(2027, "eng_prm", 0);
    CHECK(r && r->season == 2027, "a row of 2027/28");

    /* A row with two files is found by each of them. */
    int both = 0;
    for (int k = 0; (r = season_rules_find(2001, "wc_oceania_league", k)); k++)
        if (strchr(r->comp_file, ';')) both++;
    CHECK(both == 1, "rows of 2001 with wc_oceania_league and another file: %d", both);

    /* Codes. */
    r = season_rules_find(2001, "uefa_cup", 0);
    CHECK(r && r->clubs == 145 && r->tier == 2, "uefa_cup 2001");
    int not_held = 0, text = 0;
    for (int i = 0; i < season_rules_count(); i++) {
        const SeasonRule *x = season_rules_row(i);
        not_held += x->clubs == SEASON_RULE_NOT_HELD;
        text += x->clubs == SEASON_RULE_TEXT;
    }
    CHECK(not_held > 0 && text > 0, "codes not_held %d text %d", not_held, text);
    CHECK(!season_rules_row(-1) && !season_rules_row(n), "rows out of range");

    /* A damaged file and a missing file give an empty table. */
    char path[1024];
    snprintf(path, sizeof path, "%s/damaged.tsv", argv[2]);
    write_file(path, "#bosman-rules\t1\t1\nid\tseason\n");
    CHECK(season_rules_load(path) == 0 && season_rules_count() == 0, "a damaged header");
    write_file(path, "#bosman-rules\t2\t0\n");
    CHECK(season_rules_load(path) == 0, "a later format");
    snprintf(path, sizeof path, "%s/missing.tsv", argv[2]);
    remove(path);
    CHECK(season_rules_load(path) == 0 && !season_rules_row(0), "a missing file");

    if (failures) fprintf(stderr, "%d failures\n", failures);
    else printf("season_rules_test: all checks passed\n");
    return failures ? 1 : 0;
}
