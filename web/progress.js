// The game's long jobs in the bar (web/README.md, "The page around the game"): a new game's "Player histories" or
// "Initialising Leagues" took minutes on a phone, and a player thought the game had stopped (Facebook, November 2026).
// The engine logs "recomp: progress: <stage>" once a stage has run TOLD_AFTER_MS and "recomp: progress: <stage> took
// <n> s" when it ends (display.c), and rt_web_progress gives the share done: 0..100, 101 for a stage with no bar, -1
// when none runs (or it is not named yet). A named stage shows in the bar's status at once: "Player histories 45% · 0:38".
export const TOLD_AFTER_MS = 2000;

const clock = (ms) => { const s = Math.floor(ms / 1000); return `${Math.floor(s / 60)}:${String(s % 60).padStart(2, '0')}`; };
export const progressText = (name, share, ms) => `${name}${share >= 0 && share <= 100 ? ` ${share}%` : ''} · ${clock(ms)}`;

// read(): rt_web_progress; set(text): the bar's status. line(text): each log line; tick(): a few times a second.
export function watchProgress({ read, set, now = () => Date.now() }) {
  let stage = null, shown = false;
  const clear = () => { stage = null; if (shown) { shown = false; set(''); } };
  return {
    line(text) {
      const m = /^recomp: progress: (.+?)( took [\d.]+ s)?$/.exec(text);
      if (!m) return;
      if (m[2]) { if (stage?.name === m[1]) clear(); return; }
      clear();                                         // the last stage's text goes with it
      stage = { name: m[1], at: now() - TOLD_AFTER_MS };   // it started that long before its line
    },
    tick() {
      const share = read();
      if (typeof share !== 'number' || share < 0) { clear(); return; }
      if (!stage) return;
      set(progressText(stage.name, share, now() - stage.at));
      shown = true;
    },
  };
}
