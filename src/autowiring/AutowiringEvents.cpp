// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "AutowiringEvents.h"

AutowiringEvents::AutowiringEvents(void){}

AutowiringEvents::~AutowiringEvents(void){}

template class RegEvent<AutowiringEvents>;
template class TypeUnifierComplex<AutowiringEvents>;
