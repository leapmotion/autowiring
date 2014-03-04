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

      var currentNodes = Object.create(null);//a map

      scope.$watchCollection('nodes', function(nodes) {
        // Add new nodes
        _.each(nodes, function(node) {
          if (typeof currentNodes[node.name] === 'undefined'){
            var label = 'Context: ' + node.name;
            var newNode = scope.graph.newNode({label:label});
            currentNodes[node.name] = newNode;
            if (typeof node.parent !== 'undefined'){
              scope.graph.newEdge(currentNodes[node.parent], newNode);
            }
          }
        });

        // Remove expired nodes
        var updateNames = _.map(nodes, function(node){
          return String(node.name);
        });
        var currentNames = _.keys(currentNodes);
        var diff = _.difference(currentNames, updateNames);
        _.each(diff, function(nodeName) {
          scope.graph.removeNode(currentNodes[nodeName]);
          delete currentNodes[nodeName];
        });

      });
    }
  };
});
