#pragma once

namespace tabula {

template <typename L, typename R> struct Pair {
  L first;
  R second;
  Pair() = default;
  Pair(const L &l, const R &r) : first(l), second(r) {}
  const L &getLeft() const { return first; }
  const R &getRight() const { return second; }
};

} // namespace tabula
