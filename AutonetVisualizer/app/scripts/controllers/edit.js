'use strict';

angular.module('autoNetApp')
.controller('EditCtrl', ['$scope', 'LeapState', function($scope, LeapState) {
  $scope.leap = LeapState;

  $scope.deleteContext = function(ctxt){
    console.log("Delete context: ",ctxt.name);
  };
}]);
