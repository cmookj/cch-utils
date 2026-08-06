#include "date_time.h"
#include "str.h"

#include <iomanip>
#include <sstream>

namespace cch::util::dt {

time_point
now () {
  return std::chrono::high_resolution_clock::now();
}

double
duration_msec (const time_point& since) {
  return std::chrono::duration_cast<std::chrono::milliseconds> (now() - since).count();
}

const std::string
current_time (const bool& readable) {
  std::time_t now = std::time (nullptr);
  std::tm     tstruct;
  char        buf[16];
  tstruct = *std::localtime (&now);
  const char* fmt;
  if (readable) {
    fmt = "%H:%M:%S";
  } else {
    fmt = "%H%M%S";
  }
  std::strftime (buf, sizeof (buf), fmt, &tstruct);
  return buf;
}

const std::string
current_time_milli (const bool& readable) {
  // The system_clock class is the interface in C++ to get system-wide
  // real-time wall clock.  Most systems use Unix time, which is represented
  // as seconds past from 00:00:00 UTC on 1 January 1970 (an arbitrary date),
  // called Unix epoch. Note that leap seconds are ignored.  Thus Unix time is
  // not truly an accurate representation of UTC.
  //
  // Firstly, the now() method is called to return the current point in time.
  // The next method called is time_since_epoch to retrieve the amount of time
  // between *this and the clock's epoch, but it returns an duration
  // class object.  This object should call the count method to return the
  // actual number of ticks, and to represent it as milliseconds.  The result
  // is cast using duration_cast<milliseconds>.
  auto now = std::chrono::system_clock::now();
  auto ms_since_epoch =
      std::chrono::duration_cast<std::chrono::milliseconds> (now.time_since_epoch()).count();
  auto ms_str              = std::to_string (ms_since_epoch % 1000);
  auto count_leading_zeros = 3 - ms_str.length();

  std::stringstream strm{};
  auto              now_c = std::chrono::system_clock::to_time_t (now);

  const char* fmt;
  if (readable) {
    fmt = "%H:%M:%S";
  } else {
    fmt = "%H%M%S";
  }
  strm << std::put_time (std::localtime (&now_c), fmt) << ".";
  for (int i = 0; i < count_leading_zeros; i++) {
    strm << "0";
  }
  strm << ms_str;

  return strm.str();
}

const std::string
current_date (const bool& readable) {
  std::time_t now = std::time (nullptr);
  std::tm     tstruct;
  char        buf[80];
  tstruct = *std::localtime (&now);
  const char* fmt;
  if (readable) {
    fmt = "%Y-%m-%d";
  } else {
    fmt = "%Y%m%d";
  }
  std::strftime (buf, sizeof (buf), fmt, &tstruct);
  return buf;
}

// String representation of the seconds from the epoch of time (Thu. Jan. 1,
// 00:00:00 1970)
std::string
seconds_from_epoch_as_string () {
  const auto now = std::chrono::system_clock::now();
  return cch::str::format (
      "{}", std::chrono::duration_cast<std::chrono::seconds> (now.time_since_epoch()).count()
  );
}

long
estimate_remain (int total, int done, long done_elapsed) {
  return long (float (done_elapsed) / float (done) * float (total - done));
}

std::chrono::duration<double>
estimate_remain (int total, int done, const std::chrono::duration<double>& done_elapsed) {
  return done_elapsed / done * (total - done);
}

std::string
to_string (const size_t value, const size_t max_digits) {
  using std::floor;
  using std::log10;

  int count_digits{1};
  if (value != 0) count_digits = static_cast<int> (floor (log10 (static_cast<double> (value)))) + 1;

  if (count_digits > max_digits) return std::to_string (value);

  std::ostringstream os;
  for (size_t i = 0; i < max_digits - count_digits; i++)
    os << '0';

  os << std::to_string (value);

  return os.str();
}

std::string
to_string (long msecs) {
  using std::setfill;
  using std::setw;

  std::ostringstream oss;

  long sec = (msecs / 1000);
  long min = sec / 60;
  long hr  = min / 60;

  if (hr > 23) {
    oss << hr / 24 << "d-";
  }
  oss << setfill ('0') << setw (2) << (hr % 24) << ":" << setw (2) << (min % 60) << ":" << setw (2)
      << (sec % 60) << "." << setfill ('0') << setw (3) << (msecs % 1000);

  return oss.str();
}

std::string
to_string (const std::chrono::duration<double>& seconds) {
  using std::setfill;
  using std::setw;

  std::ostringstream oss;

  long sec = (long)seconds.count();
  long min = sec / 60;
  long hr  = min / 60;

  if (hr > 23) {
    oss << hr / 24 << "d-";
  }
  oss << setfill ('0') << setw (2) << (hr % 24) << ":" << setw (2) << (min % 60) << ":" << setw (2)
      << (sec % 60) << "." << setfill ('0') << setw (3)
      << int ((seconds.count() - double (sec)) * 1000 + 0.5);

  return oss.str();
}

std::string
time_stamp (long long msecs, bool show_msec, bool utc) {
  using std::setfill;
  using std::setw;

  std::time_t        t  = msecs / 1000;
  int                ms = msecs % 1000;
  std::ostringstream oss;

  if (utc) {
    oss << std::put_time (std::gmtime (&t), "%Y-%m-%dT%H-%M-%S");
    if (show_msec) oss << "." << setfill ('0') << setw (3) << ms;
    oss << "-UTC";
  } else {
    oss << std::put_time (std::localtime (&t), "%Y-%m-%dT%H-%M-%S");
    if (show_msec) oss << "." << setfill ('0') << setw (3) << ms;
  }

  return oss.str();
}

std::string
time_stamp (bool show_msec) {
  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds> (
      std::chrono::system_clock::now().time_since_epoch()
  );

  return time_stamp (ms.count(), show_msec, false);
}

std::string
time_stamp_utc (bool show_msec) {
  std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds> (
      std::chrono::system_clock::now().time_since_epoch()
  );

  return time_stamp (ms.count(), show_msec, true);
}

}  // namespace cch::util::dt
