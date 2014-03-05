'use strict';

angular.module('autoNetApp')
.filter('display', function () {
  return function (input) {
    return _.omit(input, 'parent', 'name', 'id');
  };
});
