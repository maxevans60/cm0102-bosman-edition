// Tests of the game's long jobs in the bar (web/progress.js). Run: cd web && npm test
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { TOLD_AFTER_MS, progressText, watchProgress } from '../progress.js';

test('the text: the stage, its share and the time', () => {
  assert.equal(progressText('Player histories', 45, 38000), 'Player histories 45% · 0:38');
  assert.equal(progressText('Initialising game data', 101, 125000), 'Initialising game data · 2:05', 'a stage with no bar');
});

test('a named stage shows at once, timed from its start; it goes when it ends, another starts, or the game waits', () => {
  assert.equal(TOLD_AFTER_MS, 2000, 'display.c, STAGE_TELL_S');
  let t = 0, share = -1;
  const said = [];
  const w = watchProgress({ read: () => share, set: (s) => said.push(s), now: () => t });
  w.tick();
  assert.deepEqual(said, [], 'nothing runs: nothing said');
  t = 36000;
  share = 10;
  w.line('recomp: progress: Player histories');                 // it has run 2 s
  w.tick();
  assert.deepEqual(said, ['Player histories 10% · 0:02']);
  t = 72000;
  share = 45;
  w.tick();
  assert.equal(said.at(-1), 'Player histories 45% · 0:38');
  w.line('recomp: progress: Player histories took 38.4 s');
  assert.equal(said.at(-1), '', 'its end clears the bar');
  t = 80000;
  share = 101;
  w.line('recomp: progress: Initialising game data');
  w.tick();
  assert.equal(said.at(-1), 'Initialising game data · 0:02');
  w.line('recomp: progress: Creating Shortlists');              // the next one, named while the last showed
  assert.equal(said.at(-1), '', 'the last stage\'s text goes when the next is named');
  t = 85000;
  share = 30;
  w.tick();
  assert.equal(said.at(-1), 'Creating Shortlists 30% · 0:07');
  share = -1;                                                    // the game waits for the player
  w.tick();
  assert.equal(said.at(-1), '');
  w.line('page: something else');
  w.tick();
  assert.equal(said.at(-1), '');
});
