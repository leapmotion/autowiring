'use strict';

describe('Controller: EditCtrl', function () {

  // load the controller's module
  beforeEach(module('autoNetApp'));

  var EditCtrl,
    scope;

  // Initialize the controller and a mock scope
  beforeEach(inject(function ($controller, $rootScope) {
    scope = $rootScope.$new();
    EditCtrl = $controller('EditCtrl', {
      $scope: scope
    });
  }));

  it('should attach a list of awesomeThings to the scope', function () {
    expect(scope.awesomeThings.length).toBe(3);
  });
});
