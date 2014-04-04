'use strict';

describe('Directive: DirectedGraph', function () {

  // load the directive's module
  beforeEach(module('autoNetApp'));

  var element,
    scope;

  beforeEach(inject(function ($rootScope) {
    scope = $rootScope.$new();
  }));

  it('should make hidden element visible', inject(function ($compile) {
    element = angular.element('<-directed-graph></-directed-graph>');
    element = $compile(element)(scope);
    expect(element.text()).toBe('this is the DirectedGraph directive');
  }));
});
