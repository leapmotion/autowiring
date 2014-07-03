'use strict';

// Controller for the "main" view. See app.js for details
angular.module('autoNetApp')
.controller('MainCtrl', ['$scope', 'LeapState', function ($scope, LeapState) {
  $scope.leap = LeapState;
  $scope.selectedType = 'contextMember';
}]);
