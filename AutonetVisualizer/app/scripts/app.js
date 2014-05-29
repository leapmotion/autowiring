'use strict';

angular.module('autoNetApp', [
  'ngAnimate',
  'ngRoute',
  'ngSanitize'
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
    .when('/object/:contextID/:linkName', {
      templateUrl: 'views/object.html',
      controller: 'ObjectCtrl'
    })
    .when('/context/:contextID', {
      templateUrl: 'views/context.html',
      controller: 'ContextCtrl'
    })
    .otherwise({
      redirectTo: '/main'
    });
}]);
