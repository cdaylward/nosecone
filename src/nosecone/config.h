#pragma once

#include <string>

#include "appc/discovery/types.h"


namespace nosecone {


struct Config {
  bool verbose;
  std::string images_path;
  std::string containers_path;
  appc::discovery::Labels default_labels;
};


} // namespace nosecone
