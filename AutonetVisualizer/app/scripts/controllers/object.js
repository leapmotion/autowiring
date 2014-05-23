'use strict';

angular.module('autoNetApp')
.controller('ObjectCtrl', ['$scope', '$routeParams', 'LeapState', function ($scope, $routeParams, LeapState) {
  $scope.leap = LeapState;
  $scope.context = LeapState.GetContexts()[$routeParams['contextID']];
  $scope.object = $scope.context.objects[$routeParams['linkName']];
}]);
