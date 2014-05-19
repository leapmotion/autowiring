'use strict';

angular.module('autoNetApp')
.factory('LeapState', ['websocket', function LeapState(websocket) {

  // Object definitions
  function Context(id, ctxt){
    this.objects = [];
    this.name = "Unnamed";
    this.id = id;

    _.extend(this, ctxt)
  }

  Context.prototype.addObject = function(types, objData) {
    this.objects.push(new LeapObject(types, objData));
  };

  function LeapObject(types, objData){
    this.name = objData.name;
    this.types = Object.keys(types);

    if (types.hasOwnProperty("bolt")) {
      this.boltSigils = types.bolt;
      if (this.boltSigils.length === 0){
        this.boltSigils.push("Everything!");
      }
    }

    if (types.hasOwnProperty("eventReceiver")) {
      this.events = {};
      var numEvents = types.eventReceiver.length;

      for (var evt=0; evt<numEvents; evt++) {
        var eventType = types.eventReceiver[evt];
        this.events[eventType] = 0;
      }

      if (numEvents === 0){
        this.events["Nothing!"] = 0;
      }
    }
  }

  LeapObject.prototype.IsType = function(type){
    return _.contains(this.types, type);
  };

  LeapObject.prototype.IsAnyType = function(filterTypes){
    for (var i = 0; i<this.types.length; i++) {
      var type = this.types[i];

      if (filterTypes.hasOwnProperty(type)) return true;
    }
    return false;
  };

  LeapObject.prototype.hasBolts = function(){
    return this.hasOwnProperty('boltSigils');
  }

  LeapObject.prototype.isEventReceiver = function(){
    return this.hasOwnProperty('events');
  }

  LeapObject.prototype.eventFired = function(evt){
    if (!this.hasOwnProperty('events')) return;

    if (this.events.hasOwnProperty(evt)) {
      this.events[evt]++;
    }
  }

  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////

  var ContextMap = {}; //a map of context.id to contexts
  var isSubscribed = false;

  websocket.on('subscribed', function(){
    isSubscribed = true;
  })

  websocket.on('unsubscribed', function(){
    ContextMap = {};
    isSubscribed = false;
  });

  websocket.on('newContext', function(contextID, context){
    if (!ContextMap.hasOwnProperty(contextID)) {
      ContextMap[contextID] = new Context(contextID, context);
    } else {
      console.log('context alreadys exists');
    }
  });

  websocket.on('expiredContext', function(contextID){
    delete ContextMap[contextID];
  });

  websocket.on('newObject', function(contextID, types, objData) {
    var updatedContext = ContextMap[contextID];

    updatedContext.addObject(types, objData);
  });

  websocket.on('eventFired', function(contextID, eventHash){
    var updatedContext = ContextMap[contextID];

    for (var i = 0; i<updatedContext.objects.length; i++) {
      var obj = updatedContext.objects[i];
      obj.eventFired(eventHash.name);
    }
  });

  return {
    GetContexts: function(){
      return ContextMap;
    },
    isSubscribed: function(){
      return isSubscribed;
    }
  };
}]);
