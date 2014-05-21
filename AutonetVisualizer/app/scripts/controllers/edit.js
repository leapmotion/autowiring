'use strict';

angular.module('autoNetApp')
.controller('EditCtrl', ['$scope', 'LeapState', 'websocket', function($scope, LeapState, websocket) {
  $scope.leap = LeapState;

  $scope.terminateContext = function(ctxt){
    console.log("Terminate context: ",ctxt.name);
    websocket.SendMessage("terminateContext", ctxt.id);
  };
}]);
