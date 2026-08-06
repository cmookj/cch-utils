//
// fileio.h
//
// Definitions for file and stream input/output
//
#ifndef __FILE_IO_H__
#define __FILE_IO_H__

#include <iostream>

#include <cstddef>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "types.h"

namespace cch::fileio {

// Read CSV file into a vector.
template <Indexable T>
std::vector<T>
read_csv_points (const std::string& filename) {
  std::vector<T> points;
  std::ifstream  file (filename);

  if (!file.is_open()) {
    throw std::runtime_error ("Failed to open file: " + filename);
  }

  std::string line;
  while (std::getline (file, line)) {
    if (line.empty()) continue;

    std::istringstream ss (line);
    std::string        x_str, y_str;

    if (!std::getline (ss, x_str, ',')) continue;
    if (!std::getline (ss, y_str, ',')) continue;

    T p;
    p[0] = std::stod (x_str);
    p[1] = std::stod (y_str);
    points.push_back (p);
  }

  return points;
}

// Write CSV file
template <Indexable T>
void
write_points_csv_file (
    const std::vector<T>& points,
    const std::string&    filename,
    const bool            cyclic = false
) {
  std::ofstream file (filename);

  if (!file.is_open()) {
    throw std::runtime_error ("Failed to open file: " + filename);
  }

  for (const auto& p : points) {
    file << p[0] << ", " << p[1] << "\n";
  }

  if (cyclic) file << points.front().x << ", " << points.front().y << "\n";
}

// Write stream of bytes to a file
template <HasBeginEnd Container>
void
write_file (const std::string& path_str, const Container& c, bool in_binary = true) {
  std::ofstream file (path_str, std::ios::binary);
  if (!in_binary) {
    file = std::ofstream{path_str};
  }

  if (!file.is_open()) {
    throw std::runtime_error ("Cannot open file: " + path_str);
  }

  for (const auto& item : c) {
    file.write (reinterpret_cast<const char*> (&item), sizeof (typename Container::value_type));
  }
}

// Read stream of data into a file
// Fast path: single allocation & single bulk read
template <ContiguousByteContainer Container>
Container
read_file_binary (const std::string& path) {
  // Opening with std::ios::ate positions the cursor at EOF, so tellg() gives
  // the file isze in O(1) without a separate stat call.
  std::ifstream f (path, std::ios::binary | std::ios::ate);
  if (!f) throw std::runtime_error ("Cannot open file: " + path);

  const auto size = static_cast<std::size_t> (f.tellg());
  f.seekg (0);

  Container container (size / sizeof (typename Container::value_type));  // One allocation, no fill
  if (size > 0)
    f.read (reinterpret_cast<char*> (container.data()), static_cast<std::streamsize> (size));

  return container;
}

// Read stream of data into a file
// Generic path: iterator-based, reserve when possible
template <typename Container>
  requires (!ContiguousByteContainer<Container>)
Container
read_file_binary (const std::string& path) {
  std::ifstream f (path, std::ios::binary | std::ios::ate);
  if (!f) throw std::runtime_error ("Cannot open file: " + path);

  const auto size = static_cast<std::size_t> (f.tellg());
  f.seekg (0);

  const auto count = size / sizeof (typename Container::value_type);

  std::cout << "Size = " << size << std::endl;
  std::cout << "Count elements = " << count << std::endl;

  // Read into a vector buffer first
  std::vector<typename Container::value_type> buffer (count);
  f.read (reinterpret_cast<char*> (buffer.data()), size);

  // Move into the container
  Container container;

  // Reserve if supported (vector, string, deque-but not list)
  if constexpr (requires { container.reserve (count); }) container.reserve (count);

  container.assign (buffer.begin(), buffer.end());

  return container;
}

}  // namespace cch::fileio

#endif
