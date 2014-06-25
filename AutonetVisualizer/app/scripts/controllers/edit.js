'use strict';

// Controller for the "edit" view. See app.js for details
angular.module('autoNetApp')
.controller('EditCtrl', ['$scope', '$timeout', 'LeapState', 'websocket', function($scope, $timeout, LeapState, websocket) {
  $scope.leap = LeapState;
  $scope.selectedType = undefined;
  $scope.selectedCtxt = undefined;
  $scope.contextMemberAlerts = [];

  $scope.terminateContext = function(ctxt){
    console.log("Terminate context: ",ctxt.name);
    websocket.SendMessage("terminateContext", ctxt.id);
  };

  $scope.injectContextMember = function(){
    // Invalid member
    if (_.isUndefined($scope.selectedType)) {
      addContextMemberAlert("<strong>Warning:</strong> Must select a type", "warning");
      return;
    }
    if (_.isUndefined($scope.selectedCtxt)){
      addContextMemberAlert("<strong>Warning:</strong> Must select a context", "warning");
      return;
    }

    websocket.SendMessage('injectContextMember', $scope.selectedCtxt, $scope.selectedType);
  }

  function addContextMemberAlert(msg, type) {
    $scope.contextMemberAlerts.unshift({message:msg, type:type});
    $timeout(function(){
      $scope.contextMemberAlerts.pop();
    },4000);
  }

  websocket.on('newObject', function(contextID, types, objData) {
    addContextMemberAlert('<strong>New Object!</strong> ' + objData.name + ' was successfully injected', 'success');
  });
}]);
