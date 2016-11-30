# vime, a faster vm module

## Synopsis

Execute JS code in a new VM context.

## Motivation

The [built-in vm module] is flexible but slow.  Code sometimes runs [200x
slower](https://github.com/nodejs/benchmarking/issues/75#issuecomment-262740252)
inside a vm context than outside.  Vime sacrifices some flexibility in exchange
for fast and reliable performance.

## Usage

The module exports a single function.

```js
const vime = require('vime');
const result = vime('1 + 1');
console.log(result);  // Prints "2".
```

To execute more code in the same vime context, first grab a reference to the
global object (or any other object from the context):

```js
const vime = require('vime');
const context = vime('this');
vime('var x = 1', context);
console.log(vime('x + x', context));  // Prints "2".
```

Exporting functions from the main context is done in similar fashion:

```js
const vime = require('vime');
const context = Object.assign(vime('this'), { console, require });
vime('console.log(require("./package.json"))');
```

It follows that `vime()` is not restricted to running JS code in a _new_
context, you can also execute it in the current context:

```js
const vime = require('vime');
const context = global;  // Or any object from this context.
vime('console.log("ok")', context);  // Prints "ok".
```

## Caveats

`instanceof` checks don't work across contexts because each context gets
its own copy of `Object`, `Array`, and other built-ins.

```js
const vime = require('vime');
const array = vime('[]');
console.log(Array.isArray(array));  // Prints "true".
console.log(array instanceof Array);  // Prints "false".
```

References to objects from a context keep the context alive until the last
reference goes away.  Because contexts are heavy-weight objects (a context
is a complete JS runtime), keeping many contexts around has a notable impact
on memory usage:

```js
const vime = require('vime');
console.log(process.memoryUsage().rss >>> 20);  // 25 MB on my machine.
const leak = [];
for (let i = 0; i < 100; ++i) leak.push(vime('this'));
console.log(process.memoryUsage().rss >>> 20);  // 100 MB on my machine.
```

Use [node-heapdump] or the [built-in inspector] to debug such issues.

Important note: this module is currently not designed for executing untrusted
code ("sandboxing.")  If that is an important use case for you, please file a
bug report.

## Reporting bugs

Report bugs at https://github.com/bnoordhuis/vime/issues.  Please check
existing issues first.  If possible, include a test case demonstrating
the bug.

UNIX users, please include the output of `node -v` and `uname -a`.

Windows users, please include the output of `node -v` and `winver`.

When reporting build errors, include the full terminal output from
`npm install vime` on down and the output of `g++ -v` or `clang++ -v`
(if you are a UNIX user) or the version of Visual Studio or Build Tools
(if you are a Windows user.)

## License

ISC.  See the LICENSE file in the top-level directory.

[built-in vm module]: https://nodejs.org/dist/latest-v7.x/docs/api/vm.html
[built-in inspector]: https://medium.com/@paul_irish/debugging-node-js-nightlies-with-chrome-devtools-7c4a1b95ae27
[node-heapdump]: https://github.com/bnoordhuis/node-heapdump
