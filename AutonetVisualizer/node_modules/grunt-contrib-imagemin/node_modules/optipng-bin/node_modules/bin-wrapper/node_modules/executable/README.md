# executable [![Build Status](https://travis-ci.org/kevva/executable.png?branch=master)](https://travis-ci.org/kevva/executable)

Check if a file is executable using Node.js.

## Getting started

Install with npm: `npm install executable`

## Example

```js
var executable = require('executable');

executable('bash', function (err, exec) {
    console.log(exec);
    // => true
});

executable.sync('bash')
// => true
```

## API

### executable(name, cb)

Find out if a file is executable. Returns `true` if it is.

## License

[MIT License](http://en.wikipedia.org/wiki/MIT_License) (c) [Kevin Mårtensson](https://github.com/kevva)
