'use strict';

describe('Controller: HeaderctrlCtrl', function () {

  // load the controller's module
  beforeEach(module('autoNetApp'));

  var HeaderctrlCtrl,
    scope;

  // Initialize the controller and a mock scope
  beforeEach(inject(function ($controller, $rootScope) {
    scope = $rootScope.$new();
    HeaderctrlCtrl = $controller('HeaderctrlCtrl', {
      $scope: scope
    });
  }));

  it('should attach a list of awesomeThings to the scope', function () {
    expect(scope.awesomeThings.length).toBe(3);
  });
});
