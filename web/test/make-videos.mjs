// Two how-to videos of the page (web/README.md, "How-to videos"): how to edit a saved game (and a data set), and how
// to use the app around the game. A player asked where the editor is (18 November 2026). The page is recorded in
// Chromium against the stand-in engine and small saves and a data set in the game's formats (fakes.mjs), with a pointer
// drawn on it; the game's own screen is not in them (a panel says where the game runs). Each step's words and the box it
// points at go into a log, and videos-compose.mjs makes the film of it with HyperFrames: a title card, the capture in a
// frame with a caption under it for each step and a zoom on what the step points at, an end card.
// NODE_PATH=$(npm root -g) node web/test/make-videos.mjs <folder> [editor|app]
//   → <folder>/editor.mp4 and <folder>/app.mp4 (and <folder>/<name>-project/, the HyperFrames project, to edit or
//   render again: node .claude/skills/hyperframes/scripts/plugin-cli.mjs render, in it)
// CM_SELLER and CM_COFFEE_URL, as on the server, name the footer's maker and the coffee link. CM_VIDEO_MUSIC (or
// CM_VIDEO_MUSIC_APP, CM_VIDEO_MUSIC_EDITOR) names a music file for the films (videos-compose.mjs; not in git).
import { createRequire } from 'node:module';
import { execFileSync } from 'node:child_process';
import { mkdirSync, renameSync, rmSync, writeFileSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { fakeSaveGame } from './fakes.mjs';
import { composeVideo, render } from './videos-compose.mjs';
import { startDemoSite } from './demo-site.mjs';

const require = createRequire(import.meta.url);
const { chromium } = require('playwright');
const OUT = resolve(process.argv[2] ?? '.');
const SITE = 'cm0102-web.fly.dev';
mkdirSync(OUT, { recursive: true });

const { BASE, tmp, close: closeSite } = await startDemoSite();

// On the page: the pointer (with a ring at each click) and the panel where the game runs. The
// pointer is a popover, raised above each dialog the page opens (dialogs sit in the top layer).
const OVERLAY = String.raw`(() => {
  const css = '#vid-ptr{position:fixed;inset:auto;margin:0;padding:0;border:0;background:transparent;overflow:visible;pointer-events:none;color:inherit}'
    + '#vid-ptr{left:0;top:0;width:30px;height:30px}'
    + '#vid-ptr .ring{position:absolute;left:-16px;top:-16px;width:34px;height:34px;border-radius:50%;border:3px solid #35b673;opacity:0}'
    + '#vid-ptr.down .ring{animation:vring .55s ease-out}@keyframes vring{from{transform:scale(.35);opacity:1}to{transform:scale(1.5);opacity:0}}'
    + '#canvas{opacity:0!important}#toasts{display:none!important}'
    + '#stage{background:repeating-linear-gradient(135deg,#101820 0 22px,#0d141b 22px 44px)!important}'
    + '#vid-game{position:absolute;inset:0;display:grid;place-items:center;pointer-events:none;font:600 26px/1.4 Inter,system-ui,sans-serif;color:#8fa3b5;text-align:center}'
    + '#vid-game small{display:block;font-weight:400;font-size:18px;color:#6c7f90}';
  const raise = () => { for (const id of ['vid-ptr']) { const el = document.getElementById(id); if (!el) continue; try { if (el.matches(':popover-open')) el.hidePopover(); el.showPopover(); } catch {} } };
  addEventListener('DOMContentLoaded', () => {
    const style = document.createElement('style'); style.textContent = css; document.head.append(style);
    const ptr = document.createElement('div'); ptr.id = 'vid-ptr'; ptr.popover = 'manual';
    ptr.innerHTML = '<div class="ring"></div><svg width="30" height="30" viewBox="0 0 24 24"><path d="M4 2l15 10.5-6.6 1.3 3.9 7.2-2.7 1.4-3.9-7.3L4 20z" fill="#fff" stroke="#111" stroke-width="1.4" stroke-linejoin="round"/></svg>';
    ptr.style.left = '640px'; ptr.style.top = '400px'; document.body.append(ptr);
    const stage = document.getElementById('stage');
    if (stage) { const g = document.createElement('div'); g.id = 'vid-game'; g.innerHTML = '<div>Your game of CM 01/02 runs here<small>played with the mouse and keys, as the original</small></div>'; stage.style.position ||= 'relative'; stage.append(g); }
    raise();
    new MutationObserver(raise).observe(document.body, { subtree: true, attributes: true, attributeFilter: ['open'] });
    // The stand-in's game files are a few KB: the start box says what the site's are.
    const box = document.getElementById('overlay');
    if (box) new MutationObserver(() => { for (const el of box.querySelectorAll('p')) if (/^\s*0\.0 MB of game files/.test(el.textContent)) el.innerHTML = el.innerHTML.replace(/0\.0 MB of game files/, '467 MB of game files'); })
      .observe(box, { childList: true, subtree: true });
  });
  addEventListener('mousemove', (e) => { const p = document.getElementById('vid-ptr'); if (p) { p.style.left = e.clientX + 'px'; p.style.top = e.clientY + 'px'; } }, true);
  addEventListener('mousedown', () => { const p = document.getElementById('vid-ptr'); if (!p) return; p.classList.remove('down'); void p.offsetWidth; p.classList.add('down'); }, true);
})();`;

const browser = await chromium.launch();
const saves = { juve: await fakeSaveGame() };

async function recorder(name) {
  const dir = join(tmp, `video-${name}`);
  const ctx = await browser.newContext({ viewport: { width: 1280, height: 800 }, recordVideo: { dir, size: { width: 1280, height: 800 } } });
  await ctx.addInitScript(OVERLAY);
  const p = await ctx.newPage();
  const t0 = Date.now();                                 // the capture starts with the page
  const now = () => (Date.now() - t0) / 1000;
  p.on('dialog', (d) => d.accept());
  const pause = (ms) => p.waitForTimeout(ms);
  // A step: its words, shown for ms, in the log; the box pointed at just before it (point, not click) is its zoom.
  const captions = [];
  let pointed = null;
  const say = async (text, sub = '', ms = 3000) => {
    const start = now();
    await pause(ms);
    captions.push({ text, sub, start, end: now(), focus: pointed });
    pointed = null;
  };
  const point = async (sel, { steps = 24, wait = 350 } = {}) => {
    const el = p.locator(sel).first();
    await el.scrollIntoViewIfNeeded();
    const b = await el.boundingBox();
    await p.mouse.move(b.x + b.width / 2, b.y + b.height / 2, { steps });
    await pause(wait);
    pointed = { x: b.x, y: b.y, w: b.width, h: b.height };
    return b;
  };
  const click = async (sel, opts) => { const b = await point(sel, opts); pointed = null; await p.mouse.click(b.x + b.width / 2, b.y + b.height / 2); await pause(500); };
  const type = async (sel, text) => {
    const b = await point(sel);
    await p.mouse.click(b.x + b.width / 2, b.y + b.height / 2, { clickCount: 3 });
    await p.keyboard.press('ControlOrMeta+A');
    await p.keyboard.type(text, { delay: 85 });
    await pause(400);
  };
  const putSave = (file, bytes) => p.evaluate(([n, b]) => window.Module.FS.writeFile(`/user/${n}`, new Uint8Array(b)), [file, Array.from(bytes)]);
  const finish = async (file) => {
    const video = p.video();
    await ctx.close();
    const webm = await video.path();
    const capture = join(tmp, `${name}-capture.mp4`), mp4 = join(OUT, file), project = join(OUT, `${name}-project`);
    execFileSync('ffmpeg', ['-y', '-v', 'error', '-i', webm, '-c:v', 'libx264', '-preset', 'slow', '-crf', '14', '-r', '30', '-pix_fmt', 'yuv420p', '-movflags', '+faststart', capture]);
    rmSync(project, { recursive: true, force: true });
    // The music of this film: CM_VIDEO_MUSIC_<NAME> (APP, EDITOR), else CM_VIDEO_MUSIC for both, else none.
    const music = process.env[`CM_VIDEO_MUSIC_${name.toUpperCase()}`] || process.env.CM_VIDEO_MUSIC || null;
    const seconds = composeVideo({ dir: project, capture, log: { captions }, music });
    writeFileSync(join(project, 'steps.json'), JSON.stringify(captions, null, 1));
    render(project, `${mp4}.part.mp4`);
    renameSync(`${mp4}.part.mp4`, mp4);
    console.log(`made ${mp4} (${Math.round(seconds)} s)`);
  };
  return { p, pause, say, point, click, type, putSave, finish };
}

// ---- How to edit a saved game (and a data set) ----
async function editorVideo() {
  const v = await recorder('editor');
  const { p } = v;
  await p.goto(BASE);
  await p.waitForSelector('#go');
  await v.say('How to edit a saved game', 'its players, or your own database for new games', 3800);
  await v.point('#go');
  await v.say('Start the game as usual', '', 2000);
  await v.click('#go');
  await p.waitForFunction(() => window.__fake, null, { timeout: 30000 });
  await v.putSave('Juventus 2001-07-29.sav', saves.juve);
  await v.point('#saves-open');
  await v.say('The editor is in Saved games', 'the button at the top of the page', 2600);
  await v.click('#saves-open');
  await p.waitForSelector('#saves-list tr:has-text("Juventus 2001-07-29.sav") button[data-edit]');
  await v.say('Each saved game has an Edit button', '', 2200);
  await v.click('#saves-list tr:has-text("Juventus 2001-07-29.sav") button[data-edit]');
  await p.waitForSelector('#editor[open] .ed-row');
  await v.say('The editor lists the players of the save, best first', 'Search by a player\'s name or by a club', 3600);
  await v.type('#editor-search', 'del piero');
  await p.waitForFunction(() => document.querySelectorAll('.ed-row').length === 1);
  await v.say('Pick the player', '', 1400);
  await v.click('.ed-row');
  await p.waitForSelector('#editor-form [data-f="Pace"]');
  await v.say('His ability, positions, attributes and hidden attributes', 'Attributes go from 1 to 20, ability from 1 to 200', 3800);
  await v.type('[data-f="ca"]', '190');
  await v.type('[data-f="Pace"]', '18');
  await v.say('A changed field is marked', 'change as many as you like', 2600);
  await v.click('#editor-apply');
  await v.say('Apply keeps the changes of this player', 'then pick another, or save', 3000);
  await v.point('#editor-save');
  await v.say('Save writes them into the saved game', 'The save as it was is kept too, as an older copy', 3400);
  await v.click('#editor-save');
  await p.waitForFunction(() => /^Saved:/.test(document.getElementById('editor-note').textContent), null, { timeout: 30000 });
  await v.pause(1200);
  await v.say('In the game: Load Game, and pick the save', 'the player has his new ratings', 3400);
  await v.click('#editor-close');
  // A data set: the Editor button.
  await v.point('#editor-open');
  await v.say('The Editor button edits data sets too', 'your own copy of the game\'s database, for new games', 3600);
  await v.click('#editor-open');
  await p.waitForSelector('#editor[open] #editor-new');
  await v.say('Make one from the game\'s own data', 'give it a name', 2400);
  await v.type('#editor-new-name', 'My database');
  await v.click('#editor-new');
  await p.waitForSelector('#editor .ed-row', { timeout: 30000 });
  await v.type('#editor-search', 'del piero');
  await p.waitForFunction(() => document.querySelectorAll('.ed-row').length === 1);
  await v.click('.ed-row');
  await p.waitForSelector('#editor-form [data-f="club"]');
  await v.say('Move a player to another club, change his contract or ratings', '', 2800);
  await v.type('[data-f="club"]', 'AC Milan (Italy)');
  await v.click('#editor-apply');
  await v.point('#editor-add');
  await v.say('Add a player creates a new one', '', 2400);
  await v.click('#editor-save');
  await p.waitForFunction(() => /^Saved:/.test(document.getElementById('editor-note').textContent), null, { timeout: 30000 });
  await v.pause(800);
  await v.click('#editor-close');
  await v.click('#saves-open');
  await v.click('#saves .tabs [data-tab="sets"]');
  await p.waitForSelector('#sets-list [data-set-use]');
  await v.point('#sets-list [data-set-use]');
  await v.say('To play it: Saved games, Databases, Use', 'then start a new game: it uses your database', 4000);
  await v.click('#saves-close');
  await v.say('Have fun!', SITE, 3200);
  await v.finish('editor.mp4');
}

// ---- How to use the app ----
async function appVideo() {
  const v = await recorder('app');
  const { p } = v;
  await p.goto(BASE);
  await p.waitForSelector('#go');
  await v.say('How to play in your browser', 'the whole game, free, nothing to install', 3800);
  await v.point('#go');
  await v.say('The first visit downloads the game files into this browser', 'once; the next visits start at once', 3600);
  await v.click('#go');
  await p.waitForFunction(() => window.__fake, null, { timeout: 30000 });
  await v.say('The game runs in this tab', 'mouse and keys as in the original; on a touch screen, tap and long press', 3800);
  await v.putSave('Juventus 2001-07-29.sav', saves.juve);
  await v.putSave('Liverpool 2002-05-12.sav', saves.juve);
  await p.evaluate(() => { const FS = window.Module.FS; try { FS.mkdir('/user/tactics'); } catch {} FS.writeFile('/user/tactics/Attacking 4-3-3.tct', new Uint8Array(3000).fill(1)); });
  await v.point('#saves-open');
  await v.say('Save in the game as usual', 'your saves stay in this browser: Saved games lists them', 3000);
  await v.click('#saves-open');
  await p.waitForSelector('#saves-list button[data-play]');
  await v.point('#saves-list button[data-play]');
  await v.say('Play loads a save at once', '', 2400);
  await v.point('#saves-list button[data-kind="save"]');
  await v.say('Download keeps a copy as a file', 'for the PC game, or to bring it to another device', 3200);
  await v.point('#saves-upload');
  await v.say('Upload brings a save in', 'a .sav file, or a .zip of saves', 2800);
  const [chooser] = await Promise.all([p.waitForEvent('filechooser'), v.click('#saves-upload')]);
  await chooser.setFiles({ name: 'Arsenal 2003-04-19.sav', mimeType: 'application/octet-stream', buffer: Buffer.from(saves.juve) });
  await p.waitForSelector('#saves-list tr:has-text("Arsenal 2003-04-19.sav")', { timeout: 30000 });
  await v.say('It is in the list, ready to play', '', 2200);
  await v.point('#saves-all');
  await v.say('Download all keeps a copy of everything', 'a browser may delete a site\'s data by itself: keep copies', 3800);
  await v.click('#saves .tabs [data-tab="tactics"]');
  await p.waitForSelector('#tactics-list button[data-tactic]');
  await v.say('Tactics: the ones you saved in the game', 'download them, or upload a .tct from the PC game or a friend', 3800);
  await v.click('#saves .tabs [data-tab="sets"]');
  await p.waitForSelector('#sets-lib button[data-lib-get]', { timeout: 20000 });
  await v.say('Databases: play another season', 'free databases to download, or your own', 3600);
  await v.point('#sets-lib tr:has-text("2018-19") button[data-lib-get]');
  await v.say('Get one from the list', '', 2200);
  await v.click('#sets-lib tr:has-text("2018-19") button[data-lib-get]');
  await p.waitForSelector('#sets-season', { timeout: 30000 });
  await v.point('[data-set-go]');
  await v.say('Use it for your next new game', 'its season is chosen for you; the next new game is played with it', 4000);
  await v.click('[data-set-go]');
  await v.pause(1200);
  await v.click('#saves-close');
  if (await p.isVisible('#fullscreen')) {
    await v.point('#fullscreen');
    await v.say('Full screen gives the game the whole screen', '', 2600);
  }
  await v.click('#account-open');
  await p.waitForSelector('#account[open]');
  await v.say('Cloud Saves: your saves on every computer, iPad and phone', 'an optional membership; the game itself is free', 4000);
  await v.point('#account-signin');
  await v.say('Sign in with Google, then become a member', 'each save goes to the cloud, and comes to your other devices', 4000);
  await v.click('#account-close');
  await v.click('#help-open');
  await p.waitForSelector('#help[open]');
  await v.say('Help has the controls, the touch gestures and tips', '', 3200);
  if (await p.isVisible('#help-report-open')) {
    await v.point('#help-report-open');
    await v.say('Something wrong? Send to the developer', 'your note, the log and your save', 3600);
  }
  await v.click('#help-close').catch(() => p.keyboard.press('Escape'));
  await v.say('On an iPhone or iPad: Share, Add to Home Screen', 'it then opens like an app', 3800);
  await v.say('Have fun!', SITE, 3200);
  await v.finish('app.mp4');
}

try {
  const which = process.argv[3];
  if (!which || which === 'editor') await editorVideo();
  if (!which || which === 'app') await appVideo();
} finally {
  await browser.close();
  await closeSite();
}
