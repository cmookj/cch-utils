#include "log.h"

#include <fstream>
#include <iostream>

namespace cch::utils {

void
logger_t::enable_console_output (const bool& flag) {
  _console_output_enabled = flag;
}

void
logger_t::_log (const size_t idx, const std::string_view msg) {
  std::string log_msg =
      std::string{"["} + std::string{_prefixes[idx]} + std::string{"] "} + std::string{msg};
  _logs.emplace_back (msg);

  if (_console_output_enabled) {
    std::cout << log_msg << '\n';
  }
}

void
logger_t::info (const std::string_view msg) {
  _log (0, msg);
}

void
logger_t::warn (const std::string_view msg) {
  _log (1, msg);
}

void
logger_t::error (const std::string_view msg) {
  _log (2, msg);
}

void
logger_t::write (const fs::path& path, const bool flush) {
  if (_logs.size() == 0) return;

  bool done = false;

  std::ofstream strm{path};
  if (strm.is_open()) {
    for (const auto& log : _logs) {
      strm << log << '\n';
    }
    done = true;
  }

  if (flush && done) _logs.clear();
}

}  // namespace cch::utils

namespace cch {

cch::utils::logger_t logger;

void
info (const std::string& msg) {
  logger.info (msg);
}

void
warn (const std::string& msg) {
  logger.warn (msg);
}

void
error (const std::string& msg) {
  logger.error (msg);
}

}  // namespace cch
