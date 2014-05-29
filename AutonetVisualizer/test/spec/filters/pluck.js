'use strict';

describe('Filter: Pluck', function () {

  // load the filter's module
  beforeEach(module('autoNetApp'));

  // initialize a new instance of the filter before each test
  var Pluck;
  beforeEach(inject(function ($filter) {
    Pluck = $filter('Pluck');
  }));

  it('should return the input prefixed with "Pluck filter:"', function () {
    var text = 'angularjs';
    expect(Pluck(text)).toBe('Pluck filter: ' + text);
  });

});
