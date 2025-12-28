#ifndef BEHAVIORFLOW_TESTING_UTILS_H_
#define BEHAVIORFLOW_TESTING_UTILS_H_

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <random>
#include <string_view>

namespace bflow::test {

class ScopedTempFile {
 public:
  explicit ScopedTempFile(std::string_view content, std::string_view extension = ".tmp") {
    auto temp_dir = std::filesystem::temp_directory_path();
    const uint64_t n = std::random_device{}();
    std::string filename = std::string("test_config_") + std::to_string(n) + std::string(extension);
    path_ = temp_dir / filename;
    std::ofstream ofs(path_);
    ofs << content;
    ofs.close();
  }

  ~ScopedTempFile() {
    if (std::filesystem::exists(path_)) {
      std::filesystem::remove(path_);
    }
  }

  std::filesystem::path path() const { return path_; }
  std::string path_string() const { return path_.string(); }

 private:
  std::filesystem::path path_;
};

}  // namespace bflow::test

#endif  // BEHAVIORFLOW_TESTING_UTILS_H_