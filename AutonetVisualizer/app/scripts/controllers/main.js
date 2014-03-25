'use strict';

angular.module('autoNetApp')
.controller('MainCtrl', ['$scope', 'websocket', function ($scope, websocket) {
  $scope.contexts = {}; //a map of context.id to contexts


  //This object is the "values" in $scope.contexts
  function Context(ctxt){
    this.members = [];
    this.threads = [];
    this.eventReceivers = [];
    this.bolts = [];
    this.exceptionFilters = [];
    this.name = "Unnamed";

    _.extend(this, ctxt)
  }

  websocket.on('unsubscribed', function(){
    $scope.contexts = {};
  });

  websocket.on('newContext', function(context){
    if (!_.has($scope.contexts, context.id)) {
      $scope.contexts[context.id] = new Context(context);
    }
  });

  websocket.on('expiredContext', function(contextID){
    delete $scope.contexts[contextID];
  });

  websocket.on('newContextMember', function(contextID, member){
    var updatedContext = $scope.contexts[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.members, {name: member.name}))) {
      updatedContext.members.push(member);
    } else {
      console.log("Member already exists");
    }
  });

  websocket.on('newCoreThread', function(contextID, thread){
    var updatedContext = $scope.contexts[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.threads, {name: thread.name}))) {
      updatedContext.threads.push(thread);
    } else {
      console.log("Thread already exists");
    }
  });

  websocket.on('newEventReceiver', function(contextID, receiver){
    var updatedContext = $scope.contexts[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.eventReceivers, {name: receiver.name}))) {
      updatedContext.eventReceivers.push(receiver);
    } else {
      console.log("EventReceiver already exists");
    }
  });

  websocket.on('newBolt', function(contextID, bolt){
    var updatedContext = $scope.contexts[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.bolts, {name: bolt.name}))) {
      updatedContext.bolts.push(bolt);
    } else {
      console.log("Bolt already exists");
    }
  });

  websocket.on('newExceptionFilter', function(contextID, filter){
    var updatedContext = $scope.contexts[contextID];

    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(updatedContext.exceptionFilters, {name: filter.name}))) {
      updatedContext.exceptionFilters.push(filter);
    } else {
      console.log("EventReceiver already exists");
    }
  });

}]);
