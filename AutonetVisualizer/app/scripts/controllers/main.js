'use strict';

angular.module('autoNetApp')
.controller('MainCtrl', ['$scope', 'LeapState', function ($scope, LeapState) {
  $scope.leap = LeapState;
  $scope.currentFilter = 'contextMember';
}]);
