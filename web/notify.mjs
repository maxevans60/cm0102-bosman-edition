// A notice to the developer when a player sends a report (web/README.md, "Send to the developer"): a short message to
// the webhook of CM_REPORT_WEBHOOK, so a report is seen the day it comes and not at the next look at the admin page.
// The body is JSON with the text twice: "content" for a Discord webhook, "text" for a Slack incoming webhook and for a
// Telegram bot's sendMessage link (with chat_id in its query); any other service that takes a JSON POST gets the same.
// The text names what the game said, the browser and whether a save came, and links the admin page: never the player's
// note or log, which stay on this server (privacy.html).
export const NOTICES_A_DAY = 30;    // then one notice that more came, and none until the next day (UTC)
const TIMEOUT_MS = 10000;

// url: the webhook ('' or none: off). Returns { on, notice(text) }; notice never throws.
export function createNotifier({ url = '', fetch = globalThis.fetch, log = (t) => console.log(t), now = () => Date.now() } = {}) {
  let day = '', count = 0;
  async function notice(text) {
    if (!url) return;
    const d = new Date(now()).toISOString().slice(0, 10);
    if (d !== day) { day = d; count = 0; }
    if (count > NOTICES_A_DAY) return;
    const body = ++count > NOTICES_A_DAY ? 'More reports came today: see the admin page. No more notices until tomorrow.' : text;
    try {
      const r = await fetch(url, { method: 'POST', headers: { 'content-type': 'application/json' },
        body: JSON.stringify({ content: body, text: body }), signal: AbortSignal.timeout(TIMEOUT_MS) });
      if (!r.ok) throw new Error(`the webhook answered ${r.status}`);
    } catch (e) {
      log(`stats: report notice not sent: ${e.message}`);
    }
  }
  return { on: Boolean(url), notice };
}
