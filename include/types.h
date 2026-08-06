//
// types.h
//
// Definitions for types
//

#ifndef _CCH_TYPES_H_
#define _CCH_TYPES_H_

#include <concepts>
#include <cstddef>
#include <iterator>
#include <utility>

namespace cch {

// concept: Indexable
template <typename T>
concept Indexable = requires (T a, std::size_t i) {
  typename T::value_type;
  { a[i] } -> std::same_as<typename T::value_type&>;
  { std::as_const (a)[i] } -> std::same_as<const typename T::value_type&>;
};

// Concept: does the container have begin() and end()?
template <typename Container>
concept HasBeginEnd = requires (Container c) {
  { c.begin() } -> std::forward_iterator;
  { c.end() } -> std::same_as<decltype (c.begin())>;
};

// Concept: does the container expose continuous raw storage?
template <typename Container>
concept ContiguousByteContainer = requires (Container c) {
  { c.data() } -> std::same_as<typename Container::value_type*>;
  { c.size() } -> std::convertible_to<std::size_t>;
  c.resize (std::size_t{});
};

}  // namespace cch

#endif
