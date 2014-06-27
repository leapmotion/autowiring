'use strict';

// Handles the buisness logic for the header and subscribe/unsubscribe buttons
angular.module('autoNetApp')
.controller('HeaderCtrl', ['$scope', 'websocket', function ($scope, websocket) {
  $scope.isSubscribed = false;

  function subscribe() {
    if (websocket.isConnected()){
      websocket.SendMessage("subscribe");
    }
  };

  function unsubscribe() {
    if (websocket.isConnected()){
      websocket.SendMessage("unsubscribe");
    }
  };

  websocket.on('subscribed', function(){
    $scope.isSubscribed = true;
  });

  websocket.on('unsubscribed', function(){
    $scope.isSubscribed = false;
  });

  $scope.toggleSubscription = function(){
    if ($scope.isSubscribed){
      unsubscribe();
    } else {
      subscribe();
    }
  }

  $scope.isConnected = function() {
    var con = websocket.isConnected();
    if (!con) {
      $scope.isSubscribed= false;
    }
    return con;
  };
}]);
