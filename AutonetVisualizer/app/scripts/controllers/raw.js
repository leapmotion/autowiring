'use strict';

angular.module('autoNetApp')
.controller('RawCtrl', function ($scope, websocket, LeapState) {
  $scope.events = websocket.GetEventHistory();
});
