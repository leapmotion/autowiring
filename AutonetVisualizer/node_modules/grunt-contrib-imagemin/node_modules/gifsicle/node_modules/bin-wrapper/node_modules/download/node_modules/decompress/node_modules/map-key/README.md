# map-key [![Build Status](https://travis-ci.org/kevva/map-key.png?branch=master)](http://travis-ci.org/kevva/map-key)

> Map an object key that ends with a value

## Getting started

```bash
npm install --save map-key
```

## Usage

```js
var map = require('map-key');

map({ '.tar.gz': 'bar' }, 'foo.tar.gz');
// => 'bar'
```

## License

[MIT License](http://en.wikipedia.org/wiki/MIT_License) © [Kevin Mårtensson](https://github.com/kevva)
