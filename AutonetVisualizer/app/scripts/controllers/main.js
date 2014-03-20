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
    context.name = context.name || context.id;
    $scope.contexts.push(context);
  });

  websocket.on('expiredContext', function(context){
    $scope.contexts = _.reject($scope.contexts, function(ctxt){
      return ctxt.id === context.id;
    });
  });

  websocket.on('updateContext', function(context){
    var toUpdate = _.findWhere($scope.contexts, {name: context.name});
    _.extend(toUpdate, context);
  });

  websocket.on('newContextMember', function(context, member){
    console.log('newContextMember!');
    console.log(context);
    console.log(member);
  });

}]);
