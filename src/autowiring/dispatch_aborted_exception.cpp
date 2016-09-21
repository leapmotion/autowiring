// Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "dispatch_aborted_exception.h"

dispatch_aborted_exception::dispatch_aborted_exception(void) :
  autowiring_error("Dispatch queue aborted")
{}

dispatch_aborted_exception::dispatch_aborted_exception(const std::string& what) :
  autowiring_error(what)
{}

dispatch_aborted_exception::~dispatch_aborted_exception(void){}
