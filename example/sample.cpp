#include "../include/log.h"

#include <string>

int
main (int argc, char* argv[]) {
  // cch::utils::logger_t logger;

  // logger.enable_console_output (true);

  cch::info ("Hi, there!");
  cch::warn ("Be careful...");
  cch::error ("You are in a big trouble");

  return 0;
}
