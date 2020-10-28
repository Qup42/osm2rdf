// Copyright 2020, University of Freiburg
// Authors: Axel Lehmann <lehmann@cs.uni-freiburg.de>.

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

#include "osm2ttl/Version.h"
#include "osm2ttl/config/Config.h"
#include "osm2ttl/osm/OsmiumHandler.h"
#include "osm2ttl/ttl/Writer.h"
#include "osm2ttl/util/Ram.h"
#include "osm2ttl/util/Time.h"

template <typename T>
void run(osm2ttl::config::Config& config) {
  // Setup
  // Input file reference
  osm2ttl::ttl::Writer<T> writer{config};
  if (!writer.open()) {
    std::cerr << "Error opening outputfile: " << config.output << std::endl;
    exit(1);
  }
  writer.writeHeader();

  osm2ttl::osm::OsmiumHandler osmiumHandler{config, &writer};
  osmiumHandler.handle();

  // All work done, close output
  writer.close();
}

// ____________________________________________________________________________
int main(int argc, char** argv) {
  std::cerr << osm2ttl::util::currentTimeFormatted()
            << "osm2ttl :: " << osm2ttl::version::GIT_INFO << " :: BEGIN"
            << std::endl;
  osm2ttl::config::Config& config = osm2ttl::config::Config::getInstance();
  config.fromArgs(argc, argv);
  std::cerr << config.getInfo(osm2ttl::util::formattedTimeSpacer) << std::endl;

  std::cerr << osm2ttl::util::currentTimeFormatted() << "Free ram: "
            << osm2ttl::util::ram::available() /
                   (osm2ttl::util::ram::GIGA * 1.0)
            << "G/"
            << osm2ttl::util::ram::physPages() /
                   (osm2ttl::util::ram::GIGA * 1.0)
            << "G" << std::endl;

  try {
    if (config.outputFormat == "qlever") {
      run<osm2ttl::ttl::format::QLEVER>(config);
    } else if (config.outputFormat == "nt") {
      run<osm2ttl::ttl::format::NT>(config);
    } else if (config.outputFormat == "ttl") {
      run<osm2ttl::ttl::format::TTL>(config);
    } else {
      std::cerr << osm2ttl::util::currentTimeFormatted()
                << "osm2ttl :: " << osm2ttl::version::GIT_INFO << " :: ERROR"
                << std::endl;
      std::cerr << "Unknown output format: " << config.outputFormat
                << std::endl;
      std::exit(1);
    }
  } catch (const std::exception& e) {
    // All exceptions used by the Osmium library derive from std::exception.
    std::cerr << osm2ttl::util::currentTimeFormatted()
              << "osm2ttl :: " << osm2ttl::version::GIT_INFO << " :: ERROR"
              << std::endl;
    std::cerr << e.what() << std::endl;
    std::exit(1);
  }
  std::cerr << osm2ttl::util::currentTimeFormatted()
            << "osm2ttl :: " << osm2ttl::version::GIT_INFO << " :: FINISHED"
            << std::endl;
  std::exit(0);
}
