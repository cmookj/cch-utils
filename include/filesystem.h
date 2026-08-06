#include <filesystem>
#include <string>
#include <string_view>

namespace fs = std::filesystem;

/*
#if __cplusplus > 201703L
#include <version>

#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;

#elif __cpp_lib_experimental_filesystem
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#else
#error "no filesystem support ='("
#endif

#else

// We haven't checked which filesystem to include yet
#ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL

// Check for feature test macro for <filesystem>
#if defined(__cpp_lib_filesystem)
#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0

// Check for feature test macro for <experimental/filesystem>
#elif defined(__cpp_lib_experimental_filesystem)
#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// We can't check if headers exist...
// Let's assume experimental to be safe
#elif !defined(__has_include)
#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// Check if the header "<filesystem>" exists
#elif __has_include(<filesystem>)

// If we're compiling on Visual Studio and are not compiling with C++17, we need
to use experimental #ifdef _MSC_VER

// Check and include header that defines "_HAS_CXX17"
#if __has_include(<yvals_core.h>)
#include <yvals_core.h>

// Check for enabled C++17 support
#if defined(_HAS_CXX17) && _HAS_CXX17
// We're using C++17, so let's use the normal version
#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#endif
#endif

// If the marco isn't defined yet, that means any of the other VS specific
checks failed, so we need to use experimental #ifndef
INCLUDE_STD_FILESYSTEM_EXPERIMENTAL #define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
1 #endif

// Not on Visual Studio. Let's use the normal version
#else // #ifdef _MSC_VER
#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
#endif

// Check if the header "<filesystem>" exists
#elif __has_include(<experimental/filesystem>)
#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

// Fail if neither header is available with a nice error message
#else
#error Could not find system header "<filesystem>" or
"<experimental/filesystem>" #endif

// We priously determined that we need the exprimental version
#if INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
// Include it
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

// We have a decent compiler and can use the normal version
#else
// Include it
#include <filesystem>
namespace fs = std::filesystem;
#endif

#endif // #ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL

#endif
*/

namespace cch::filesystem {

std::string
append_suffix (const std::string& file_name, const std::string& suffix);

std::filesystem::path
prepend_dir (const std::string& dir, const std::string& file_name);

/**
 @brief Checks whether the file exists or not
 @param filepath The path to the file
 @param length The length of the file in bytes
 */
bool
file_exists (const fs::path& filepath, const std::size_t length = 0);

/**
 @brief Checks whether the file exists or not in the current directory
 @param filename The name of the file
 @param length The length of the file in bytes
 */
bool
file_exists (const std::string_view& filename, const std::size_t length = 0);

std::string
check_duplicate (const std::string_view& filename, const std::string_view& extension);

/**
 * @brief Checks whether there exists the directory given in a directory
 */
bool
directory_exists (const std::string_view&, const fs::path&);

/**
 * @brief Moves the filesystem item given to the path specified
 */
void
move_item_to (const fs::path&, const fs::path&);

/**
 * @brief Copies the filesystem item given to the path specified
 */
void
copy_item_to (const fs::path&, const fs::path&);

/**
 * @brief Moves all the contents of a directory to the path specified
 */
void
move_directory_contents_to (const fs::path&, const fs::path&);

/**
 * @brief Copies all the contents of a directory to the path specified
 */
void
copy_directory_contents_to (const fs::path&, const fs::path&);

/**
 * @brief Deletes all the contents of a directory
 *
 * @param delete_directory If it is true, this function deletes the directory
 * too. Otherwise, it leaves the directory
 */
void
remove_all_contents (const fs::path&, const bool delete_directory = false);

/**
 * @brief Creates a new directory in the destination directory.
 *
 * @param name The name of the directory to create.
 * @param dst The path to the destination directory.
 */
void
create_new_directory_in (const std::string_view& name, const fs::path& dst);

/**
 * @brief Creates a directory if possible.
 * This function checks the existence of a directory with the path given, and
 * creates if it does not.
 *
 * @param dir The path to the directory to create.
 * @return true if the directory was created, false otherwise.
 */
bool
create_directory (const fs::path& dir);

/**
 * @brief Determines if an item is movable to a destination.
 *
 * @param item The path to the item to move.
 * @param dst The path the destination.
 * @return true The item is movable.
 * @return false The item is not movable.
 */
bool
item_movable (const fs::path& item, const fs::path& dst);

// Read a text file into a string object
std::string
to_string (const fs::path&);

// Find file
//
// This function tries to find the specified file beginning from the current
// directory (given as current).
// 1. If it finds the file in the current directory, it returns the canonical
// path to the file.
// 2. Otherwise, it looks whether there exists dir/file in the current
// directory.
//    If found, it returns the canonical path to the file.
// 3. Otherwise, it sets current directory to the parent of the current
// directory.
//    Then repeat from step 1 above.
// 4. If it fails to find until it reaches to the root path of current
// filesystem,
//    if throws an exception (runtime_error).
fs::path
find_file (
    const std::string_view& dir,
    const std::string_view& file,
    fs::path                current = fs::current_path()
);

// Find directory
//
// This function tries to find the specified directory beginning from the
// current directory (given as current).
// 1. If it finds the directory in the current directory, it returns the
// canonical path to the directory.
// 2. Otherwise, it sets current directory to the parent of the current
// directory.
//    Then repeat from step 1 above.
// 4. If it fails to find until it reaches to the root path of current
// filesystem,
//    if throws an exception (runtime_error).
fs::path
find_directory (const std::string_view& dir, fs::path current = fs::current_path());

std::string
make_unique_filename (
    const fs::path&         dir,
    const std::string_view& filename,
    const std::string_view& ext
);

/**
 * @brief Creates a symbolic link to a file
 * @param target A path to a normal file
 * @param link A symbolic link to create
 */
bool
create_symlink (const fs::path target, const fs::path link);

/**
 * @breif Creates a symbolic link to a directory
 * @param target A path to a normal directory
 * @param link A symbolic link to create
 */
bool
create_directory_symlink (const fs::path target, const fs::path link);

/**
 * @brief Follows a symbolic link.
 * @detail If the given argument is a path to a normal file, it returns the
 * input.
 * @param link A path to follow
 */
fs::path
follow_target_path (const fs::path& link);

}  // namespace cch::filesystem
