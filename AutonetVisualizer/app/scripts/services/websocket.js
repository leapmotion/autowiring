'use strict';

angular.module('autoNetApp')
.factory('websocket', ['$rootScope', function($rootScope) {
  var socket = null;
  var interval = null;
  var listeners = [];
  var isConnected = false;

  var FireEvent = function(event, args) {
    args = _.isArray(args) ? args : [];
    _.map(listeners, function(listener){
      if (listener.type === event) {
        $rootScope.$apply(function(){
          listener.callback.apply(socket, args);
        });
      }
    });
  };

  var InitConnection = function() {
    socket = new WebSocket('ws://localhost:8000');

    socket.onmessage = function(evt) {
      var msg = JSON.parse(evt.data);
      FireEvent(msg.type, msg.args);
    };

    socket.onopen = function() {
      clearInterval(interval);
      interval = null;
      console.log('open');
      isConnected = true;
      $rootScope.$digest();
    };

    socket.onclose = function() {
      isConnected = false;
      $rootScope.$digest();
      FireEvent('unsubscribed');
      if (interval === null) {
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
