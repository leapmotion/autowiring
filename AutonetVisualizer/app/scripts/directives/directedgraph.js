'use strict';

angular.module('autoNetApp')
.directive('directedGraph', function () {
  return {
    template: '<canvas/ width=1000 height=400>',
    restrict: 'E',
    replace: true,
    scope: {
      nodes: '='
    },
    controller: function($scope) {
      $scope.graph = new Springy.Graph();
    },
    link: function(scope, element, attrs) {
      // Register canvas with jQuery
      jQuery(function(){
        jQuery(element).springy({
          graph: scope.graph
        })
      });

      // Map of current Nodes
      var currentNodes = Object.create(null);

      // Watch when nodes change
      scope.$watchCollection('nodes', function(nodes) {
        // Add new nodes
        _.each(nodes, function(node) {
          if (typeof currentNodes[node.id] === 'undefined'){
            var label = 'Context: ' + node.name;
            var newNode = scope.graph.newNode({label:label});
            currentNodes[node.id] = newNode;
            if (typeof node.parent !== 'undefined'){
              scope.graph.newEdge(currentNodes[node.parent], newNode);
            }
          }
        });

        // Remove expired nodes
        var updateIds = _.map(nodes, function(node){
          return String(node.id);
        });
        var currentIds = _.keys(currentNodes);
        var diff = _.difference(currentIds, updateIds);
        _.each(diff, function(nodeId) {
          scope.graph.removeNode(currentNodes[nodeId]);
          delete currentNodes[nodeId];
        });

      });
    }
  };
});
