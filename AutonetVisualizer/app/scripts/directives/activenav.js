'use strict';
// This file contains a couple navigation directives with slightly differing functionality. Probobly
// can be combined into a more generic directive


// adds class "active" to a tab if the attribute value is the same as the path
// See the header view for an example

//<ul class="nav nav-pills pull-right">
//  <li route-nav="/main"><a ng-href="#/main">Overview</a></li>
//  <li route-nav="/edit"><a ng-href="#/edit">Edit</a></li>
//  <li route-nav="/raw"><a ng-href="#/raw">Raw</a></li>
//</ul>
angular.module('autoNetApp')
  .directive('routeNav', ['$location', function ($location) {
    return {
      restrict: 'A',
      scope: {},
      link: function postLink(scope, element, attrs) {
        element.toggleClass('active', attrs.routeNav === $location.path());
        scope.$on('$routeChangeSuccess', function(event, current, previous){
          element.toggleClass('active', attrs.routeNav === $location.path());
        });
      }
    };
  }]);

// A simple tab directive. Adds the class "active" to the currently selected tab
// The state variable for the selected tab is passed in by the attribute "check"

//<ul class="nav nav-pills pull-right">
//  <li active-nav="contextMember" check="currentFilter"><a>ContextMembers</a></li>
//  <li active-nav="coreRunnable" check="currentFilter"><a>CoreRunnables</a></li>
//  <li active-nav="eventReceiver" check="currentFilter"><a>EventReceivers</a></li>
//</ul>
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

          scope.$apply(function(){
            scope.check = attrs.activeNav;
          });
        })
      }
    };
  });

// A tab like directve that allows multiple "tabs" to be toggled indiciduality
// A map of "active" tabs is passed in by the attribute "check"

//<ul class="nav nav-pills pull-right">
//  <li active-nav="contextMember" check="currentFilters"><a>ContextMembers</a></li>
//  <li active-nav="coreRunnable" check="currentFilters"><a>CoreRunnables</a></li>
//  <li active-nav="eventReceiver" check="currentFilters"><a>EventReceivers</a></li>
//</ul>
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
