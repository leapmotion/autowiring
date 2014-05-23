'use strict';

angular.module('autoNetApp')
.controller('ContextCtrl', ['$scope', 'LeapState', '$routeParams', function ($scope, LeapState, $routeParams) {
  $scope.leap = LeapState;
  $scope.context = LeapState.GetContext()[$routeParams['contextID']];
}]);
