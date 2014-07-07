'use strict';

/* "<directed-graph nodes="MyNodes"></directed-graph>" creates a canvas with a
** force-directed graph of nodes. "MyNodes" needs to be an array of objects. Each
** object must contain an "id" and "parent" field. "id" is a unique int identifier
** for the node and "parent" is the idenifier for the parent node
*/
angular.module('autoNetApp')
.directive('directedTree', function () {
  return {
    template: '<canvas width="1000" height="400">HTML5 canvas not supported on your browser</canvas>',
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

// Similar to directedTree, but for arbitrary graphs
// Specialized for the context view

// Usage:
// <directed-graph  nodes="allObjects" id="name" children="slots" unidentified="unidentified" orphans="orphans">
// </directed-graph>

// "allObject" is a list of node objects
// "name" is the property by which to uniqly identify objects in "allObjects"
// "childen" is the property which contains a list of childen identifiers

// "unidentified" are "orphans" are outputs, which fill the past in list with extra information.
// see decription below
angular.module('autoNetApp')
.directive('directedGraph', function () {
  return {
    template: '<canvas width="1000" height="600">HTML5 canvas not supported on your browser</canvas>',
    restrict: 'E',
    replace: true,
    scope: {
      nodes: '=', //A list of node objects. 
      orphans: '=', // A list of nodes that are not connected to the main graph
      unidentified: '=', // A list of node ids that were children of a node, but not in 'Nodes'
      id: '@', // key of objects in 'nodes' to use as unique identifier
      children: '@' // key of objects in 'nodes' that contains list of 'id's of child nodes
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

        // List of slots that arn't known by visualizer
        scope.unidentified = [];
        // List of context members not in slot graph
        scope.orphans = [];

        _.each(nodeMap, function(node, nodeID){
          // Add node if doesn't exist
          if (!nodeSet.hasOwnProperty(nodeID) && node[scope.children] && node[scope.children].length) {
            graph.addNode(new Springy.Node(node[scope.id], {label:node.name}) );
          }

          // Add edges, and endpoint node if doesn't exist
          _.each(node[scope.children], function(slot){
            var slotID = slot[scope.id];

            if (nodeMap.hasOwnProperty(slotID)) {
              if ( !nodeSet.hasOwnProperty(slotID)) {
                var newNode = nodeMap[slotID];
                graph.addNode(new Springy.Node(newNode[scope.id], {label:newNode.name, mass:0.5}) );
              }

              // Determine edge drawing style
              var edgeData;
              if (slot.autoRequired){
                edgeData = {length: 3.0, label:"AutoRequired", color:'#FF3333'}
              } else {
                edgeData = {length: 3.0, label:"Autowired", color:'#458B00'}
              }
              graph.newEdge(nodeSet[nodeID], nodeSet[slotID], edgeData);
            } else {
              scope.unidentified.push(slotID);
            }
          }); // each slot
        }); //each nodeMap 

        // Populate list of orphan nodes (not shown in graph)
        var orphanNodes = {};
        _.extend(orphanNodes, nodeMap);
        graph.edges.forEach(function(edge){
          delete orphanNodes[edge.source.id];
          delete orphanNodes[edge.target.id];
        });
        scope.orphans = _.pluck(orphanNodes, scope.id);
        scope.unidentified = _.uniq(scope.unidentified);
      });
    }
  };
});
