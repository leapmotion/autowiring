'use strict';

angular.module('autoNetApp')
.controller('EditCtrl', ['$scope', 'LeapState', 'websocket', function($scope, LeapState, websocket) {
  $scope.leap = LeapState;
  $scope.selectedType = LeapState.GetAllTypes()[0];

  $scope.terminateContext = function(ctxt){
    console.log("Terminate context: ",ctxt.name);
    websocket.SendMessage("terminateContext", ctxt.id);
  };

  $scope.injectContextMember = function(){
    // Invalid member
    if (_.isUndefined($scope.selectedType)) return;

    console.log($scope.selectedType);
    websocket.SendMessage('injectContextMember',2, $scope.selectedType);
  }
}]);
