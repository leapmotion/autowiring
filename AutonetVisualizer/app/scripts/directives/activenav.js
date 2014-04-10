'use strict';

angular.module('autoNetApp')
  .directive('activeNav', function () {
    return {
      restrict: 'A',
      scope: {},
      link: function postLink(scope, element, attrs) {
        scope.$on('$routeChangeSuccess', function(event, current, previous){
          var path = attrs.activeNav;
          element.toggleClass('active',path === current.$$route.originalPath);
        });
      }
    };
  });
