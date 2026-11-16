// A new version (web/README.md, "A new version"): the server names its release (/version.json: the page files and the
// engines it serves), and a page that runs an older one says so, with a button in the bar that loads the new one the
// way Restart Game does (the saves go to the browser's storage first). It never loads the page by itself: a player in
// the middle of a match or a transfer decides when. The page asks every RELEASE_EVERY_MS and when it comes back into
// view (a phone that slept, a tab left for the night).
export const RELEASE_EVERY_MS = 10 * 60000;

const fromServer = async () => {
  const r = await fetch('/version.json', { cache: 'no-store' });
  return r.ok ? r.json() : null;
};

// current: the release this page started with (files.json); onNew(release): once, when the server has another one.
export function watchRelease({ current, onNew, get = fromServer, everyMs = RELEASE_EVERY_MS }) {
  let told = false;
  async function check() {
    if (told || !current) return;
    try {
      const v = await get();
      if (v?.release && v.release !== current && !told) { told = true; onNew(v.release); }
    } catch { /* no connection now: the next time */ }
  }
  const timer = setInterval(check, everyMs);
  const onVisible = () => { if (!document.hidden) check(); };
  document.addEventListener('visibilitychange', onVisible);
  return { check, stop() { clearInterval(timer); document.removeEventListener('visibilitychange', onVisible); } };
}
