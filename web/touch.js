// Touch input (web/README.md, "On an iPad or another touch device"): what a finger does on the game, as the mouse
// that the game knows.
//
//   a tap                      a click of the left button
//   a drag                     the left button held and moved (scroll bars, the tactics board)
//   a long press               the right button, down when the ring under the finger has filled and up when the finger
//                              lifts: lifted at once, a click of the right button; moved first, a drag with the right
//                              button (on the tactics board, a player's run: the game drags a widget of flag 0x100 while
//                              the right button is held, FUN_00561210, docs/research/2026-10-13-input-inventory.md 2.6)
//   a tap with two fingers     a click of the right button
//   Right click, then a touch  the right button for that touch: a tap is a click of the right button (no timing, so a
//                              player's marker on the tactics board cannot become a drag; right-clicking a player
//                              there opens his instructions), a drag is a drag with the right button (his run)
//   a drag with two fingers    the mouse wheel (lists scroll with the fingers)
//
// SDL's browser backend has its own touch handling: it turns every touch into the left button at once, so it has no
// right button and no wheel. This file takes the touches of the canvas before SDL sees them (a capture listener on
// the window, made before the engine loads) and sends mouse and wheel events to the canvas in their place. SDL reads
// those as it reads a real mouse (SDL_emscriptenevents.c: mousemove and mousedown on the canvas, mouseup on the
// document, wheel on the canvas; a button event carries no place, so a move goes first). A mouse or a trackpad on a
// touch device sends mouse events of its own, which pass through untouched.
//
// The game reads the mouse once a frame: its pump (0x68d520) handles every message that waits, then the input code
// looks at the button flags and at the place of the last WM_MOUSEMOVE (0x68d922; a button message carries no place for
// it, docs/research/2026-10-13-input-inventory.md section 1). So a press and the moves that follow it in the same frame
// read as a press where the moves ended, and a press and its release in the same frame as no press. Each made press
// therefore stands alone for CLICK_MS: the moves and the release that come in that time wait, and go after it (the last
// place only). A drag of a player onto another (a substitution, Tactics_Screen::swap_slots 0x8ab7e0, called from the
// drop at 0x8a1d22) then starts on the player the finger touched first, not on the one it had reached.
export const SLOP = 12;          // CSS pixels a finger may wander and still be a tap or a long press
export const SLOP_MIN = 6;       // on a small picture: rows a few pixels high, so a drag to the next row is a drag
export const SLOP_GAME = 10;     // ... the slop there is this many of the picture's own pixels, kept in SLOP_MIN..SLOP
export const LONG_MS = 450;      // a press this long without a move puts the right button down
export const CLICK_MS = 80;      // from a made button down to its up: the game sees them in different input polls
export const WHEEL_PX = 36;      // finger travel for one step of the wheel

