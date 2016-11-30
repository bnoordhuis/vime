// Copyright (c) 2016, Ben Noordhuis <info@bnoordhuis.nl>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

'use strict';

const assert = require('assert');
const vime = require('./');

assert.strictEqual(vime(''), undefined);
assert.strictEqual(vime('{}'), undefined);  // Parsed as code block.
assert.strictEqual(vime('1+1'), 2);
assert.strictEqual(Number.isNaN(vime('{}+{}')), true);
assert.throws(() => vime('{}*{}'), /SyntaxError/);
assert.throws(() => vime('boom'), /ReferenceError/);
assert.strictEqual(vime('this', {}), global);

{
  const context = Object.assign(vime('this'), { vime });
  assert.strictEqual(vime('vime("this", this)', context), context);
  assert.strictEqual(vime('vime("this", vime)', context), global);
}
