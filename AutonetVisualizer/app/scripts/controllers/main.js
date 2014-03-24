'use strict';

angular.module('autoNetApp')
.controller('MainCtrl', ['$scope', 'websocket', function ($scope, websocket) {
  $scope.contexts = {}; //a map of context.id to contexts


  //This object is the "values" in $scope.contexts
  function Context(ctxt){
    this.members = [];
    this.name = "Unnamed";

    _.extend(this, ctxt)
  }

  websocket.on('unsubscribed', function(){
    $scope.contexts = {};
  });

  websocket.on('newContext', function(context){
    var existingContext = $scope.contexts[context.id];
    if (_.isUndefined(existingContext)) {
      $scope.contexts[context.id] = new Context(context);
    } else {
      _.extend(existingContext,context);
    }
  });

  websocket.on('expiredContext', function(context){
    delete $scope.contexts[context.id];
  });

  websocket.on('newContextMember', function(context, member){
    var toUpdate = $scope.contexts[context.id];
    if (_.isUndefined(toUpdate)) {
      toUpdate = $scope.contexts[context.id] = new Context(context);
    }
    // only add if doesn't already exist
    if (_.isUndefined(_.findWhere(toUpdate.members, {name: member.name}))) {
      toUpdate.members.push(member);
    }
  });

}]);
