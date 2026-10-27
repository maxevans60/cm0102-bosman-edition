// Word filter for player names and game names (Q13).
// A name fails when one of its words, after the steps below, is a blocked word.
// Whole words only, so real club and place names such as Scunthorpe, Arsenal,
// Cockfosters and Penistone pass. A few words fail anywhere in a word.

/** Blocked as a whole word. Also blocked with an ending from ENDINGS. */
const WORDS = new Set([
  "fuck", "fuk", "fck", "shit", "cunt", "twat", "wanker", "wank", "bollock", "bitch",
  "prick", "pussy", "slut", "whore", "bastard", "nigger", "nigga", "faggot", "fag",
  "retard", "spastic", "spaz", "paki", "kike", "chink", "coon", "tranny",
  "rape", "rapist", "nazi", "hitler", "jizz", "dildo", "porn",
]);

/** Blocked anywhere in a word. Only words that are never part of an ordinary word. */
const ANYWHERE = ["fuck", "nigger", "nigga", "faggot", "wanker"];

const ENDINGS = ["ers", "er", "ing", "ed", "es", "s", "y", "z"];

const LEET: Record<string, string> = {
  "0": "o", "1": "i", "3": "e", "4": "a", "5": "s", "7": "t", "8": "b",
  "@": "a", "$": "s", "!": "i", "|": "i",
};

function collapse(word: string): string {
  return word.replace(/(.)\1+/g, "$1");
}

const COLLAPSED = new Set([...WORDS].map(collapse));

function wordBlocked(word: string): boolean {
  const forms = new Set([word, word.replace(/v/g, "u")]);
  for (const form of forms) {
    for (const f of [form, collapse(form)]) {
      if (WORDS.has(f) || COLLAPSED.has(f)) return true;
      for (const end of ENDINGS) {
        if (f.length > end.length + 2 && f.endsWith(end)) {
          const stem = f.slice(0, -end.length);
          if (WORDS.has(stem) || COLLAPSED.has(collapse(stem))) return true;
        }
      }
      if (ANYWHERE.some((w) => f.includes(w))) return true;
    }
  }
  return false;
}

/** Splits a name into lower-case words, with number-for-letter spellings undone. */
function words(name: string): string[] {
  const plain = name.normalize("NFKD").replace(/[̀-ͯ]/g, "").toLowerCase();
  const out: string[] = [];
  for (const raw of plain.match(/[a-z0-9@$!|]+/g) ?? []) {
    if (!/[a-z]/.test(raw) && !/[@$!|]/.test(raw)) continue; // plain numbers, such as 1874
    out.push([...raw].map((c) => LEET[c] ?? c).join("").replace(/[^a-z]/g, ""));
  }
  return out.filter((w) => w.length > 0);
}

export function nameAllowed(name: string): boolean {
  const ws = words(name);
  if (ws.some(wordBlocked)) return false;
  // Letters with gaps, such as "f u c k" or "F.U.C.K": join runs of single letters.
  let run = "";
  for (const w of [...ws, ""]) {
    if (w.length === 1) {
      run += w;
      continue;
    }
    if (run.length > 1 && wordBlocked(run)) return false;
    run = "";
  }
  return true;
}
