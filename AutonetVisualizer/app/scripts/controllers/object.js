'use strict';

angular.module('autoNetApp')
.controller('ObjectCtrl', ['$scope', '$routeParams', 'LeapState', function ($scope, $routeParams, LeapState) {
  $scope.leap = LeapState;
  $scope.allObjects = LeapState.GetObjects()

  $scope.$watchCollection('allObjects', function(){
    $scope.object =   $scope.allObjects[$routeParams['linkName']];
  });

}]);
