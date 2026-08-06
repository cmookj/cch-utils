#include "str.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

namespace cch::utils {

class logger_t {
public:
  logger_t ()          = default;
  virtual ~logger_t () = default;

  logger_t (const logger_t&) = default;
  logger_t (logger_t&&)      = default;

  logger_t&
  operator= (const logger_t&) = default;
  logger_t&
  operator= (logger_t&&) = default;

  void
  enable_console_output (const bool&);

  void
  info (const std::string_view);

  void
  warn (const std::string_view);

  void
  error (const std::string_view);

  void
  write (const fs::path&, const bool flush = true);

private:
  void
  _log (const size_t, const std::string_view);

  bool                     _console_output_enabled = true;
  std::vector<std::string> _logs;
  std::vector<std::string> _prefixes = {" ", "\033[1;33m*\033[0m", "\033[1;31m!\033[0m"};
};

}  // namespace cch::utils

namespace cch {

void
info (const std::string&);

void
warn (const std::string&);

void
error (const std::string&);

template <typename FormatString, typename... Args>
inline void
info (const FormatString& fmt, Args&&... args) {
  info (cch::str::format (fmt, std::forward<Args> (args)...));
}

template <typename FormatString, typename... Args>
inline void
warn (const FormatString& fmt, Args&&... args) {
  warn (cch::str::format (fmt, std::forward<Args> (args)...));
}

template <typename FormatString, typename... Args>
inline void
error (const FormatString& fmt, Args&&... args) {
  error (cch::str::format (fmt, std::forward<Args> (args)...));
}

}  // namespace cch
