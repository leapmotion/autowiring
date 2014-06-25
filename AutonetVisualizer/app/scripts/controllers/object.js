'use strict';

// Controller for the "object" view. See app.js
angular.module('autoNetApp')
.controller('ObjectCtrl', ['$scope', '$routeParams', 'LeapState', 'websocket', function ($scope, $routeParams, LeapState, websocket) {
  $scope.leap = LeapState;
  $scope.allContexts = LeapState.GetContexts();

  $scope.$watchCollection('allContexts', function(){
    $scope.context = $scope.allContexts[$routeParams['contextID']];
    if ($scope.context) {
      $scope.object = $scope.context.objects[$routeParams['linkName']];
    }
  });

}]);
