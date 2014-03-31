'use strict';

/* "<directed-graph nodes="MyNodes"></directed-graph>" creates a canvas with a
** force-directed graph of nodes. "MyNodes" needs to be an array of objects. Each
** object must contain an "id" and "parent" field. "id" is a unique int identifier
** for the node and "parent" is the idenifier for the parent node
*/
angular.module('autoNetApp')
.directive('directedGraph', function () {
  return {
    template: '<canvas width="900" height="400">HTML5 canvas not supported on your browser</canvas>',
    restrict: 'E',
    replace: true,
    scope: {
      nodes: '='
    },
    controller: function($scope) {
      $scope.graph = new Springy.Graph();
    },
    link: function(scope, element, attrs) {
      // Register canvas with jQuery/springy
      jQuery(function(){
        jQuery(element).springy({
          graph: scope.graph
        })
      });

      // Create a map to keep track of current Nodes
      var currentNodes = Object.create(null);

      // Watch for any new or deleted nodes
      scope.$watch('nodes', function(nodeMap) {
        var nodes = _.values(nodeMap);
        // Add any new nodes
        _.each(nodes, function(node) {
          if (typeof currentNodes[node.id] === 'undefined'){
            var label = node.name;
            var newNode = scope.graph.newNode({label:label});
            currentNodes[node.id] = newNode;
            if (typeof currentNodes[node.parent] !== 'undefined'){
              scope.graph.newEdge(currentNodes[node.parent], newNode);
            }
          }
        });

        // Remove any expired nodes
        var updateIds = _.map(nodes, function(node){
          return String(node.id);
        });
        var currentIds = _.keys(currentNodes);
        var diff = _.difference(currentIds, updateIds);
        _.each(diff, function(nodeId) {
          scope.graph.removeNode(currentNodes[nodeId]);
          delete currentNodes[nodeId];
        });

      }, true);
    }
  };
});
