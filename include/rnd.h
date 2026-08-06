#include <algorithm>
#include <random>  // For QuickCheck
#include <type_traits>

namespace cch {
template <typename T> struct random_int_gen {
  // By default, random integral numbers are generated between the minimum
  // and maximum possible values.
  random_int_gen (
      const T lower = std::numeric_limits<T>::min(),
      const T upper = std::numeric_limits<T>::max()
  ) {
    // Create a random device to seed the generator
    std::random_device rd;

    // Use the Mersenne Twister engine
    gen = std::mt19937 (rd());

    // Define distributions
    dist = std::uniform_int_distribution<T> (lower, upper);
  }

  T
  operator() () {
    return dist (gen);
  }

  std::mt19937                     gen;
  std::uniform_int_distribution<T> dist;
};

template <typename T> struct random_float_gen {
  // By default, random floating point numbers are generated between 0.0 and 1.0
  random_float_gen (const T lower = T (0.), const T upper = T (1.)) {
    // Create a random device to seed the generator
    std::random_device rd;

    // Use the Mersenne Twister engine
    gen = std::mt19937 (rd());

    // Define distributions
    if constexpr (!std::is_floating_point_v<T>)
      throw std::runtime_error ("The template parameter should be floating_point");

    dist = std::uniform_real_distribution<T> (lower, upper);
  }

  T
  operator() () {
    return dist (gen);
  }

  std::mt19937                      gen;
  std::uniform_real_distribution<T> dist;
};

struct random_string_gen {
  random_string_gen (const uint8_t max_len = 40) {
    // The length of the random string is randomly picked between 0 and 40.
    dice_len = random_int_gen<uint8_t>{0, max_len};
  }

  std::string
  operator() () {
    uint8_t     len = dice_len();
    std::string random_str (len, ' ');

    // Random characters are generated between 48(0x30) and 122(0x7a) which
    // correspond to '0' and 'z', respectively.
    random_int_gen<uint8_t> dice = random_int_gen<uint8_t>{48, 122};

    std::generate (random_str.begin(), random_str.end(), dice);
    return random_str;
  }

  // Random number generator for the length of the arbitrary string generated.
  random_int_gen<uint8_t> dice_len;
};

}  // namespace cch
