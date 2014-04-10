'use strict';

describe('Controller: FiltergraphCtrl', function () {

  // load the controller's module
  beforeEach(module('autoNetApp'));

  var FiltergraphCtrl,
    scope;

  // Initialize the controller and a mock scope
  beforeEach(inject(function ($controller, $rootScope) {
    scope = $rootScope.$new();
    FiltergraphCtrl = $controller('FiltergraphCtrl', {
      $scope: scope
    });
  }));

  it('should attach a list of awesomeThings to the scope', function () {
    expect(scope.awesomeThings.length).toBe(3);
  });
});
