'use strict';

/**
 * Provides information reguarding this build of the client
 */
angular.module('autoNetApp')
.factory('info', ['$http', function($http) {

  // Convert date to desired string format
  function parseDate(date) {
    date = new Date(date);
    var year = date.getUTCFullYear();
    var month = "0" + (date.getUTCMonth() + 1)
    month.slice(-2);
    var day = "0" + date.getUTCDate()
    day.slice(-2)
    return year + '-' + month + '-' + day;
  }

  // Default values in our info object
  var info = {
    version: '0.0.0',
    deploy_date: parseDate(new Date())
  };

  // Get version from server
  $http.get('/api/info/version').success(function(data){
    info.version = JSON.parse(data);
  }).error(function(data){
    console.log("Version error!");
  })

  // Get deploy date from serfer
  $http.get('/api/info/deploy_date').success(function(data){
    info.deploy_date = parseDate(JSON.parse(data));
  }).error(function(data){
    console.log("deploy_date error!");
  })

  return info;
}]);