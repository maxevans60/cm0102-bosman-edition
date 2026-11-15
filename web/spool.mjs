// A request's body into a file on disk, with its sha256 for the store's signature (s3.mjs putFile): the server streams
// large uploads (sent saves, cloud saves) through its disk instead of holding them in memory, on a machine of 512 MB.
// Resolves to { path, size, sha256 }; at most limit bytes (else an error with status 413). The caller removes the file.
import { createHash, randomBytes } from 'node:crypto';
import { createWriteStream, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

export function spool(req, limit, prefix = 'cm0102-up-') {
  const path = join(tmpdir(), `${prefix}${randomBytes(8).toString('hex')}`);
  return new Promise((ok, bad) => {
    const hash = createHash('sha256'), out = createWriteStream(path);
    let n = 0, failed = false;
    const fail = (err) => { if (failed) return; failed = true; req.destroy(); out.destroy(); rmSync(path, { force: true }); bad(err); };
    req.on('data', (c) => { n += c.length; if (n > limit) fail(Object.assign(new Error('too large'), { status: 413 })); else hash.update(c); });
    req.on('error', fail);
    out.on('error', fail);
    out.on('finish', () => { if (!failed) ok({ path, size: n, sha256: hash.digest('hex') }); });
    req.pipe(out);
  });
}

export const unspool = (file) => { if (file) rmSync(file.path, { force: true }); };
