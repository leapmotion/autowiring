'use strict';

//Controller for the "Context" view. See app.js for details
angular.module('autoNetApp')
.controller('ContextCtrl', ['$scope', 'LeapState', '$routeParams', function ($scope, LeapState, $routeParams) {
  $scope.leap = LeapState;
  $scope.allContexts = LeapState.GetContexts();

  $scope.$watchCollection('allContexts', function(){
    $scope.context = $scope.allContexts[$routeParams['contextID']];
    $scope.allObjects = {}

    // Get all objects from parent contexts as well
    var currentContext = $scope.context;
    while (!_.isUndefined(currentContext)) {
      _.extend($scope.allObjects, currentContext.objects);
      currentContext = $scope.allContexts[currentContext.parent];
    }
  });


}]);
