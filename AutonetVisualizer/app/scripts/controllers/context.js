'use strict';

angular.module('autoNetApp')
.controller('ContextCtrl', ['$scope', 'LeapState', '$routeParams', function ($scope, LeapState, $routeParams) {
  $scope.leap = LeapState;
  $scope.allContexts = LeapState.GetContexts();

  $scope.$watchCollection('allContexts', function(){
    $scope.context = $scope.allContexts[$routeParams['contextID']]
  });
}]);
