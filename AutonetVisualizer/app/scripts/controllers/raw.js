'use strict';

angular.module('autoNetApp')
.controller('RawCtrl', function ($scope, LeapState) {
  $scope.leap = LeapState;
});
