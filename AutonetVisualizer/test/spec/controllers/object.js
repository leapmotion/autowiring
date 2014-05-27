'use strict';

describe('Controller: ObjectCtrl', function () {

  // load the controller's module
  beforeEach(module('autoNetApp'));

  var ObjectCtrl,
    scope;

  // Initialize the controller and a mock scope
  beforeEach(inject(function ($controller, $rootScope) {
    scope = $rootScope.$new();
    ObjectCtrl = $controller('ObjectCtrl', {
      $scope: scope
    });
  }));

  it('should attach a list of awesomeThings to the scope', function () {
    expect(scope.awesomeThings.length).toBe(3);
  });
});
