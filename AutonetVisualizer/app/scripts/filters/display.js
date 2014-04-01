'use strict';

// Omits redundent fields from list of Context properties
angular.module('autoNetApp')
.filter('display', function () {
  return function (input) {
    return _.omit(input, 'parent', 'name', 'id');
  };
});
