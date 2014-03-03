'use strict';

angular.module('autoNetApp')
.factory('websocket', ['$rootScope', function($rootScope) {
  var socket = null;
  var interval = null;
  var listeners = [];
  var isConnected = false;

  var InitConnection = function() {
    socket = new WebSocket('ws://localhost:8000');

    socket.onmessage = function(evt) {
      var msg = JSON.parse(evt.data);
      _.map(listeners, function(listener){
        if (listener.type === msg.type) {
          $rootScope.$apply(function(){
            listener.callback.apply(socket, msg.args);
          });
        }
      });
    };

    socket.onopen = function() {
      clearInterval(interval);
      interval = null;
      console.log('open');
      isConnected = true;
      $rootScope.$digest();
    };

    socket.onclose = function() {
      console.log('close');
      isConnected = false;
      $rootScope.$digest();
      if (interval === null){
        interval = setInterval(InitConnection, 1000);
      }
    };
  };

  InitConnection();

  return {
    on: function(eventName, callback) {
      listeners.push({
        type: eventName,
        callback: callback
      });
    },
    subscribe: function() {
      socket.send('subscribe');
    },
    unsubscribe: function() {
      socket.send('unsubscribe');
    },
    isConnected: function(){
      return isConnected;
    }
  };
}]);
