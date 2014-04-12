'use strict';

angular.module('autoNetApp')
.factory('LeapState', function LeapState(websocket) {

  function Context(ctxt){
    this.members = [];
    this.threads = [];
    this.eventReceivers = [];
    this.bolts = {};
    this.exceptionFilters = [];
    this.name = "Unnamed";

    _.extend(this, ctxt)
  }

  Context.prototype.numMembers = function(){
    return this.members.length +
          this.threads.length +
          this.eventReceivers.length +
          Object.keys(this.bolts).length +
          this.exceptionFilters.length
  }

  function Message(name,info) {
    this.name = name;
    this.info = info
  };
  
  function Messages(maxSize){
    this.maxSize = maxSize;
    this.messages = [];
  }

  Messages.prototype.addMessage = function(name, info){
    if (this.messages.length >= this.maxSize){
      this.messages.pop();
    }
    this.messages.unshift(new Message(name,info))
  }

  //////////////////////////////////////////////////////
  //////////////////////////////////////////////////////

  var ContextMap = {}; //a map of context.id to contexts
  var EventHistory = new Messages(200);

  websocket.on('unsubscribed', function(){
    ContextMap = {};
    EventHistory = new Messages(200);
  });

  websocket.on('newContext', function(context){
    if (! (context.id in ContextMap)) {
      ContextMap[context.id] = new Context(context);
    } else {
      console.log('context alreadys exists');
    }

    EventHistory.addMessage('newContext', context.name);
  });

  websocket.on('expiredContext', function(contextID){
    EventHistory.addMessage('expiredContext', ContextMap[contextID].name);
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

    EventHistory.addMessage('newContextMember', sprintf('%s in %s', member.name, updatedContext.name));
  });

  websocket.on('newCoreThread', function(contextID, thread){
    var updatedContext = ContextMap[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.threads, {name: thread.name}))) {
      updatedContext.threads.push(thread);
    } else {
      console.log("Thread already exists");
    }

    EventHistory.addMessage('newCoreThread', sprintf('%s in %s', thread.name, updatedContext.name));
  });

  websocket.on('newEventReceiver', function(contextID, receiver){
    var updatedContext = ContextMap[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.eventReceivers, {name: receiver.name}))) {
      updatedContext.eventReceivers.push(receiver);
    } else {
      console.log("EventReceiver already exists");
    }

    EventHistory.addMessage('newEventReceiver', sprintf('%s in %s', receiver.name, updatedContext.name));
  });

  websocket.on('newBolt', function(contextID, serverBolt){
    var updatedContext = ContextMap[contextID];
    var bolt = updatedContext.bolts[serverBolt.sigil];
    if (_.isUndefined(bolt)) {
      updatedContext.bolts[serverBolt.sigil] = [serverBolt.name];
    } else {
      bolt.push(serverBolt.name);
    }
    EventHistory.addMessage('newBolt', sprintf('%s listening for %s in %s',
                               serverBolt.name, serverBolt.sigil ,updatedContext.name));
  });

  websocket.on('newExceptionFilter', function(contextID, filter){
    var updatedContext = ContextMap[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.exceptionFilters, {name: filter.name}))) {
      updatedContext.exceptionFilters.push(filter);
    } else {
      console.log("EventReceiver already exists");
    }

    EventHistory.addMessage('newExceptionFilter', sprintf('%s in %s', filter.name, updatedContext.name))
  });

  return {
    GetContexts: function(){
      return ContextMap;
    },
    GetEventHistory: function(){
      return EventHistory.messages;
    }
  };
});
