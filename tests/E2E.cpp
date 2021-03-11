// Copyright 2020, University of Freiburg
// Authors: Axel Lehmann <lehmann@cs.uni-freiburg.de>.

#include <filesystem>
#include <string>
#include <vector>

#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"
#include "osm2ttl/config/Config.h"
#include "osm2ttl/osm/OsmiumHandler.h"
#include "osm2ttl/ttl/Writer.h"
#include "osm2ttl/util/Output.h"

std::vector<std::filesystem::path> getFilesWithPrefixFromPath(
    std::filesystem::path path, std::string_view prefix) {
  std::vector<std::filesystem::path> result;
  for (auto& e : std::filesystem::directory_iterator(path)) {
    if (!e.is_regular_file()) {
      continue;
    }
    if (e.path().filename().string().find(prefix, 0) != 0) {
      continue;
    }
    result.push_back(e.path());
  }
  return result;
}

// ____________________________________________________________________________
TEST(E2E, singleNode) {
  // Capture std::cerr and std::cout
  std::stringstream cerrBuffer;
  std::stringstream coutBuffer;
  std::streambuf* cerrBufferOrig = std::cerr.rdbuf();
  std::streambuf* coutBufferOrig = std::cout.rdbuf();
  std::cerr.rdbuf(cerrBuffer.rdbuf());
  std::cout.rdbuf(coutBuffer.rdbuf());

  osm2ttl::config::Config config;
  config.output = "";
  config.outputCompress = false;
  config.mergeOutput = osm2ttl::util::OutputMergeMode::NONE;

  // Create empty input file
  config.input = config.getTempPath("E2E", "singleNode.osm");
  std::ofstream inputFile(config.input);
  // Copied from
  // https://wiki.openstreetmap.org/w/index.php?title=OSM_XML&oldid=2081001
  inputFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<osm version=\"0.6\" generator=\"CGImap 0.0.2\">\n"
            << "<node id=\"298884269\" lat=\"54.0901746\" lon=\"12.2482632\" "
               "user=\"SvenHRO\" uid=\"46882\" visible=\"true\" version=\"1\" "
               "changeset=\"676636\" timestamp=\"2008-09-21T21:37:45Z\"/>"
            << "</osm>" << std::endl;

  osm2ttl::util::Output output{config, config.output};
  output.open();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::QLEVER> writer{config, &output};
  writer.writeHeader();

  osm2ttl::osm::OsmiumHandler osmiumHandler{config, &writer};
  osmiumHandler.handle();

  output.flush();
  output.close();

  const auto printedState = cerrBuffer.str();
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("areas seen:0 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("nodes seen:1 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("relations seen:0 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("ways seen:0 dumped: 0 geometry: 0\n"));
  const auto printedData = coutBuffer.str();
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr(
          "@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "@prefix wd: <http://www.wikidata.org/entity/> .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "@prefix xsd: <http://www.w3.org/2001/XMLSchema#> .\n"));

  // Reset std::cerr and std::cout
  std::cerr.rdbuf(cerrBufferOrig);
  std::cout.rdbuf(coutBufferOrig);
  std::filesystem::remove(config.input);
}

// ____________________________________________________________________________
TEST(E2E, singleNodeWithTags) {
  // Capture std::cerr and std::cout
  std::stringstream cerrBuffer;
  std::stringstream coutBuffer;
  std::streambuf* cerrBufferOrig = std::cerr.rdbuf();
  std::streambuf* coutBufferOrig = std::cout.rdbuf();
  std::cerr.rdbuf(cerrBuffer.rdbuf());
  std::cout.rdbuf(coutBuffer.rdbuf());

  osm2ttl::config::Config config;
  config.output = "";
  config.outputCompress = false;
  config.mergeOutput = osm2ttl::util::OutputMergeMode::NONE;

  // Create empty input file
  config.input = config.getTempPath("E2E", "singleNodeWithTags.osm");
  std::ofstream inputFile(config.input);
  // Copied from ...
  inputFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<osm version=\"0.6\" generator=\"CGImap 0.0.2\">\n";
  // ... https://www.openstreetmap.org/node/240092010
  inputFile << "<node id=\"240092010\" lat=\"47.9960901\" lon=\"7.8494005\" "
               "visible=\"true\" version=\"1\">"
               "  <tag k=\"alt_name\" v=\"Freiburg i. Br.\"/>\n"
               "  <tag k=\"name\" v=\"Freiburg im Breisgau\"/>\n"
               "  <tag k=\"name:ja\" v=\"フライブルク\"/>\n"
               "  <tag k=\"short_name\" v=\"Freiburg\"/>\n"
               "  <tag k=\"wikidata\" v=\"Q2833\"/>\n"
               "  <tag k=\"wikipedia\" v=\"de:Freiburg im Breisgau\"/>\n"
               "</node>\n";
  inputFile << "</osm>" << std::endl;

  osm2ttl::util::Output output{config, config.output};
  output.open();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::QLEVER> writer{config, &output};
  writer.writeHeader();

  osm2ttl::osm::OsmiumHandler osmiumHandler{config, &writer};
  osmiumHandler.handle();

  output.flush();
  output.close();

  const auto printedState = cerrBuffer.str();
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("areas seen:0 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("nodes seen:1 dumped: 1 geometry: 1\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("relations seen:0 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("ways seen:0 dumped: 0 geometry: 0\n"));
  const auto printedData = coutBuffer.str();
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmnode:240092010 rdf:type osm:node .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmnode:240092010 geo:hasGeometry \"POINT(7.849400500000 "
                  "47.996090100000)\"^^geo:wktLiteral .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmnode:240092010 osmt:alt_name \"Freiburg i. Br.\" .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmnode:240092010 osmt:name \"Freiburg im Breisgau\" .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmnode:240092010 osmt:name:ja "
                                   "\"\xE3\x83\x95\xE3\x83\xA9\xE3\x82\xA4\xE3"
                                   "\x83\x96\xE3\x83\xAB\xE3\x82\xAF\" .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmnode:240092010 osmt:short_name \"Freiburg\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmnode:240092010 osmt:wikidata \"Q2833\" .\n"));
  ASSERT_THAT(printedData, ::testing::HasSubstr(
                               "osmnode:240092010 osm:wikidata wd:Q2833 .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr(
          "osmnode:240092010 osmt:wikipedia \"de:Freiburg im Breisgau\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr(
          "osmnode:240092010 osm:wikipedia "
          "<https://de.wikipedia.org/wiki/Freiburg%20im%20Breisgau> .\n"));

  // Reset std::cerr and std::cout
  std::cerr.rdbuf(cerrBufferOrig);
  std::cout.rdbuf(coutBufferOrig);
  std::filesystem::remove(config.input);
}

// ____________________________________________________________________________
TEST(E2E, singleWayWithTagsAndNodes) {
  // Capture std::cerr and std::cout
  std::stringstream cerrBuffer;
  std::stringstream coutBuffer;
  std::streambuf* cerrBufferOrig = std::cerr.rdbuf();
  std::streambuf* coutBufferOrig = std::cout.rdbuf();
  std::cerr.rdbuf(cerrBuffer.rdbuf());
  std::cout.rdbuf(coutBuffer.rdbuf());

  osm2ttl::config::Config config;
  config.output = "";
  config.outputCompress = false;
  config.mergeOutput = osm2ttl::util::OutputMergeMode::NONE;

  // Create empty input file
  config.input = config.getTempPath("E2E", "singleWayWithTagsAndNodes.osm");
  std::ofstream inputFile(config.input);
  // Copied from ...
  inputFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<osm version=\"0.6\" generator=\"CGImap 0.0.2\">\n";
  // ... https://www.openstreetmap.org/node/1137213233 see e2e/w98284318.xml
  // for full file
  inputFile
      << "<way id=\"98284318\" visible=\"true\" version=\"10\">"
         "  <tag k=\"addr:city\" v=\"Freiburg im Breisgau\"/>\n"
         "  <tag k=\"addr:housenumber\" v=\"51\"/>\n"
         "  <tag k=\"addr:postcode\" v=\"79110\"/>\n"
         "  <tag k=\"addr:street\" v=\"Georges-Köhler-Allee\"/>\n"
         "  <tag k=\"building\" v=\"university\"/>\n"
         "  <tag k=\"building:levels\" v=\"4\"/>\n"
         "  <tag k=\"name\" v=\"Gebäude 51\"/>\n"
         "  <tag k=\"roof:levels\" v=\"1\"/>\n"
         "  <tag k=\"roof:shape\" v=\"hipped\"/>\n"
         "  <tag k=\"source:outline\" v=\"maps4bw (LGL, www.lgl-bw.de)\"/>\n"
         "</way>\n";
  inputFile << "</osm>" << std::endl;

  osm2ttl::util::Output output{config, config.output};
  output.open();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::QLEVER> writer{config, &output};
  writer.writeHeader();

  osm2ttl::osm::OsmiumHandler osmiumHandler{config, &writer};
  osmiumHandler.handle();

  output.flush();
  output.close();

  const auto printedState = cerrBuffer.str();
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("areas seen:0 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("nodes seen:0 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("relations seen:0 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("ways seen:1 dumped: 1 geometry: 1\n"));
  const auto printedData = coutBuffer.str();
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmway:98284318 rdf:type osm:way .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr(
          "osmway:98284318 osmt:addr:city \"Freiburg im Breisgau\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:addr:housenumber \"51\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:addr:postcode \"79110\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr(
          "osmway:98284318 osmt:addr:street \"Georges-Köhler-Allee\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:building \"university\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:building:levels \"4\" .\n"));
  ASSERT_THAT(printedData, ::testing::HasSubstr(
                               "osmway:98284318 osmt:name \"Gebäude 51\" .\n"));
  ASSERT_THAT(printedData, ::testing::HasSubstr(
                               "osmway:98284318 osmt:roof:levels \"1\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:roof:shape \"hipped\" .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmway:98284318 osmt:source:outline "
                                   "\"maps4bw (LGL, www.lgl-bw.de)\" .\n"));
  // No nodes -> no real geometry
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmway:98284318 geo:hasGeometry "
                                   "\"LINESTRING()\"^^geo:wktLiteral .\n"));

  // Reset std::cerr and std::cout
  std::cerr.rdbuf(cerrBufferOrig);
  std::cout.rdbuf(coutBufferOrig);
  std::filesystem::remove(config.input);
}

// ____________________________________________________________________________
TEST(E2E, osmWikiExample) {
  // Capture std::cerr and std::cout
  std::stringstream cerrBuffer;
  std::stringstream coutBuffer;
  std::streambuf* cerrBufferOrig = std::cerr.rdbuf();
  std::streambuf* coutBufferOrig = std::cout.rdbuf();
  std::cerr.rdbuf(cerrBuffer.rdbuf());
  std::cout.rdbuf(coutBuffer.rdbuf());

  osm2ttl::config::Config config;
  config.output = "";
  config.outputCompress = false;
  config.mergeOutput = osm2ttl::util::OutputMergeMode::NONE;

  // Create empty input file
  config.input = config.getTempPath("E2E", "osmWikiExample.osm");
  std::ofstream inputFile(config.input);
  // Based on
  // https://wiki.openstreetmap.org/w/index.php?title=OSM_XML&oldid=2081001
  inputFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<osm version=\"0.6\" generator=\"CGImap 0.0.2\">\n";
  inputFile << " <node id=\"298884269\" lat=\"54.0901746\" lon=\"12.2482632\" "
               "visible=\"true\" version=\"1\"/>\n";
  inputFile << " <node id=\"261728686\" lat=\"54.0906309\" lon=\"12.2441924\" "
               "visible=\"true\" version=\"1\"/>\n";
  inputFile << " <node id=\"1831881213\" version=\"1\" lat=\"54.0900666\" "
               "lon=\"12.2539381\" visible=\"true\">\n"
               "  <tag k=\"name\" v=\"Neu Broderstorf\"/>\n"
               "  <tag k=\"traffic_sign\" v=\"city_limit\"/>\n"
               " </node>\n";
  inputFile << " <node id=\"298884272\" lat=\"54.0901447\" lon=\"12.2516513\" "
               "visible=\"true\" version=\"1\"/>\n";
  inputFile << " <way id=\"26659127\" visible=\"true\" version=\"5\">\n"
               "  <nd ref=\"298884269\"/>\n"
               "  <nd ref=\"261728686\"/>\n"
               "  <nd ref=\"298884272\"/>\n"
               "  <tag k=\"highway\" v=\"unclassified\"/>\n"
               "  <tag k=\"name\" v=\"Pastower Straße\"/>\n"
               " </way>\n";
  inputFile << " <relation id=\"56688\" visible=\"true\" version=\"28\">\n"
               "  <member type=\"node\" ref=\"298884269\" role=\"\"/>\n"
               "  <member type=\"node\" ref=\"261728686\" role=\"\"/>\n"
               "  <member type=\"way\" ref=\"26659127\" role=\"\"/>\n"
               "  <member type=\"node\" ref=\"1831881213\" role=\"\"/>\n"
               "  <tag k=\"name\" v=\"Küstenbus Linie 123\"/>\n"
               "  <tag k=\"network\" v=\"VVW\"/>\n"
               "  <tag k=\"operator\" v=\"Regionalverkehr Küste\"/>\n"
               "  <tag k=\"ref\" v=\"123\"/>\n"
               "  <tag k=\"route\" v=\"bus\"/>\n"
               "  <tag k=\"type\" v=\"route\"/>\n"
               " </relation>";
  inputFile << "</osm>" << std::endl;

  osm2ttl::util::Output output{config, config.output};
  output.open();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::TTL> writer{config, &output};
  writer.writeHeader();

  osm2ttl::osm::OsmiumHandler osmiumHandler{config, &writer};
  osmiumHandler.handle();

  output.flush();
  output.close();

  const auto printedState = cerrBuffer.str();
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("areas seen:0 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("nodes seen:4 dumped: 1 geometry: 1\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("relations seen:1 dumped: 1 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("ways seen:1 dumped: 1 geometry: 1\n"));
  const auto printedData = coutBuffer.str();
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmnode:1831881213 osmt:traffic_sign \"city_limit\" .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:26659127 osmt:name \"Pastower Straße\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:26659127 geo:hasGeometry \"LINESTRING("));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmrel:56688 rdf:type osm:relation .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("_:2 osm:id osmway:26659127 .\n"));

  // Reset std::cerr and std::cout
  std::cerr.rdbuf(cerrBufferOrig);
  std::cout.rdbuf(coutBufferOrig);
  std::filesystem::remove(config.input);
}

// ____________________________________________________________________________
TEST(E2E, building51) {
  // Capture std::cerr and std::cout
  std::stringstream cerrBuffer;
  std::stringstream coutBuffer;
  std::streambuf* cerrBufferOrig = std::cerr.rdbuf();
  std::streambuf* coutBufferOrig = std::cout.rdbuf();
  std::cerr.rdbuf(cerrBuffer.rdbuf());
  std::cout.rdbuf(coutBuffer.rdbuf());

  osm2ttl::config::Config config;
  config.output = "";
  config.outputCompress = false;
  config.mergeOutput = osm2ttl::util::OutputMergeMode::NONE;

  // Create empty input file
  config.input = config.getTempPath("E2E", "building51.osm");
  std::ofstream inputFile(config.input);

  std::vector<std::filesystem::path> nodes =
      getFilesWithPrefixFromPath("../../tests/e2e/building_51", "n");
  std::vector<std::filesystem::path> ways =
      getFilesWithPrefixFromPath("../../tests/e2e/building_51", "w");
  std::sort(nodes.begin(), nodes.end(), [](const auto& a, const auto& b) -> bool {
    return a.filename().string() < b.filename().string();
  });
  std::sort(ways.begin(), ways.end(), [](const auto& a, const auto& b) -> bool {
    return a.filename().string() < b.filename().string();
  });
  // Copied from ...
  inputFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<osm version=\"0.6\" generator=\"CGImap 0.0.2\">\n";
  for (const auto& f : nodes) {
    inputFile << std::ifstream(f).rdbuf();
  }
  for (const auto& f : ways) {
    inputFile << std::ifstream(f).rdbuf();
  }
  inputFile << "</osm>" << std::endl;

  osm2ttl::util::Output output{config, config.output};
  output.open();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::QLEVER> writer{config, &output};
  writer.writeHeader();

  osm2ttl::osm::OsmiumHandler osmiumHandler{config, &writer};
  osmiumHandler.handle();

  output.flush();
  output.close();

  const auto printedState = cerrBuffer.str();
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("areas seen:1 dumped: 1 geometry: 1\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("nodes seen:19 dumped: 3 geometry: 3\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("relations seen:0 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("ways seen:1 dumped: 1 geometry: 1\n"));
  ASSERT_THAT(
      printedState,
      ::testing::HasSubstr("Contains relations for 3 nodes in 1 areas ...\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr(
                  "... done with looking at 3 areas, 0 skipped by DAG\n"
                  "                           3 checks performed\n"
                  "                           contains: 3 yes: 3\n"));
  const auto printedData = coutBuffer.str();
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmway:98284318 rdf:type osm:way .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr(
          "osmway:98284318 osmt:addr:city \"Freiburg im Breisgau\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:addr:housenumber \"51\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:addr:postcode \"79110\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr(
          "osmway:98284318 osmt:addr:street \"Georges-Köhler-Allee\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:building \"university\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:building:levels \"4\" .\n"));
  ASSERT_THAT(printedData, ::testing::HasSubstr(
                               "osmway:98284318 osmt:name \"Gebäude 51\" .\n"));
  ASSERT_THAT(printedData, ::testing::HasSubstr(
                               "osmway:98284318 osmt:roof:levels \"1\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:roof:shape \"hipped\" .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmway:98284318 osmt:source:outline "
                                   "\"maps4bw (LGL, www.lgl-bw.de)\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 geo:hasGeometry \"LINESTRING(7"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "smway:98284318 geo:hasGeometry \"MULTIPOLYGON(((7"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:intersects osmnode:2110601105 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:contains osmnode:2110601105 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:intersects osmnode:2110601134 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:contains osmnode:2110601134 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:intersects osmnode:5190342871 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:contains osmnode:5190342871 .\n"));

  // Reset std::cerr and std::cout
  std::cerr.rdbuf(cerrBufferOrig);
  std::cout.rdbuf(coutBufferOrig);
  std::filesystem::remove(config.input);
}

// ____________________________________________________________________________
TEST(E2E, tf) {
  // Capture std::cerr and std::cout
  std::stringstream cerrBuffer;
  std::stringstream coutBuffer;
  std::streambuf* cerrBufferOrig = std::cerr.rdbuf();
  std::streambuf* coutBufferOrig = std::cout.rdbuf();
  std::cerr.rdbuf(cerrBuffer.rdbuf());
  std::cout.rdbuf(coutBuffer.rdbuf());

  osm2ttl::config::Config config;
  config.output = "";
  config.outputCompress = false;
  config.mergeOutput = osm2ttl::util::OutputMergeMode::NONE;

  // Create empty input file
  config.input = config.getTempPath("E2E", "tf.osm");
  std::ofstream inputFile(config.input);

  std::vector<std::filesystem::path> nodes =
      getFilesWithPrefixFromPath("../../tests/e2e/tf", "n");
  std::vector<std::filesystem::path> ways =
      getFilesWithPrefixFromPath("../../tests/e2e/tf", "w");
  std::sort(nodes.begin(), nodes.end(), [](const auto& a, const auto& b) -> bool {
    return a.filename().string() < b.filename().string();
  });
  std::sort(ways.begin(), ways.end(), [](const auto& a, const auto& b) -> bool {
    return a.filename().string() < b.filename().string();
  });
  // Copied from ...
  inputFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<osm version=\"0.6\" generator=\"CGImap 0.0.2\">\n";
  for (const auto& f : nodes) {
    inputFile << std::ifstream(f).rdbuf();
  }
  for (const auto& f : ways) {
    inputFile << std::ifstream(f).rdbuf();
  }
  inputFile << "</osm>" << std::endl;

  osm2ttl::util::Output output{config, config.output};
  output.open();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::QLEVER> writer{config, &output};
  writer.writeHeader();

  osm2ttl::osm::OsmiumHandler osmiumHandler{config, &writer};
  osmiumHandler.handle();

  output.flush();
  output.close();

  const auto printedState = cerrBuffer.str();
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("areas seen:1 dumped: 1 geometry: 1\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("nodes seen:45 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("relations seen:0 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("ways seen:1 dumped: 1 geometry: 1\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr(
                  "Skipping contains relation for nodes ... no nodes\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("0 intersection checks performed, 0 skipped "
                                   "by DAG, 0 skipped by NodeInfo\n"));
  const auto printedData = coutBuffer.str();
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmway:4498466 rdf:type osm:way .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:4498466 osmt:name \"Technische Fakultät\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr(
          "osmway:4498466 osmt:int_name \"Faculty of Engineering\" .\n"));
  ASSERT_THAT(printedData, ::testing::HasSubstr(
                               "osmway:4498466 osmt:operator "
                               "\"Albert-Ludwigs-Universität Freiburg\" .\n"));
  ASSERT_THAT(printedData, ::testing::HasSubstr(
                               "osmway:4498466 osmt:wheelchair \"yes\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:4498466 geo:hasGeometry \"LINESTRING(7"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:4498466 geo:hasGeometry \"MULTIPOLYGON(((7"));

  // Reset std::cerr and std::cout
  std::cerr.rdbuf(cerrBufferOrig);
  std::cout.rdbuf(coutBufferOrig);
  std::filesystem::remove(config.input);
}

// ____________________________________________________________________________
TEST(E2E, building51inTF) {
  // Capture std::cerr and std::cout
  std::stringstream cerrBuffer;
  std::stringstream coutBuffer;
  std::streambuf* cerrBufferOrig = std::cerr.rdbuf();
  std::streambuf* coutBufferOrig = std::cout.rdbuf();
  std::cerr.rdbuf(cerrBuffer.rdbuf());
  std::cout.rdbuf(coutBuffer.rdbuf());

  osm2ttl::config::Config config;
  config.output = "";
  config.outputCompress = false;
  config.mergeOutput = osm2ttl::util::OutputMergeMode::NONE;

  // Create empty input file
  config.input = config.getTempPath("E2E", "building51.osm");
  std::ofstream inputFile(config.input);

  std::vector<std::filesystem::path> nodes =
      getFilesWithPrefixFromPath("../../tests/e2e/building_51", "n");
  const auto& tempNodes = getFilesWithPrefixFromPath("../../tests/e2e/tf", "n");
  nodes.insert(nodes.end(), tempNodes.begin(), tempNodes.end());
  std::vector<std::filesystem::path> ways =
      getFilesWithPrefixFromPath("../../tests/e2e/building_51", "w");
  const auto& tempWays = getFilesWithPrefixFromPath("../../tests/e2e/tf", "w");
  ways.insert(ways.end(), tempWays.begin(), tempWays.end());
  std::sort(nodes.begin(), nodes.end(), [](const auto& a, const auto& b) -> bool {
    return a.filename().string() < b.filename().string();
  });
  std::sort(ways.begin(), ways.end(), [](const auto& a, const auto& b) -> bool {
    return a.filename().string() < b.filename().string();
  });
  // Copied from ...
  inputFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            << "<osm version=\"0.6\" generator=\"CGImap 0.0.2\">\n";
  for (const auto& f : nodes) {
    inputFile << std::ifstream(f).rdbuf();
  }
  for (const auto& f : ways) {
    inputFile << std::ifstream(f).rdbuf();
  }
  inputFile << "</osm>" << std::endl;

  osm2ttl::util::Output output{config, config.output};
  output.open();
  osm2ttl::ttl::Writer<osm2ttl::ttl::format::QLEVER> writer{config, &output};
  writer.writeHeader();

  osm2ttl::osm::OsmiumHandler osmiumHandler{config, &writer};
  osmiumHandler.handle();

  output.flush();
  output.close();

  const auto printedState = cerrBuffer.str();
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("areas seen:2 dumped: 2 geometry: 2\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("nodes seen:64 dumped: 3 geometry: 3\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("relations seen:0 dumped: 0 geometry: 0\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr("ways seen:2 dumped: 2 geometry: 2\n"));
  ASSERT_THAT(
      printedState,
      ::testing::HasSubstr("Contains relations for 3 nodes in 2 areas ...\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr(
                  "... done with looking at 6 areas, 3 skipped by DAG\n"
                  "                           3 checks performed\n"
                  "                           contains: 3 yes: 3\n"));
  ASSERT_THAT(
      printedState,
      ::testing::HasSubstr("Contains relations for 2 ways in 2 areas ...\n"));
  ASSERT_THAT(printedState,
              ::testing::HasSubstr(
                  "... done with looking at 2 areas\n"
                  "                           1 intersection checks performed, "
                  "0 skipped by DAG, 1 skipped by NodeInfo\n"
                  "                           intersect: 1 yes: 0\n"
                  "                           1 contains checks performed, 0 "
                  "skipped by DAG\n"
                  "                           contains: 1 contains envelope: 1 "
                  "yes: 1\n"));
  const auto printedData = coutBuffer.str();
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmway:98284318 rdf:type osm:way .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr(
          "osmway:98284318 osmt:addr:city \"Freiburg im Breisgau\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:addr:housenumber \"51\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:addr:postcode \"79110\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr(
          "osmway:98284318 osmt:addr:street \"Georges-Köhler-Allee\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:building \"university\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:building:levels \"4\" .\n"));
  ASSERT_THAT(printedData, ::testing::HasSubstr(
                               "osmway:98284318 osmt:name \"Gebäude 51\" .\n"));
  ASSERT_THAT(printedData, ::testing::HasSubstr(
                               "osmway:98284318 osmt:roof:levels \"1\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 osmt:roof:shape \"hipped\" .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmway:98284318 osmt:source:outline "
                                   "\"maps4bw (LGL, www.lgl-bw.de)\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:98284318 geo:hasGeometry \"LINESTRING(7"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "smway:98284318 geo:hasGeometry \"MULTIPOLYGON(((7"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr("osmway:4498466 rdf:type osm:way .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:4498466 osmt:name \"Technische Fakultät\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr(
          "osmway:4498466 osmt:int_name \"Faculty of Engineering\" .\n"));
  ASSERT_THAT(printedData, ::testing::HasSubstr(
                               "osmway:4498466 osmt:operator "
                               "\"Albert-Ludwigs-Universität Freiburg\" .\n"));
  ASSERT_THAT(printedData, ::testing::HasSubstr(
                               "osmway:4498466 osmt:wheelchair \"yes\" .\n"));
  ASSERT_THAT(
      printedData,
      ::testing::HasSubstr("osmway:4498466 geo:hasGeometry \"LINESTRING(7"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:4498466 geo:hasGeometry \"MULTIPOLYGON(((7"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:4498466 ogc:contains_area osmway:98284318 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:4498466 ogc:intersects_area osmway:98284318 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:intersects osmnode:2110601105 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:contains osmnode:2110601105 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:intersects osmnode:2110601134 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:contains osmnode:2110601134 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:intersects osmnode:5190342871 .\n"));
  ASSERT_THAT(printedData,
              ::testing::HasSubstr(
                  "osmway:98284318 ogc:contains osmnode:5190342871 .\n"));

  // Reset std::cerr and std::cout
  std::cerr.rdbuf(cerrBufferOrig);
  std::cout.rdbuf(coutBufferOrig);
  std::filesystem::remove(config.input);
}