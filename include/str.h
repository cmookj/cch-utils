// -----------------------------------------------------------------------------
// String process utility
// -----------------------------------------------------------------------------
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace cch::str {

std::string_view
ltrim (std::string_view str);
std::string_view
rtrim (std::string_view str);
std::string_view
trim (std::string_view str);

std::string
trim (const std::string& str, const std::string& whitespace = " \t");

std::string
reduce (
    const std::string& str,
    const std::string& fill       = " ",
    const std::string& whitespace = " \t"
);

std::string to_upper (std::string);
std::string to_lower (std::string);
std::string remove_illegal_char (std::string);

// Compare two strings case insensitive. If strings are identical, return zero.
int
compare (std::string str1, std::string str2);

// Formatting string
template <typename... Args>
std::string
format_string (const std::string& format, Args... args) {
  size_t size = std::snprintf (nullptr, 0, format.c_str(), args...) + 1;  // Extra space for '\0'
  if (size <= 0) {
    throw std::runtime_error ("Invalid string format");
  }
  std::unique_ptr<char[]> buf (new char[size]);
  std::snprintf (buf.get(), size, format.c_str(), args...);
  return std::string (buf.get(), buf.get() + size - 1);  // We don't want the '\0' inside
}

std::string
format (const char*);

template <typename T, typename... Args>
std::string
format (const char* s, T value, Args... args) {
  std::stringstream ss;

  while (s && *s) {
    switch (*s) {
    case '{':
      if (*(s + 1) == '}') {
        s += 2;
        ss << value;
        return ss.str() + format (s, args...);
      } else if (*(s + 1) == '{') {
        ss << '{';
        s += 2;
      } else throw std::runtime_error{"invalid format: { should be followed by another { or }"};

      break;

    case '}':
      if (*(s + 1) == '}') {
        ss << '}';
        s += 2;
      } else throw std::runtime_error{"invalid format: } should be followed by another }"};

      break;

    default: ss << *s++;
    }
  }
  throw std::runtime_error{"extra arguments provided to format"};
}

// KMP (Knuth-Morris-Pratt) search algorithm
std::vector<int>
llps (std::string& pat);

std::vector<int>
search (std::string& pat, std::string& txt);

}  // namespace cch::str
