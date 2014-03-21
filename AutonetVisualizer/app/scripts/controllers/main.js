'use strict';

angular.module('autoNetApp')
.controller('MainCtrl', ['$scope', 'websocket', function ($scope, websocket) {
  $scope.contexts = [];
  $scope.objects = [];

  websocket.on('unsubscribed', function(){
    $scope.contexts = [];
    $scope.objects = [];
  });

  websocket.on('newContext', function(context){
    var existingContext = _.findWhere($scope.contexts, {id: context.id});
    if (typeof existingContext === 'undefined') {
      context.name = context.name || context.id;
      context.members = [];
      $scope.contexts.push(context);
    } else {
      _.extend(existingContext,context);
    }
  });

  websocket.on('expiredContext', function(context){
    $scope.contexts = _.reject($scope.contexts, function(ctxt){
      return ctxt.id === context.id;
    });
  });

  websocket.on('newContextMember', function(context, member){
    var toUpdate = _.findWhere($scope.contexts, {id: context.id});
    if (typeof toUpdate === 'undefined'){
      var newContext = {};
      _.extend(newContext, context);
      newContext.members = [];
      
      $scope.contexts.push(newContext);
      toUpdate = newContext;
    }
    if (_.isUndefined(_.findWhere(toUpdate.members, {name: member.name}))) {
      toUpdate.members.push(member);
    }
  });

}]);
