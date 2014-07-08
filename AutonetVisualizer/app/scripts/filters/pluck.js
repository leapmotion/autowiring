'use strict';

// wrapper around underscore's "pluck"
angular.module('autoNetApp')
.filter('Pluck', function () {
  return function (input, key) {
    return _.pluck(input, key);
  };
});

angular.module('autoNetApp')
.filter('notEmpty', function () {
  return function (input) {
    return !_.isEmpty(input);
  };
});
