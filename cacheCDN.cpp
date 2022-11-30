#include "cacheCDN.h"

#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>

using std::ifstream;
using std::ios;
using std::map;
using std::streampos;
using std::string;
using std::stringstream;

namespace ICS45C {
namespace CDN {

Cache::Cache(unsigned int freshnessCount) : freshnessCount(freshnessCount) {
  diskAccess = 0;
}

Cache::~Cache() {
  for (auto entry : binaryMap) {
    delete[] entry.second;
  }
}

string Cache::getText(string filepath) {
  auto access = accessMap.find(filepath);
  if (access == accessMap.end()) {
    accessMap[filepath] = 1;
  } else {
    accessMap[filepath]++;
  }
  auto search = contentMap.find(filepath);
  string content = "";
  if (search == contentMap.end() || freshnessMap[filepath] == 0) {
    diskAccess++;
    ifstream inFile;
    string text;
    inFile.open(filepath);
    while (getline(inFile, text)) {
      content = content + text + "\n";
    }
    inFile.close();
    contentMap[filepath] = content;
    if (freshnessCount == 0) {
      freshnessMap[filepath] = freshnessCount;
    } else {
      freshnessMap[filepath] = freshnessCount - 1;
    }
  } else {
    content = search->second;
    freshnessMap[filepath] -= 1;
  }
  return content;
}

char* Cache::getBinary(string filepath) {
  auto access = accessMap.find(filepath);
  if (access == accessMap.end()) {
    accessMap[filepath] = 1;
  } else {
    accessMap[filepath]++;
  }
  auto search = binaryMap.find(filepath);
  char* memblock;
  if (search == binaryMap.end() || freshnessMap[filepath] == 0) {
    diskAccess++;
    if (search != binaryMap.end()) {
      delete[] binaryMap[filepath];
    }

    ifstream inBinFile(filepath, ios::in | ios::binary | ios::ate);
    streampos size = inBinFile.tellg();
    memblock = new char[size];
    inBinFile.seekg(0, ios::beg);
    inBinFile.read(memblock, size);
    inBinFile.close();
    binaryMap[filepath] = memblock;
    if (freshnessCount == 0) {
      freshnessMap[filepath] = freshnessCount;
    } else {
      freshnessMap[filepath] = freshnessCount - 1;
    }
  } else {
    memblock = search->second;
    freshnessMap[filepath] -= 1;
  }
  return memblock;
}

bool Cache::isCached(string filepath) {
  auto search = freshnessMap.find(filepath);
  if (search == freshnessMap.end()) {
    return false;
  } else {
    return true;
  }
}

unsigned int Cache::getFreshness(string filepath) {
  auto search = freshnessMap.find(filepath);
  if (search == freshnessMap.end()) {
    return 0;
  } else {
    return search->second;
  }
}

void Cache::markFileFresh(string filepath) {
  auto search = freshnessMap.find(filepath);
  if (search != freshnessMap.end()) {
    search->second = freshnessCount;
  }
}

void Cache::purgeCache() {
  for (auto entry : binaryMap) {
    delete[] entry.second;
  }
  contentMap.clear();
  freshnessMap.clear();
  binaryMap.clear();
}

string Cache::topFile() {
  string top = "";
  unsigned int highNum = 0;
  for (auto entry : accessMap) {
    if (entry.second > highNum) {
      top = entry.first;
      highNum = entry.second;
    }
  }
  return top;
}

string Cache::getStats() {
  stringstream output;
  string top = topFile();
  unsigned int totalRequests = 0;
  unsigned int fileRequests = 0;
  double averageRequests = 0.00;

  for (auto entry : accessMap) {
    fileRequests++;
    totalRequests += entry.second;
  }

  if (fileRequests != 0) {
    averageRequests = (double)totalRequests / fileRequests;
  }

  output << "Cache Stats\n----------\nTotal requests: " << totalRequests
         << "\nTotal files requested: " << fileRequests
         << "\nAverage requests per file: " << std::fixed
         << std::setprecision(2) << averageRequests << "\nTop file: " << top
         << " (" << accessMap[top]
         << " requests)\nTotal times read from disk: " << diskAccess
         << "\n----------\n";

  return output.str();
}

}  // namespace CDN
}  // namespace ICS45C
