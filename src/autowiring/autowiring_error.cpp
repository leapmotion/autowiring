// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "autowiring_error.h"

autowiring_error::autowiring_error(const std::string& what):
  m_what(what)
{}

autowiring_error::~autowiring_error(void){}
