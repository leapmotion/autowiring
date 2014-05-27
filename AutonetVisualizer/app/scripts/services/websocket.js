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

  function Message(name,args) {
    this.name = name;
    this.args = args;
  };

  function Messages(maxSize){
    this.maxSize = maxSize;
    this.messages = [];
  }

  Messages.prototype.addMessage = function(name, args){
    if (this.messages.length >= this.maxSize){
      this.messages.pop();
    }
    this.messages.unshift(new Message(name,args))
  }

  // Initialize connection with AutoNetServer
  // Called with SetInterval one a second when disconnected from server
  var InitConnection = function() {
    socket = new WebSocket('ws://localhost:8000');

    socket.onmessage = function(evt) {
      var msg = JSON.parse(evt.data);
      $rootScope.$emit('leap-'+msg.type, msg.args);
    };

    socket.onopen = function() {
      clearInterval(interval);
      interval = null;
      isConnected = true;
      $rootScope.$digest();
    };

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
  var isConnected = false;
  var EventHistory = new Messages(200);

  InitConnection();

  function SendMessage(msgType) {
    var packet = {
      type: msgType,
      args: Array.prototype.slice.call(arguments, 1)
    };
    console.log(packet);
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
    subscribe: function() {
      SendMessage('subscribe')
    },
    unsubscribe: function() {
      SendMessage('unsubscribe');
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
