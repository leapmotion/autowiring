// Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#if __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif
#include <gtest/gtest-all.cc>
#include <iostream>
#include <csignal>
#include <autowiring/autowiring.h>

using namespace testing::internal;
using namespace std;

// Misc. globals:
int g_argc;
char** g_argv;

template<class T, int n>
const char (&ArraySizer(const T (&vals)[n]))[n];
#define ARYLEN(x) sizeof(ArraySizer(x))

int main(int argc, char* argv[])
{
  signal(SIGINT, [](int param){
    AutoGlobalContext ctxt;
    ctxt->SignalTerminate();
    ctxt->Wait();
  });
  
  g_argc = argc;
  g_argv = argv;

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
