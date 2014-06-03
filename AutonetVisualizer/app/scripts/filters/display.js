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
  return function (input, type) {
    input = _.sortBy(input, function(ele){return ele.name;});

    if (type === 'all'){
      return input;
    }
    return _.filter(input, function(ele){
      return ele.IsType(type);
    });
  };
});

angular.module('autoNetApp')
.filter('Limit', function(){
  return function(input, length){
    if (input.length <= length){
      return input;
    }

    return input.substring(0,length-3) + "...";
  };
});