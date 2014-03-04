'use strict';

angular.module('autoNetApp')
.directive('directedGraph', function () {
  return {
    template: '<canvas id="springygraph" width="800" height="360" />',
    restrict: 'E',
    replace: true,
    scope: {
      nodes: '='
    },
    controller: function($scope) {
      $scope.graph = new Springy.Graph();
    },
    link: function(scope, element, attrs) {
      jQuery(function(){
        var springy = window.springy = jQuery('#springygraph').springy({
          graph: scope.graph
        });
      });

      var currentNodes = {};

      scope.$watchCollection('nodes', function(nodes){
        _.each(nodes, function(node) {
          console.log(node);
          if (currentNodes[node.name] === undefined){
            var label = "Context: " + node.name;
            var newNode = scope.graph.newNode({label:label});
            currentNodes[node.name] = newNode;
            if (node.parent !== undefined){
              scope.graph.newEdge(currentNodes[node.parent], newNode);
            }
          }
        });

      });
    }
  };
});
