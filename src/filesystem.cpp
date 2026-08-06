#include "filesystem.h"
#include "log.h"

#include <filesystem>
#include <fstream>
#include <regex>

namespace cch::filesystem {

std::string
append_suffix (const std::string& file_name, const std::string& suffix) {
  const auto pos       = file_name.find_last_of ('.');
  const auto extension = file_name.substr (pos);
  return file_name.substr (0, pos) + suffix + extension;
}

fs::path
prepend_dir (const std::string& dir, const std::string& file_name) {
  return (dir.length() > 0 ? fs::path{dir} / fs::path{file_name} : fs::path{file_name});
}

bool
file_exists (const fs::path& filepath, const std::size_t length) {
  if (length == 0) return fs::exists (filepath);
  else return (fs::exists (filepath) && fs::file_size (filepath) >= length);
}

bool
file_exists (const std::string_view& filename, const std::size_t length) {
  // Search the file in current directory
  return file_exists (fs::current_path() / fs::path{filename}, length);
}

std::string
check_duplicate (const std::string_view& filename, const std::string_view& extension) {
  std::string ext{extension};
  if (ext.find (".") == std::string::npos) {
    ext = std::string{"."} + ext;
  }

  int         duplicated{0};
  std::string base{filename};
  while (file_exists (std::string_view{base + ext})) {
    base = std::string{filename} + std::string{"_"} + std::to_string (++duplicated);
  }
  return base + ext;
}

// Check whether there exists the directory given in a directory
bool
directory_exists (const std::string_view& name, const fs::path& dir) {
  if (!fs::exists (dir)) throw std::runtime_error{"The path given does not exist"};

  for (const auto& p : fs::directory_iterator (dir)) {
    if (fs::is_directory (p) && p.path().filename() == name) return true;
  }

  return false;
}

// Move the filesystem item given to the path specified
void
move_item_to (const fs::path& item, const fs::path& dst) {
  if (item_movable (item, dst)) {
    try {
      if (fs::is_directory (item)) {
        create_new_directory_in (fs::canonical (item).filename().string(), dst);
        move_directory_contents_to (item, dst / fs::path{fs::canonical (item).filename()});
      }
      fs::copy (item, dst / fs::canonical (item).filename());
      fs::remove (item);
    } catch (std::runtime_error& err) {
      throw err;
    }
  } else throw std::runtime_error{"The item not movable"};
}

// Copy the filesystem item given to the path specified
void
copy_item_to (const fs::path& item, const fs::path& dst) {
  if (item_movable (item, dst)) {
    try {
      if (fs::is_directory (item)) {
        create_new_directory_in (fs::canonical (item).filename().string(), dst);
        copy_directory_contents_to (item, dst / fs::path{fs::canonical (item).filename()});
      } else {
        fs::copy (item, dst / fs::canonical (item).filename());
      }
    } catch (std::runtime_error& err) {
      throw err;
    }
  } else throw std::runtime_error{"The item not movable"};
}

// Move all the contents of a directory to the path specified
void
move_directory_contents_to (const fs::path& src, const fs::path& dst) {
  for (auto& p : fs::directory_iterator (src)) {
    if (!item_movable (p.path(), dst))
      throw std::runtime_error{"The contents of the directory not moveable"};
  }

  // All the contents moveable to the destination
  for (auto& p : fs::directory_iterator (src)) {
    fs::copy (p.path(), dst / p.path().filename());
    fs::remove (p.path());
  }
}

// Copy all the contents of a directory to the path specified
void
copy_directory_contents_to (const fs::path& src, const fs::path& dst) {
  for (auto& p : fs::directory_iterator (src)) {
    if (!item_movable (p.path(), dst))
      throw std::runtime_error{"The contents of the directory not moveable"};
  }

  // All the contents moveable to the destination
  for (auto& p : fs::directory_iterator (src)) {
    fs::copy (p.path(), dst / p.path().filename());
  }
}

// Deletes all the contents of a directory
// The directory will be deleted, if the argument deleteDirectory is true.
// Otherwise, the directory itself remains.
void
remove_all_contents (const fs::path& p, const bool delete_directory) {
  if (!fs::exists (p) || !fs::is_directory (p))
    throw std::runtime_error{"The directory of which to remove the contents does not exist"};

  for (auto& item : fs::directory_iterator (p)) {
    fs::remove_all (item);
  }

  if (delete_directory) fs::remove (p);
}

/**
 * @brief Creates a new directory in the destination directory.
 *
 * @param name The name of the directory to create.
 * @param dst The path to the destination directory.
 *
 */
void
create_new_directory_in (const std::string_view& name, const fs::path& dst) {
  if (!fs::exists (dst) || !fs::is_directory (dst))
    throw std::runtime_error{"The directory in which a new directory is to create does not exist"};

  if (fs::exists (dst / fs::path{name})) throw std::runtime_error{"The directory already exists"};

  fs::create_directory (dst / name);
}

/**
 * @brief Creates a directory if possible.
 * This function checks the existence of a directory with the path given, and
 * creates if it does not.
 *
 * @param dir The path to the directory to create.
 * @return true if the directory was created, false otherwise.
 */
bool
create_directory (const fs::path& dir) {
  if (!fs::exists (dir)) {
    fs::create_directory (dir);
    return true;
  }
  return false;
}

/**
 * @brief Determines if an item is movable to a destination.
 *
 * @param item The path to the item to move.
 * @param dst The path the destination.
 * @return true The item is movable.
 * @return false The item is not movable.
 */
bool
item_movable (const fs::path& item, const fs::path& dst) {
  if (!fs::exists (item)) return false;

  if (fs::is_directory (dst)) {    // Destination is directory
    if (fs::exists (dst)) {        // Destination exists
      if (dst.filename() == "") {  // The destination is the inside of the directory
        if (fs::exists (fs::canonical (dst) / fs::canonical (item).filename())) return false;
        else return true;
      } else {  // The destination is the directory itself.
        if (dst.filename() == item.filename()) return false;
        else return true;
      }
    } else  // The destination directory does not exist.
      return false;
  } else {                 // Destination is a file
    if (fs::exists (dst))  // There exists an item in destination path.
      return false;
    else  // No item in the destination path.
      return true;
  }
}

/**
 * @brief Reads a text file into a string object
 *
 * @param file_path The path to the file.
 * @return string The string object which has the contents of the file.
 */
std::string
to_string (const fs::path& file_path) {
  if (!fs::exists (file_path)) return std::string{""};

  std::ifstream in (file_path);
  if (in) {
    in.seekg (0, std::ios::end);
    std::size_t len = in.tellg();
    in.seekg (0);
    std::string contents (len, '\0');
    in.read (&contents[0], len);
    return contents;
  }
  return std::string{""};
}

// Find file 'file' in 'dir' beginning from 'current' path upward to the root
// path.
fs::path
find_file (const std::string_view& dir, const std::string_view& file, fs::path current) {
  if (!current.has_root_path()) {
    throw std::runtime_error{"Current path does not have root path"};
  }

  for (;;) {
    if (fs::exists (current / fs::path{file})) {
      return current / fs::path{file};
    } else if (fs::exists (current / fs::path{dir} / fs::path{file})) {
      return current / fs::path{dir} / fs::path{file};
    } else if (current == current.root_path()) {
      throw std::runtime_error{"Failed to find the file"};
    } else {
      current = current.parent_path();
    }
  }
}

// Find directory 'dir' beginning from 'current' path upward to the root path.
fs::path
find_directory (const std::string_view& dir, fs::path current) {
  if (!current.has_root_path()) {
    throw std::runtime_error{"Current path does not have root path"};
  }

  for (;;) {
    if (fs::exists (current / fs::path{dir})) {
      return current / fs::path{dir};
    } else if (current == current.root_path()) {
      throw std::runtime_error{"Failed to find the directory"};
    } else {
      current = current.parent_path();
    }
  }
}

std::string
make_unique_filename (
    const fs::path&         dir,
    const std::string_view& filename,
    const std::string_view& ext
) {
  fs::path file_path = dir / fs::path{filename};
  return check_duplicate (file_path.string(), std::string{ext});
}

bool
create_symlink (const fs::path target, const fs::path link) {
  if (!fs::exists (target)) {
    cch::warn ("No symbolic link is creates: the target '{}' does not exist.", target.string());
    return false;
  }

  if (fs::is_symlink (link)) {
    cch::info ("Pre-existing link is deleted.");
    fs::remove (link);
  }

  fs::create_symlink (target, link);
  return true;
}

bool
create_directory_symlink (const fs::path target, const fs::path link) {
  if (!fs::exists (target)) {
    cch::warn ("No symbolic link is created: the target '{}' does not exist.", target.string());
    return false;
  }

  if (fs::is_symlink (link)) {
    cch::info ("Pre-existing link is deleted.");
    fs::remove (link);
  }

  fs::create_directory_symlink (target, link);
  return true;
}

fs::path
follow_target_path (const fs::path& link) {
  if (is_symlink (link)) {
    cch::info ("Following the symbolic link: {}", link.string());
    return read_symlink (link);
  } else return link;
}

}  // namespace cch::filesystem
