'use strict';

describe('Directive: ActiveNav', function () {

  // load the directive's module
  beforeEach(module('autoNetApp'));

  var element,
    scope;

  beforeEach(inject(function ($rootScope) {
    scope = $rootScope.$new();
  }));

  it('should make hidden element visible', inject(function ($compile) {
    element = angular.element('<-active-nav></-active-nav>');
    element = $compile(element)(scope);
    expect(element.text()).toBe('this is the ActiveNav directive');
  }));
});
