#include <algorithm>
#include <cstdlib>
#include <dirent.h>
#include <iostream>
#include <vector>

int main(int /*argc*/, char ** /*argv*/) {
  // Discover test binaries named tabula-unit-tests*
  std::vector<std::string> tests;
  DIR *d = opendir(".");
  if (d) {
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
      std::string name(entry->d_name);
      if (name.rfind("tabula-unit-tests", 0) == 0) {
        // executable candidates only
        tests.push_back(name);
      }
    }
    closedir(d);
  }

  std::sort(tests.begin(), tests.end());

  if (tests.empty()) {
    std::cerr << "No test binaries found matching 'tabula-unit-tests*' in "
                 "current dir\n";
    return 1;
  }

  int overall = 0;
  for (auto &t : tests) {
    std::string cmd = std::string("./") + t;
    std::cout << "Running " << cmd << "\n";
    int rc = std::system(cmd.c_str());
    if (rc != 0) {
      std::cerr << "Test " << t << " failed with code " << rc << "\n";
      overall = rc;
    }
  }
  return overall;
}
