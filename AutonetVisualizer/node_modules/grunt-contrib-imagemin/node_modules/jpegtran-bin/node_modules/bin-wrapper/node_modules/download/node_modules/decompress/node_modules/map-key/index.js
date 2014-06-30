'use strict';

var find = require('mout/array/find');
var endsWith = require('mout/string/endsWith');

/**
 * Map an object key that ends with a value
 *
 * @param {Object} obj
 * @param {String} val
 * @api public
 */

module.exports = function (obj, val) {
    var ret = find(Object.keys(obj), function (ret) {
        return endsWith(val, ret);
    });

    return ret ? obj[ret] : null;
};
