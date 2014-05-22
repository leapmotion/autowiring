'use strict';

angular.module('autoNetApp')
.controller('EditCtrl', ['$scope', 'LeapState', 'websocket', function($scope, LeapState, websocket) {
  $scope.leap = LeapState;
  $scope.selectedType = undefined;
  $scope.selectedCtxt = undefined;

  $scope.terminateContext = function(ctxt){
    console.log("Terminate context: ",ctxt.name);
    websocket.SendMessage("terminateContext", ctxt.id);
  };

  $scope.injectContextMember = function(){
    // Invalid member
    if (_.isUndefined($scope.selectedType)) return;
    if (_.isUndefined($scope.selectedCtxt)) return;

    websocket.SendMessage('injectContextMember', $scope.selectedCtxt, $scope.selectedType);
  }
}]);
