'use strict';

// Omits redundent fields from list of Context properties
angular.module('autoNetApp')
.filter('display', function () {
  return function (input) {
    return _.without(input, 'bolt', 'eventReceiver');
  };
});

angular.module('autoNetApp')
.filter('TypeFilter', function(){
  return function (input, types) {
    return _.filter(input, function(ele){
      return ele.IsAnyType(types);
    });
  }
});