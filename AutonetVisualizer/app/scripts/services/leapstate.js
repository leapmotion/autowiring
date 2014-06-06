'use strict';

angular.module('autoNetApp')
.factory('LeapState', ['websocket', function LeapState(websocket) {

  // Object definitions
  function Context(id, ctxt){
    this.objects = {}; //linkName to Object
    this.name = "Unnamed";
    this.id = id;
    this.children = [];

    _.extend(this, ctxt);

    this.linkName = this.name.replace(/\s+/g,'_'); //no whitespace
  }

  Context.prototype.addObject = function(obj) {
    this.objects[obj.linkName] = obj;
  };

  function LeapObject(types, objData){
    this.name = objData.name;
    this.linkName = this.name.replace(/\s+/g,'_'); //no whitespace
    this.types = Object.keys(types);
    this.slots = objData.slots;

    // Add bolt information
    if (types.hasOwnProperty("bolt")) {
      this.boltSigils = types.bolt;
      if (this.boltSigils.length === 0){
        this.boltSigils.push("Everything!");
      }
    }

    // Add EventReceiver information
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
  var TypeList = [];
  var isSubscribed = false;

  websocket.on('subscribed', function(allTypes) {
    isSubscribed = true;
    TypeList = allTypes;
  });

  websocket.on('unsubscribed', function(){
    ContextMap = {};
    TypeList = [];
    isSubscribed = false;
  });

  websocket.on('newContext', function(contextID, context){
    if (!ContextMap.hasOwnProperty(contextID)) {
      var newCtxt = new Context(contextID, context);
      ContextMap[contextID] = newCtxt;

      // Add this to childen of parent context
      if (ContextMap.hasOwnProperty(newCtxt.parent)) {
        ContextMap[newCtxt.parent].children.push(contextID);
      }
    } else {
      console.log('context alreadys exists');
    }
  });

  websocket.on('expiredContext', function(contextID){
    var parent = ContextMap[ContextMap[contextID].parent];
    var index = parent.children.indexOf(contextID);
    parent.children.splice(index,1);

    delete ContextMap[contextID];
  });

  websocket.on('newObject', function(contextID, types, objData) {
    var updatedContext = ContextMap[contextID];
    var object = new LeapObject(types, objData);

    updatedContext.addObject(object);
  });

  websocket.on('eventFired', function(contextID, eventHash){

    function fireEvent(ctxtID) {
      if (!ContextMap.hasOwnProperty(ctxtID)) return;

      var updatedContext = ContextMap[ctxtID];

      // Iterated objects in this context, signal event fired
      Object.keys(updatedContext.objects).forEach(function(linkName){
        var obj = updatedContext.objects[linkName];
        obj.eventFired(eventHash.name);
      });

      // recurse on children
      for (var i = 0; i < updatedContext.children.length; i++) {
        fireEvent(updatedContext.children[i]);
      }
    }

    fireEvent(contextID);
  });

  return {
    GetContexts: function(){
      return ContextMap;
    },
    isSubscribed: function(){
      return isSubscribed;
    },
    GetAllTypes: function(){
      return TypeList;
    }
  };
}]);
