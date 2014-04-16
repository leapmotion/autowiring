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

angular.module('autoNetApp')
  .directive('activeFilter', function () {
    return {
      restrict: 'A',
      scope: {
        check: '=check'
      },
      link: function postLink(scope, element, attrs) {
        scope.$watchCollection('check', function(newValue){
          element.toggleClass('active', attrs.activeFilter in newValue);
        });

        element.on('click', function(evt){
          evt.stopImmediatePropagation();
          evt.preventDefault();

          scope.$apply(function(){
            if (element.hasClass('active')) {
              delete scope.check[attrs.activeFilter];
            } else {
              scope.check[attrs.activeFilter] = true;
            }
          });
        });

        element.on('click', function(evt){
          evt.stopImmediatePropagation();
          evt.preventDefault();

          scope.$apply(function(){
            if (element.hasClass('active')) {
              delete scope.check[attrs.activeFilter];
            } else {
              scope.check[attrs.activeFilter] = true;
            }
          });
        });
      }
    };
  });
