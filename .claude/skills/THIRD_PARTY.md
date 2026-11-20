# Skills from other projects

Copied into this repository on 19 November 2026, so that every session on it has them. Each keeps its upstream
licence; the upstream text is unchanged except where a note at its top says so ("In this repository").

| Skills here | From | Commit | Licence | Left out |
|---|---|---|---|---|
| `ui-ux-pro-max` | nextlevelbuilder/ui-ux-pro-max-skill, `.claude/skills/ui-ux-pro-max` | 2de1134 | MIT (`ui-ux-pro-max/LICENSE`) | its script tests; the search script's path made relative to the repository root |
| `design-taste-frontend` | Leonxlnx/taste-skill, `skills/taste-skill` | f6b7657 | MIT (`design-taste-frontend/LICENSE`) | the kit (`assets/`, `scripts/`, about 40 MB) and its finish-gate Stop hook: kept as rules only |
| `animate`, `animate-expo`, `animation-performance`, `animation-vocabulary`, `apple-design`, `ask-emil`, `ask-sonner`, `break-ui`, `choreograph`, `emil-design-eng`, `find-animation-opportunities`, `gestures`, `improve-animations`, `learn-taste`, `mobile-native`, `pick-ui-library`, `prototype`, `remove-animations`, `review-animations`, `write-swift` | emilkowalski/skills, `skills/` | 64eb3de | MIT (`emil-design-eng/LICENSE`) | nothing |
| `impeccable` | pbakaus/impeccable, `.claude/skills/impeccable` | b49177d | Apache-2.0 (`impeccable/LICENSE`) | the copies for other agents; its `detect` tool is not in the skill folder upstream either |
| `ponytail`, `ponytail-audit`, `ponytail-debt`, `ponytail-gain`, `ponytail-help`, `ponytail-review` | DietrichGebert/ponytail, `skills/` | 96608e0 | MIT (`ponytail/LICENSE`) | the benchmark and the copies for other agents |
| `hyperframes`, `hyperframes-cli`, `hyperframes-registry` | heygen-com/hyperframes, `skills/` | 9e78353 | Apache-2.0 (`hyperframes/LICENSE`) | the framework itself (npm, when the CLI runs), the other skills of the repository; the usage beacon and feedback sends are off by a note |

Not copied:

- **obra/superpowers.** It is installed as a plugin already (`superpowers:*`); a second copy here would give every one
  of its skills two names.
- **garrytan/gstack.** A whole toolkit (about 100 MB) whose skills share a preamble and a browser binary that its own
  setup builds in the user's folder (`~/.claude/skills/gstack`); copied alone they do not run. Install it per user
  from its README.

To update one: clone its repository, copy the same folder over this one, keep the note at its top, and change the
commit above.

## Plugins

| Plugin | From | Version | Licence | How |
|---|---|---|---|---|
| `codex` (OpenAI's Codex in Claude Code: `/codex:review`, `/codex:adversarial-review`, `/codex:rescue`, `/codex:status`, `/codex:result`, `/codex:cancel`, `/codex:setup`) | openai/codex-plugin-cc, marketplace `openai-codex` | 1.0.6 (db52e28) | Apache-2.0 | `.claude/settings.json`: `extraKnownMarketplaces` and `enabledPlugins` |

The settings offer it; they do not install it. On your own computer, once per clone, after trusting the folder:
`claude plugin install codex@openai-codex --scope project`, then `/codex:setup` in Claude Code. It needs Node 18.18+, the
Codex CLI (`npm install -g @openai/codex`, which `/codex:setup` can do) and a ChatGPT plan or an OpenAI API key
(`!codex login`); its use counts against that plan's Codex limits. A cloud session (Claude Code on the web) does not
install plugins a repository enables, so it is not there.

What it sends: a Codex review or rescue sends the code and the diff it works on to OpenAI. Its stop-time review gate
(a Codex review each time Claude stops) is off unless `/codex:setup` turns it on; leave it off unless you want every turn
reviewed.
