
#pragma once

#include <cerrno>
#include <cstring>

#include "appc/util/status.h"

inline Status Errno(const std::string& where, int err) {
  return Error(where + strerror(err));
}


