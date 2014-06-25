'use strict';

// wrapper around underscore's "pluck"
angular.module('autoNetApp')
.filter('Pluck', function () {
  return function (input, key) {
    return _.pluck(input, key);
  };
});
