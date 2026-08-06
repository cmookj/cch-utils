// -----------------------------------------------------------------------------
// Date & Time
// -----------------------------------------------------------------------------

#include <chrono>
#include <string>
#include <utility>

namespace cch::util::dt {

// Measure time of execution of a function
using time_point = std::chrono::high_resolution_clock::time_point;

time_point
now ();

double
duration_msec (const time_point& since);

template <typename F, typename... Args>
double
measure_time_msec (F func, Args&&... args) {
  time_point t1 = now();
  func (std::forward<Args> (args)...);
  return duration_msec (t1);
}

// Get string of time
const std::string
current_time (const bool& readable = false);
const std::string
current_time_milli (const bool& readable = false);

// Get string of date
const std::string
current_date (const bool& readable = false);

// String representation of the seconds from the epoch of time (Thu. Jan. 1,
// 00:00:00 1970)
std::string
seconds_from_epoch_as_string ();

long
estimate_remain (int, int, long);
std::chrono::duration<double>
estimate_remain (int, int, const std::chrono::duration<double>&);

std::string
to_string (const size_t, const size_t);
std::string
to_string (long);
std::string
to_string (const std::chrono::duration<double>& seconds);
std::string
time_stamp (bool show_msec = false);
std::string
time_stamp_utc (bool show_msec = false);
std::string
time_stamp (long long msec, bool show_msec, bool utc);

}  // namespace cch::util::dt
