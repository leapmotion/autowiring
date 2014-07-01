'use strict';

// Handles the buisness logic for the header and subscribe/unsubscribe buttons
angular.module('autoNetApp')
.controller('HeaderCtrl', ['$scope', 'websocket', 'VERSION', 'LAST_UPDATE_DATE', function ($scope, websocket, VERSION, LAST_UPDATE_DATE) {
  $scope.isSubscribed = false;
  $scope.breakpoints = {};
  $scope.VERSION = VERSION;
  $scope.LAST_UPDATE_DATE = LAST_UPDATE_DATE;

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
