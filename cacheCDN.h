#ifndef __CACHECDN_H__
#define __CACHECDN_H__

#include <map>
#include <string>

using std::map;
using std::string;

namespace ICS45C {
namespace CDN {

class Cache {
 private:
  const unsigned int freshnessCount;
  map<string, string> contentMap;
  map<string, char*> binaryMap;
  map<string, unsigned int> freshnessMap;
  map<string, unsigned int> accessMap;
  unsigned int diskAccess;

 public:
  Cache(unsigned int freshnessCount);
  ~Cache();
  string getText(string filepath);
  char* getBinary(string filepath);
  bool isCached(string filepath);
  unsigned int getFreshness(string filepath);
  void markFileFresh(string filepath);
  void purgeCache();
  string topFile();
  string getStats();
};

}  // namespace CDN
}  // namespace ICS45C

#endif  // __CACHECDN_H__
