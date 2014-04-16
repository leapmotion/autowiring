'use strict';

angular.module('autoNetApp')
.controller('MainCtrl', ['$scope', 'LeapState', function ($scope, LeapState) {
  $scope.leap = LeapState;
  $scope.currentFilter = {
    contextMember: true,
    coreRunnable: true,
    eventReceiver: true,
    exceptionFilter: true,
    bolt: true
  };
}]);
