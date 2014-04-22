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

      var graph = scope.graph;

      // Set of current nodes in graph
      var nodeSet = scope.graph.nodeSet;

      // Watch for any new or deleted nodes
      scope.$watchCollection('nodes', function(nodeMap) {
        var nodeIDs = _.keys(nodeMap);
        var currentNodeIds = _.keys(nodeSet)
        var newNodes = _.difference(nodeIDs, currentNodeIds);
        var expiredNodes = _.difference(currentNodeIds, nodeIDs);

        // Add any new nodes
        _.each(newNodes, function(nodeID) {
          var node = nodeMap[nodeID];
          graph.addNode(new Springy.Node(node.id, {label:node.name}) );;
        });

        // Add any new edges
        _.each(newNodes, function(nodeID) {
          var node = nodeMap[nodeID];
          if (node.hasOwnProperty('parent')){
            graph.newEdge(nodeSet[node.parent], nodeSet[node.id]);
          }
        });

        // Remove any expired nodes
        _.each(expiredNodes, function(nodeID) {
          graph.removeNode({id: Number(nodeID)});
        });
      });
    }
  };
});
