#include <vector>
#include <type_traits>

/// Recursively get the nesting level of a collection `T`.
template <class T>
struct CollectionNestLevel {
  /// Overload picked for a TT that:
  /// - has TT::const_iterator
  /// - has TT::begin()
  /// - has TT::end()
  /// - has TT::value_type
  /// This is done by SFINAEing away this overload if any of the
  /// template parameter declaration turns out to fail.
  ///
  /// Returns 1 + whatever nesting level the element type has.
  template <class TT,
            class IT = typename TT::const_iterator,
            class = decltype(std::declval<TT>().begin()),
            class = decltype(std::declval<TT>().end()),
            class VT = typename TT::value_type>
  static constexpr int get(TT*) {
      return 1 + CollectionNestLevel<VT>::value;
  }

  /// The "this is not a collection" overload.
  static constexpr int get(...) { return 0; }

  /// Holds the nesting level, as determined by `get()`.
  static constexpr int value
    = get(std::add_pointer_t<T>(nullptr));
};

template <class T>
constexpr int CollectionNestLevel_v = CollectionNestLevel<T>::value;


////// TEST CASE:
template <class T>
using v = std::vector<T>;
using t = v<v<v<v<v<int>>>>>;
int TEST = CollectionNestLevel_v<t>;
