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
** Events are broadcast from $rootScope using angular's event system. Event
** names are namespaced internally using the "leap-" prefix
**
** See controllers/main.js for an example
*/

angular.module('autoNetApp')
.factory('websocket', ['$rootScope', 'WEBSOCKET_PORT', function($rootScope, WEBSOCKET_PORT) {

  // A single event from the server
  function Message(name,args) {
    this.name = name;
    this.args = args;
  };

  // A collection of Message's
  function Messages(maxSize){
    this.maxSize = maxSize;
    this.messages = [];
  }

  // Add message but remove oldest entry if max size
  Messages.prototype.addMessage = function(name, args){
    if (this.messages.length >= this.maxSize){
      this.messages.pop();
    }
    this.messages.unshift(new Message(name,args))
  }

  // Initialize connection with AutoNetServer
  // Called with SetInterval one a second when disconnected from server
  var InitConnection = function() {
    socket = new WebSocket('ws://localhost:' + WEBSOCKET_PORT);

    // emit a angular event when receive a server event
    socket.onmessage = function(evt) {
      var msg = JSON.parse(evt.data);
      $rootScope.$emit('leap-'+msg.type, msg.args);
    };

    // Stop polling for websocket server
    socket.onopen = function() {
      clearInterval(interval);
      interval = null;
      isConnected = true;
      $rootScope.$digest();
    };

    // Resume polling for websocket server
    socket.onclose = function() {
      console.log('close')
      isConnected = false;
      $rootScope.$digest();
      $rootScope.$emit('leap-unsubscribed');
      if (interval === null) {
        interval = setInterval(InitConnection, 1000);
      }
    };
  };

  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////

  var socket = null; //Websocket connection to server
  var interval = null; //Interval for polling for server when disconnected
  var isConnected = false; //State variable for status of connection
  var EventHistory = new Messages(200); // A list of the past 200 events received from the server

  // Attempt to connect to websocket server, poll if can't connect
  InitConnection();

  // Sends a message from the client back to the server
  function SendMessage(msgType) {
    var packet = {
      type: msgType,
      args: Array.prototype.slice.call(arguments, 1)
    };
    socket.send(JSON.stringify(packet));
  }

  return {
    on: function(eventName, callback) {
      console.log('Event Registered: ', eventName);
      $rootScope.$on('leap-'+eventName, function(event, args){
        $rootScope.$apply(function(){
          EventHistory.addMessage(eventName, args);
          callback.apply(socket, args);
        });
      });
    },
    isConnected: function(){
      return isConnected;
    },
    GetEventHistory: function(){
      return EventHistory.messages;
    },
    SendMessage: SendMessage
  };
}]);
