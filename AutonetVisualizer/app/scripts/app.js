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
    .when('/edit', {
      templateUrl: 'views/edit.html',
      controller: 'EditCtrl'
    })
    .when('/raw', {
      templateUrl: 'views/raw.html',
      controller: 'RawCtrl'
    })
    .when('/filtergraph', {
      templateUrl: 'views/filtergraph.html',
      controller: 'FilterGraphCtrl'
    })
    .when('/object/:linkName', {
      templateUrl: 'views/object.html',
      controller: 'ObjectCtrl'
    })
    .otherwise({
      redirectTo: '/main'
    });
}]);
