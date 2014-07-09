'use strict';

// Handles the buisness logic for the header and subscribe/unsubscribe buttons
angular.module('autoNetApp')
.controller('HeaderCtrl', ['$scope', 'websocket', 'info', function ($scope, websocket, info) {
  $scope.isSubscribed = false;
  $scope.breakpoints = {};
  $scope.info = info;

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
    $scope.$digest();
  });

  websocket.on('unsubscribed', function(){
    $scope.isSubscribed = false;
    $scope.$digest();
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

  //Breakpoint stuff
  function Breakpoint(name) {
    this.name = name;
  }

  websocket.on('breakpoint', function(name){
    $scope.breakpoints[name] = new Breakpoint(name);
  });

  $scope.resumeFromBreakpoint = function(name) {
    websocket.SendMessage('resumeFromBreakpoint', name);
    delete $scope.breakpoints[name];
  }
}]);
