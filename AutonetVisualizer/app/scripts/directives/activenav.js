'use strict';

angular.module('autoNetApp')
  .directive('routeNav', function () {
    return {
      restrict: 'A',
      scope: {},
      link: function postLink(scope, element, attrs) {
        scope.$on('$routeChangeSuccess', function(event, current, previous){
          var path = attrs.routeNav;
          element.toggleClass('active',path === current.$$route.originalPath);
        });
      }
    };
  });

angular.module('autoNetApp')
  .directive('activeNav', function () {
    return {
      restrict: 'A',
      scope: {
        check: '=check'
      },
      link: function postLink(scope, element, attrs) {
        scope.$watch('check', function(newValue, oldValue){
          element.toggleClass('active', attrs.activeNav === newValue);
        });

        element.on('click', function(evt){
          evt.stopImmediatePropagation();
          evt.preventDefault();

          console.log('click');
          scope.$apply(function(){
            scope.check = attrs.activeNav;
          });
        })
      }
    };
  });
