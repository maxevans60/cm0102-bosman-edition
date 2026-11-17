// The service worker (web/README.md, "As an app"): lets the page start without a connection once it has run.
//
// With a connection it changes nothing: every request goes to the server first, so a new version of the page or the
// engine is used at once. Only when the server cannot be reached (or does not answer in time) the answer comes from
// the cache that the page filled after its last good start (loader.js, primeOffline). The game files are not its
// business: the page keeps those in its own cache and asks for them there first, and the account (/api, /auth) is
// always the server's.
const SHELL = 'cm0102-shell';
const WAIT_MS = 6000;          // a server that does not answer this long, with a kept copy at hand: the copy

self.addEventListener('install', () => self.skipWaiting());
self.addEventListener('activate', (e) => e.waitUntil(self.clients.claim()));

async function kept(request) {
  const cache = await caches.open(SHELL);
  const url = new URL(request.url);
  return (await cache.match(request)) ?? (await cache.match(url.pathname + url.search)) ?? (url.pathname === '/' ? cache.match('/') : undefined);
}

// The engine under its version never changes: the kept copy first.
async function keptFirst(request) {
  return (await kept(request)) ?? fetch(request);
}

// The kept copy is one version of the page: a page that came from it gets its scripts and files.json from it too, not
// from the server. An iPhone opened the page while the stopped Fly machine woke (17 November 2026): the page came from
// the copy after 6 s, then loader.js from the server, by then up with a newer version, and that loader stopped on an
// element the old page did not have. onCopy: called when the answer is the copy.
const fromCopy = new Set();          // the clients (tabs) whose page came from the kept copy

async function serverFirst(request, onCopy = () => {}) {
  const copy = await kept(request);
  try {
    const answer = fetch(request);
    if (!copy) return await answer;
    const timeout = new Promise((resolve) => setTimeout(() => resolve(null), WAIT_MS));
    const got = await Promise.race([answer, timeout]);
    if (got) return got;
    onCopy();
    return copy;
  } catch (err) {
    if (copy) { onCopy(); return copy; }
    throw err;
  }
}

self.addEventListener('fetch', (e) => {
  const url = new URL(e.request.url);
  if (e.request.method !== 'GET' || url.origin !== self.location.origin || /^\/(game|api|auth)\//.test(url.pathname)) return;
  if (e.request.mode === 'navigate') {
    const tab = e.resultingClientId;
    if (tab) fromCopy.delete(tab);
    e.respondWith(serverFirst(e.request, () => { if (tab) fromCopy.add(tab); }));
    return;
  }
  const engine = url.pathname.startsWith('/engine/') && url.searchParams.has('v');
  e.respondWith(engine || fromCopy.has(e.clientId) ? keptFirst(e.request) : serverFirst(e.request));
});
