'use strict';

angular.module('autoNetApp')
.factory('LeapState', function LeapState(websocket) {

  // Object definitions
  function Context(ctxt){
    this.objects = [];
    this.name = "Unnamed";

    _.extend(this, ctxt)
  }

  Context.prototype.addObject = function(types, objData) {
    this.objects.push(new LeapObject(types, objData));
  };

  function LeapObject(types, objData){
    this.name = objData.name;
    this.types = _.without(Object.keys(types), 'bolt');

    if (types.hasOwnProperty("bolt")) {
      this.boltSigils = types.bolt;
      if (this.boltSigils.length === 0){
        this.boltSigils.push("Everything!");
      }
    }
  }

  LeapObject.prototype.IsType = function(type){
    if (type === 'bolt'){
      return this.hasBolts();
    }

    return _.contains(this.types, type);
  };

  LeapObject.prototype.IsAnyType = function(filterTypes){
    if (this.hasBolts() && filterTypes.hasOwnProperty('bolt')) return true;

    for (var i = 0; i<this.types.length; i++) {
      var type = this.types[i];

      if (filterTypes.hasOwnProperty(type)) return true;
    }
    return false;
  };

  LeapObject.prototype.hasBolts = function(){
    return this.hasOwnProperty('boltSigils');
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

  websocket.on('newContext', function(context){
    if (! (context.id in ContextMap)) {
      ContextMap[context.id] = new Context(context);
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
  });

  return {
    GetContexts: function(){
      return ContextMap;
    },
    isSubscribed: function(){
      return isSubscribed;
    }
  };
});
