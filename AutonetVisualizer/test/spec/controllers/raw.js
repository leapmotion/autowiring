'use strict';

describe('Controller: RawCtrl', function () {

  // load the controller's module
  beforeEach(module('autoNetApp'));

  var RawCtrl,
    scope;

  // Initialize the controller and a mock scope
  beforeEach(inject(function ($controller, $rootScope) {
    scope = $rootScope.$new();
    RawCtrl = $controller('RawCtrl', {
      $scope: scope
    });
  }));

  it('should attach a list of awesomeThings to the scope', function () {
    expect(scope.awesomeThings.length).toBe(3);
  });
});
