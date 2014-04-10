'use strict';

describe('Service: Leapstate', function () {

  // load the service's module
  beforeEach(module('autoNetApp'));

  // instantiate service
  var Leapstate;
  beforeEach(inject(function (_Leapstate_) {
    Leapstate = _Leapstate_;
  }));

  it('should do something', function () {
    expect(!!Leapstate).toBe(true);
  });

});
