#include "custom_math.h"

size_t round_up(const size_t number, const size_t roundTo) {
  if (roundTo == 0) return number;
  return ((number + roundTo - 1) / roundTo) * roundTo;
}