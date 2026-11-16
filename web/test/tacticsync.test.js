// Tests of the merge of tactics with the cloud (web/tacticsync.js, planTactics). Run: cd web && npm test
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { planTactics } from '../tacticsync.js';

const L = (o) => new Map(Object.entries(o).map(([n, mtime]) => [n, { mtime }]));
const C = (o) => new Map(Object.entries(o).map(([n, mtime]) => [n, { name: n, mtime, data: 'x' }]));

test('per name the newest wins: a file here, a file in the cloud, or its removal', () => {
  const p = planTactics(
    L({ 'Same.tct': 5, 'Newer here.tct': 9, 'Older here.tct': 2, 'Only here.tct': 4, 'Removed there.tct': 3, 'Changed after.tct': 8 }),
    C({ 'Same.tct': 5, 'Newer here.tct': 6, 'Older here.tct': 7, 'Only there.tct': 1, 'Changed after.tct': 6 }),
    { 'Removed there.tct': 4, 'Changed after.tct': 7, 'Long gone.tct': 1 });
  assert.deepEqual(p.write.sort(), ['Older here.tct', 'Only there.tct']);
  assert.deepEqual(p.remove, ['Removed there.tct']);
  assert.deepEqual(p.keep.sort(), ['Changed after.tct', 'Newer here.tct', 'Only here.tct', 'Same.tct'].sort(), 'a file changed after its removal stays');
  assert.deepEqual(p.removed, { 'Removed there.tct': 4, 'Long gone.tct': 1 });
  assert.equal(p.changed, true);
});

test('nothing to send when both sides agree', () => {
  const p = planTactics(L({ 'A.tct': 1 }), C({ 'A.tct': 1 }), { 'B.tct': 3 });
  assert.deepEqual([p.write, p.remove, p.changed], [[], [], false]);
});

test('a removal here goes up and the file goes', () => {
  const p = planTactics(L({ 'A.tct': 1 }), C({ 'A.tct': 1 }), { 'A.tct': 2 });
  assert.deepEqual([p.remove, p.keep, p.removed, p.changed], [['A.tct'], [], { 'A.tct': 2 }, true]);
});

test('a removal here of a tactic the cloud has as removed already sends nothing; a new removal does', () => {
  assert.equal(planTactics(L({}), C({}), { 'A.tct': 2 }, { 'A.tct': 2 }).changed, false);
  assert.equal(planTactics(L({}), C({}), { 'A.tct': 2 }, {}).changed, true);
});
