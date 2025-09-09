#pragma once
#include <algorithm>
#include <functional>
#include <vector>

namespace tabula {

struct QuickSort {
  template <typename T> static void sort(std::vector<T> &v) {
    sort(v, std::less<T>());
  }

  template <typename T, typename Compare>
  static void sort(std::vector<T> &v, Compare cmp) {
    if (v.size() <= 1)
      return;
    quicksort(v, 0, static_cast<int>(v.size()), cmp);
  }

private:
  template <typename T, typename Compare>
  static void quicksort(std::vector<T> &v, int left, int right, Compare cmp) {
    if (right - left < 2)
      return;
    int p = left + ((right - left) / 2);
    p = partition(v, cmp, p, left, right);
    quicksort(v, cmp, left, p);
    quicksort(v, cmp, p + 1, right);
  }

  template <typename T, typename Compare>
  static int partition(std::vector<T> &v, Compare cmp, int p, int start,
                       int end) {
    int l = start;
    int h = end - 2;
    T piv = v[p];
    std::swap(v[p], v[end - 1]);
    while (l < h) {
      if (!cmp(v[l], piv)) {
        if (cmp(piv, v[h])) {
          --h;
        } else {
          std::swap(v[l], v[h]);
        }
      } else {
        ++l;
      }
    }
    int idx = h;
    if (cmp(v[h], piv))
      idx++;
    std::swap(v[end - 1], v[idx]);
    return idx;
  }
};

} // namespace tabula
