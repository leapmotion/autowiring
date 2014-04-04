'use strict';

/* Angular service for receiving events from AutoNetServer
** Use as follows..
**
** websocket.on("MyEvent",function(arg1){
**   ....
** })
**
** This will register a callback for the "MyEvent" event from the server.
** The number args for the callback must match the number of args for the
** event type
**
** See controllers/main.js for an example
*/

angular.module('autoNetApp')
.factory('websocket', ['$rootScope', function($rootScope) {
  var socket = null; //Websocket connection to server
  var interval = null; //Interval for polling for server when disconnected
  var listeners = []; // All event listeners
  var isConnected = false;

  // Fire event callback with list of args from the server
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

  // Initialize connection with AutoNetServer
  // Called with SetInterval one a second when disconnected from server
  var InitConnection = function() {
    socket = new WebSocket('ws://localhost:8000');

    socket.onmessage = function(evt) {
      var msg = JSON.parse(evt.data);
      FireEvent(msg.type, msg.args);
    };

    socket.onopen = function() {
      clearInterval(interval);
      interval = null;
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
      console.log('Event Registered: ', eventName);
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