// onRightUsed(): the armed right click was used (or disarmed); the page's button shows it.
// pictureWidth(): the width of the game's picture in its own pixels (1280, or 800 in classic mode).
export function mountTouch({ canvas, stage, fx, pictureWidth = () => 1280, onFirstTouch = () => {}, onTap = () => {}, onRightUsed = () => {} }) {
  let mode = 'idle';              // idle | pending | drag | right | scroll | done (right: the long press's button is down)
  let first = null;               // the first finger: {id, x0, y0, x, y, t0, button}
  let longTimer = 0, upTimer = 0, pendingUp = null, started = false;
  let scroll = null;              // {y, acc, moved, t0}
  let rightArmed = false;         // the page's Right click button: the next tap is a click of the right button
  let hold = null;                // the ring that grows under a held finger
  let gate = null;                // a made press's CLICK_MS: {move, up, timer}, what waits for its end

  const send = (type, x, y, button, buttons) => canvas.dispatchEvent(new MouseEvent(type, {
    bubbles: true, cancelable: true, view: window, clientX: x, clientY: y, screenX: x, screenY: y, button, buttons }));
  const wheel = (x, y, deltaY) => canvas.dispatchEvent(new WheelEvent('wheel', {
    bubbles: true, cancelable: true, view: window, clientX: x, clientY: y, deltaY, deltaMode: 0 }));

  function ping(x, y, kind) {
    if (!fx) return;
    const r = stage.getBoundingClientRect();
    const el = document.createElement('i');
    el.className = `ping ${kind}`;
    el.style.left = `${x - r.left}px`;
    el.style.top = `${y - r.top}px`;
    el.addEventListener('animationend', () => el.remove());
    fx.appendChild(el);
    while (fx.childElementCount > 6) fx.firstElementChild.remove();
  }

  // The ring of a press that may become a long press: it fills over LONG_MS, so the player sees that holding works.
  function holdStart(x, y) {
    holdEnd();
    if (!fx) return;
    const r = stage.getBoundingClientRect();
    hold = document.createElement('i');
    hold.className = 'ping hold';
    hold.style.left = `${x - r.left}px`;
    hold.style.top = `${y - r.top}px`;
    hold.style.animationDuration = `${LONG_MS}ms`;
    fx.appendChild(hold);
  }
  function holdEnd() { if (hold) { hold.remove(); hold = null; } }

  // The up of a made click, now: before another press, or when its time has come.
  function flushUp() {
    clearTimeout(upTimer);
    if (pendingUp) { const p = pendingUp; pendingUp = null; send('mouseup', p.x, p.y, p.button, 0); }
    gateEnd();
  }

  // The press of a drag or a long press: down now, alone in the game's frame for CLICK_MS.
  function press(x, y, button) {
    flushUp();
    send('mousemove', x, y, 0, 0);
    send('mousedown', x, y, button, button === 2 ? 2 : 1);
    gate = { move: null, up: null, timer: setTimeout(gateEnd, CLICK_MS) };
  }
  function gateEnd() {
    if (!gate) return;
    const g = gate;
    gate = null;
    clearTimeout(g.timer);
    if (g.move) send('mousemove', g.move.x, g.move.y, 0, g.move.buttons);
    if (g.up) send('mouseup', g.up.x, g.up.y, g.up.button, 0);
  }
  const moveTo = (x, y, buttons) => { if (gate) gate.move = { x, y, buttons }; else send('mousemove', x, y, 0, buttons); };
  const upAt = (x, y, button) => { if (gate) gate.up = { x, y, button }; else send('mouseup', x, y, button, 0); };
  // How far a finger may wander for a tap: SLOP_GAME of the picture's pixels, in SLOP_MIN..SLOP CSS pixels.
  const slop = () => {
    const w = canvas.getBoundingClientRect().width, pw = pictureWidth();
    return w > 0 && pw > 0 ? Math.min(SLOP, Math.max(SLOP_MIN, SLOP_GAME * w / pw)) : SLOP;
  };

  function click(x, y, button) {
    flushUp();
    send('mousemove', x, y, 0, 0);
    send('mousedown', x, y, button, button === 2 ? 2 : 1);
    pendingUp = { x, y, button };
    upTimer = setTimeout(flushUp, CLICK_MS);
  }

  const find = (list, id) => { for (const t of list) if (t.identifier === id) return t; return null; };
  const centre = (touches) => {
    let x = 0, y = 0;
    for (const t of touches) { x += t.clientX; y += t.clientY; }
    return { x: x / touches.length, y: y / touches.length };
  };

  function onStart(e) {
    if (e.target !== canvas) return;
    e.preventDefault();
    e.stopImmediatePropagation();
    if (!started) { started = true; onFirstTouch(); }
    const t = e.changedTouches[0];
    if (mode === 'idle') {
      flushUp();
      first = { id: t.identifier, x0: t.clientX, y0: t.clientY, x: t.clientX, y: t.clientY, t0: performance.now() };
      mode = 'pending';
      send('mousemove', t.clientX, t.clientY, 0, 0);          // the pointer is under the finger: hover, and the wheel's place
      if (!rightArmed) holdStart(t.clientX, t.clientY);       // armed, a tap is the right click already
      longTimer = setTimeout(() => {
        if (mode !== 'pending') return;
        mode = 'right';                                       // down now, up when the finger lifts: it may drag first
        holdEnd();
        navigator.vibrate?.(15);                              // Android: the press is felt (iOS has no vibration for pages)
        ping(first.x, first.y, 'right');
        press(first.x, first.y, 2);
        if (rightArmed) { rightArmed = false; onRightUsed(); }
      }, LONG_MS);
    } else if (mode === 'pending' && e.touches.length === 2) {
      clearTimeout(longTimer);
      holdEnd();
      const c = centre(e.touches);
      scroll = { y: c.y, acc: 0, moved: false, t0: first.t0 };
      mode = 'scroll';
      send('mousemove', c.x, c.y, 0, 0);
    }
  }

  function onMove(e) {
    if (mode === 'idle' || (e.target !== canvas && mode === 'done')) return;
    if (e.target === canvas) { e.preventDefault(); e.stopImmediatePropagation(); }
    if (mode === 'pending' || mode === 'drag' || mode === 'right') {
      const t = find(e.touches, first.id);
      if (!t) return;
      first.x = t.clientX;
      first.y = t.clientY;
      if (mode === 'pending' && Math.hypot(t.clientX - first.x0, t.clientY - first.y0) > slop()) {
        clearTimeout(longTimer);
        holdEnd();
        mode = 'drag';
        first.button = rightArmed ? 2 : 0;                     // armed: the drag is the right button's
        if (rightArmed) { rightArmed = false; onRightUsed(); ping(first.x0, first.y0, 'right'); }
        press(first.x0, first.y0, first.button);
      }
      if (mode === 'drag') moveTo(t.clientX, t.clientY, first.button === 2 ? 2 : 1);
      else if (mode === 'right') moveTo(t.clientX, t.clientY, 2);
    } else if (mode === 'scroll' && e.touches.length >= 2) {
      const c = centre(e.touches);
      scroll.acc += c.y - scroll.y;
      scroll.y = c.y;
      while (Math.abs(scroll.acc) >= WHEEL_PX) {
        const down = scroll.acc > 0;                            // the fingers go down: the list follows, as the wheel turned up
        scroll.acc -= down ? WHEEL_PX : -WHEEL_PX;
        scroll.moved = true;
        wheel(c.x, c.y, down ? -100 : 100);
      }
    }
  }

  function onEnd(e) {
    if (mode === 'idle') return;
    if (e.target === canvas) { e.preventDefault(); e.stopImmediatePropagation(); }
    // Another finger lifted while the first one drags (with either button): the drag goes on, and the first finger's
    // own lift lets the button go (else the game kept it held).
    if ((mode === 'drag' || mode === 'right') && !find(e.changedTouches, first.id) && find(e.touches, first.id)) return;
    clearTimeout(longTimer);
    holdEnd();
    const cancelled = e.type === 'touchcancel';
    if (mode === 'pending' && find(e.changedTouches, first.id)) {
      if (!cancelled && rightArmed) {
        rightArmed = false;
        ping(first.x, first.y, 'right');
        click(first.x, first.y, 2);
        onRightUsed();
      } else if (!cancelled) {
        onTap(first.x, first.y);                                // inside the touch's own handler: it may open the keyboard
        ping(first.x, first.y, 'tap');
        click(first.x, first.y, 0);
      }
    } else if ((mode === 'drag' || mode === 'right') && find(e.changedTouches, first.id)) {
      upAt(first.x, first.y, mode === 'right' ? 2 : first.button);
    } else if (mode === 'scroll' && !scroll.moved && !cancelled && performance.now() - scroll.t0 < 400) {
      const c = centre([...e.touches, ...e.changedTouches]);
      ping(c.x, c.y, 'right');
      click(c.x, c.y, 2);
      scroll.moved = true;                                      // once: the second finger's lift is not another click
    }
    mode = e.touches.length === 0 ? 'idle' : mode === 'scroll' ? 'scroll' : 'done';
    if (mode === 'idle') { first = null; scroll = null; }
  }

  const opts = { capture: true, passive: false };
  addEventListener('touchstart', onStart, opts);
  addEventListener('touchmove', onMove, opts);
  addEventListener('touchend', onEnd, opts);
  addEventListener('touchcancel', onEnd, opts);
  // Safari's own pinch: it would zoom the page under the game.
  document.addEventListener('gesturestart', (e) => e.preventDefault());
  return {
    // The page's Right click button: on, the next touch on the game is the right button's (a tap, a drag, a long press).
    armRight(on) { rightArmed = Boolean(on); },
    get rightArmed() { return rightArmed; },
  };
}

