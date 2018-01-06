// Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include "SystemThreadPoolWin.hpp"
#include "SystemThreadPoolWinLH.hpp"
#include "SystemThreadPoolWinXP.hpp"
#include "DispatchThunk.h"

using namespace autowiring;
using namespace autowiring::detail;

static const HMODULE hKernel32 = LoadLibrary("kernel32.dll");
const decltype(&CreateThreadpoolWork) autowiring::detail::g_CreateThreadpoolWork = (decltype(&CreateThreadpoolWork)) GetProcAddress(hKernel32, "CreateThreadpoolWork");
const decltype(&CloseThreadpoolWork) autowiring::detail::g_CloseThreadpoolWork = (decltype(&CloseThreadpoolWork)) GetProcAddress(hKernel32, "CloseThreadpoolWork");
const decltype(&SubmitThreadpoolWork) autowiring::detail::g_SubmitThreadpoolWork = (decltype(&SubmitThreadpoolWork)) GetProcAddress(hKernel32, "SubmitThreadpoolWork");

SystemThreadPoolWin::SystemThreadPoolWin(void) :
  m_toBeDone(~0)
{}

SystemThreadPoolWin::~SystemThreadPoolWin(void) {}

std::shared_ptr<SystemThreadPool> SystemThreadPool::New(void)
{
  if (!g_CreateThreadpoolWork)
    // Fallback to the XP APIs:
    return std::make_shared<SystemThreadPoolWinXP>();

  // Use the latest greatest API:
  return std::make_shared<SystemThreadPoolWinLH>();
}

void SystemThreadPoolWin::OnStop(void) {
  m_toBeDone.Abort();
}
