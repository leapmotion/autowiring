'use strict';

// Omits redundent fields from list of Context properties
angular.module('autoNetApp')
.filter('display', function () {
  return function (input) {
    return _.omit(input, 'parent', 'name', 'id');
  };
});

angular.module('autoNetApp')
.filter('TypeFilter', function(){
  return function (input, type) {
    if (type === 'all') return input;
    return _.filter(input, function(ele){
      return ele.IsType(type);
    });
  }
});