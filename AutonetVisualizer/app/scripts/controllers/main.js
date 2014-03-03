'use strict';

angular.module('autoNetApp')
.controller('MainCtrl', ['$scope', 'websocket', function ($scope, websocket) {
  $scope.contexts = [1,2,3,4,5];
  $scope.objects = ['A', 'B', 'C'];
}]);
