'use strict';

angular.module('autoNetApp')
.controller('HeaderCtrl', ['$scope', 'websocket', function ($scope, websocket) {
  $scope.isSubscribed = false;

  $scope.subscribe = function() {
    if (websocket.isConnected()){
      websocket.subscribe();
    }
  };

  $scope.unsubscribe = function() {
    if (websocket.isConnected()){
      websocket.unsubscribe();
    }
  };

  websocket.on('subscribed', function(){
    $scope.isSubscribed = true;
  });

  websocket.on('unsubscribed', function(){
    $scope.isSubscribed = false;
  });

  $scope.isConnected = function() {
    var con = websocket.isConnected();
    if (!con) {
      $scope.isSubscribed= false;
    }
    return con;
  };
}]);