// A Mac: Control with a click is the right button there, and the only right-button drag a trackpad makes easily. The
// browser reports the left button with ctrlKey, which the game would take for a left click (a player moved, not given
// his run). On the game, a press of the left button with Control becomes a press of the right button, and its release
// (wherever it comes, even with Control let go) the right button's release. The game reads no Control with a click
// (its window procedure 0x68d630 sets the button flags b67a34 and b67a38 only, docs/research/2026-10-13-input-inventory.md),
// so nothing is lost. Only the browser's own events: the touches above send theirs as made events.
export const isMac = () => /Mac/i.test(navigator.userAgentData?.platform ?? navigator.platform ?? '');
export function mountMacRightButton({ canvas, mac = isMac() }) {
  if (!mac) return { get held() { return false; } };
  let held = false;
  const send = (target, type, e, button, buttons) => target.dispatchEvent(new MouseEvent(type, {
    bubbles: true, cancelable: true, view: window, clientX: e.clientX, clientY: e.clientY, screenX: e.screenX, screenY: e.screenY, button, buttons }));
  addEventListener('mousedown', (e) => {
    if (!e.isTrusted || e.target !== canvas || e.button !== 0) return;
    if (!e.ctrlKey) { letGo(); return; }               // a plain press: a right press still held ends first
    e.preventDefault();
    e.stopImmediatePropagation();
    held = true;
    send(canvas, 'mousedown', e, 2, 2);
    canvas.focus({ preventScroll: true });              // as a click of the mouse does (loader.js): the keys go to the game
  }, true);
  addEventListener('mouseup', (e) => {
    if (!e.isTrusted || !held || e.button !== 0) return;
    e.stopImmediatePropagation();
    held = false;
    send(canvas, 'mouseup', e, 2, 0);                  // it bubbles to the document, where SDL listens for ups
  }, true);
  // The window lost the focus while the press was held (Command-Tab): its release does not come here, so the right
  // button goes up now, else the next click's release would be taken for it and the game kept its left button held.
  const letGo = () => {
    if (!held) return;
    held = false;
    const r = canvas.getBoundingClientRect();
    send(canvas, 'mouseup', { clientX: r.left, clientY: r.top, screenX: 0, screenY: 0 }, 2, 0);
  };
  addEventListener('blur', letGo);
  document.addEventListener('visibilitychange', () => { if (document.hidden) letGo(); });
  return { get held() { return held; } };
}
