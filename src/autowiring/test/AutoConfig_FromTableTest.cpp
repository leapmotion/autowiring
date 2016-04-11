// Copyright (C) 2012-2015 Leap Motion, Inc. All rights reserved.
#include "stdafx.h"
#include <autowiring/config_descriptor.h>
#include <autowiring/config.h>
#include <autowiring/ConfigRegistry.h>
#include <autowiring/observable.h>

namespace aw = autowiring;

class AutoConfig_FromTableTest :
  public testing::Test
{};

namespace {
  // An example object that might be responsible for downloading data from a server.  The
  // path and timeout parameters, which such a class might use, are described in a config
  // descriptor.
  class DataDownloader {
  public:
    std::string api_path;
    int timeout;

    static aw::config_descriptor GetConfigDescriptor(void) {
      return{
        { "api_path", &DataDownloader::api_path },
        { "timeout", &DataDownloader::timeout }
      };
    }
  };
}

// A simulated map of key-value pairs.  In a real use case, this might be populated from a
// configuration file or downloaded from a network source.
static std::unordered_map<std::string, std::string> sc_config = {
  { "api_path", "http://example.com/v3/endpoint" },
  { "timeout", "299" }
};

TEST_F(AutoConfig_FromTableTest, SimpleConfigurationReadIn) {
  AutoCurrentContext ctxt;

  // Load in all of the keys:
  for(const auto& key : sc_config)
    ctxt->Config.Set(key.first.c_str(), key.second.c_str());

  // Now instantiate and see if we get valid values:
  AutoRequired<DataDownloader> ddld;
  ASSERT_STREQ("http://example.com/v3/endpoint", ddld->api_path.c_str());
  ASSERT_EQ(299, ddld->timeout);
}

// A simulated set of users and user data:
static std::unordered_map<std::string, std::string> sc_allusers[] = {
  {
    { "user_name", "Code McKenzy" },
    { "user_email", "code@mckenzy.com" },
    { "user_age", "30" },
    { "user_valid", "true" }
  },
  {
    { "user_name", "Louis Stalwart" },
    { "user_email", "louie@gmail.com" },
    { "user_age", "22" },
    { "user_valid", "false" }
  }
};

namespace {
  // A single user in a hypothetical database of users
  class UserTest {
  public:
    // These are specific to this context:
    std::string user_name;
    std::string user_email;
    int user_age;
    bool user_valid;

    static aw::config_descriptor GetConfigDescriptor(void) {
      return{
        { "user_name", &UserTest::user_name },
        { "user_email", &UserTest::user_email },
        { "user_age", &UserTest::user_age },
        { "user_valid", &UserTest::user_valid }
      };
    }
  };
}

TEST_F(AutoConfig_FromTableTest, HeirarchialAssignment) {
  AutoCurrentContext ctxt;

  // Load in all of the root keys:
  for (const auto& key : sc_config)
    ctxt->Config.Set(key.first.c_str(), key.second.c_str());

  // Then start creating subcontexts for all of the user classes:
  for (auto& user_key : sc_allusers) {
    AutoCreateContext child;

    // Set all configuration keys in the configuration block:
    for (auto& key : user_key)
      child->Config.Set(key.first.c_str(), key.second.c_str());

    // Inject after the fact, verify key assignments are all correct
    AutoRequired<UserTest> ut{ child };
    ASSERT_EQ(user_key["user_name"], ut->user_name);
    ASSERT_EQ(user_key["user_email"], ut->user_email);

    std::stringstream ss;
    ss << ut->user_age;
    ASSERT_EQ(user_key["user_age"], ss.str());
    ASSERT_EQ(user_key["user_valid"] == "true", ut->user_valid);
  }
}
