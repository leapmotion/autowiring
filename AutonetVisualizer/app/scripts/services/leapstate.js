'use strict';

angular.module('autoNetApp')
.factory('LeapState', function LeapState(websocket) {
  
  //This object is the "values" in $scope.contexts
  function Context(ctxt){
    this.members = [];
    this.threads = [];
    this.eventReceivers = [];
    this.bolts = {};
    this.exceptionFilters = [];
    this.name = "Unnamed";

    _.extend(this, ctxt)
  }

  var ContextMap = {}; //a map of context.id to contexts

  websocket.on('unsubscribed', function(){
    ContextMap = {};
  });

  websocket.on('newContext', function(context){
    console.log("Context");
    if (! (context.id in ContextMap)) {
      ContextMap[context.id] = new Context(context);
    } else {
      console.log('context alreadys exists');
    }
  });

  websocket.on('expiredContext', function(contextID){
    delete ContextMap[contextID];
  });

  websocket.on('newContextMember', function(contextID, member){
    var updatedContext = ContextMap[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.members, {name: member.name}))) {
      updatedContext.members.push(member);
    } else {
      console.log("Member already exists");
    }
  });

  websocket.on('newCoreThread', function(contextID, thread){
    var updatedContext = ContextMap[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.threads, {name: thread.name}))) {
      updatedContext.threads.push(thread);
    } else {
      console.log("Thread already exists");
    }
  });

  websocket.on('newEventReceiver', function(contextID, receiver){
    var updatedContext = ContextMap[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.eventReceivers, {name: receiver.name}))) {
      updatedContext.eventReceivers.push(receiver);
    } else {
      console.log("EventReceiver already exists");
    }
  });

  websocket.on('newBolt', function(contextID, serverBolt){
    var updatedContext = ContextMap[contextID];
    var bolt = updatedContext.bolts[serverBolt.name];
    if (_.isUndefined(bolt)) {
      updatedContext.bolts[serverBolt.name] = [serverBolt.listener];
    } else {
      bolt.push(serverBolt.listener);
    }
  });

  websocket.on('newExceptionFilter', function(contextID, filter){
    var updatedContext = ContextMap[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.exceptionFilters, {name: filter.name}))) {
      updatedContext.exceptionFilters.push(filter);
    } else {
      console.log("EventReceiver already exists");
    }
  });

  return {
    GetContexts: function(){
      return ContextMap;
    }
  };
});
