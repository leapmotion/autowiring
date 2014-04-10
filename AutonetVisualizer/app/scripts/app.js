'use strict';

angular.module('autoNetApp', [
  'ngRoute'
])
.config(['$routeProvider', function ($routeProvider) {
  $routeProvider
    .when('/main', {
      templateUrl: 'views/main.html',
      controller: 'MainCtrl'
    })
    .when('/raw', {
      templateUrl: 'views/raw.html',
      controller: 'RawCtrl'
    })
    .when('/filtergraph', {
      templateUrl: 'views/filtergraph.html',
      controller: 'FilterGraphCtrl'
    })
    .otherwise({
      redirectTo: '/main'
    });
}]);
