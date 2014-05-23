'use strict';

angular.module('autoNetApp')
.controller('RawCtrl', ['$scope', 'websocket', 'LeapState', function ($scope, websocket, LeapState) {
  $scope.events = websocket.GetEventHistory();
}]);
