'use strict';

describe('Filter: display', function () {

  // load the filter's module
  beforeEach(module('autoNetApp'));

  // initialize a new instance of the filter before each test
  var display;
  beforeEach(inject(function ($filter) {
    display = $filter('display');
  }));

  it('should return the input prefixed with "display filter:"', function () {
    var text = 'angularjs';
    expect(display(text)).toBe('display filter: ' + text);
  });

});
