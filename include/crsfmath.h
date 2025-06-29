#ifndef CRSF_MATH_H
#define CRSF_MATH_H

#include <Arduino.h>

struct Point {
  int16_t x;  // e.g., RPM
  int16_t y;  // e.g., fuel flow
};

// Interpolates y from x using a lookup table
int16_t interpolateFromTable(int16_t x, const Point* table, uint8_t size) {
  if (x <= table[0].x) return table[0].y;
  if (x >= table[size - 1].x) return table[size - 1].y;

  for (uint8_t i = 1; i < size; i++) {
    if (x < table[i].x) {
      int16_t x0 = table[i - 1].x;
      int16_t y0 = table[i - 1].y;
      int16_t x1 = table[i].x;
      int16_t y1 = table[i].y;
      return y0 + ((int32_t)(x - x0) * (y1 - y0)) / (x1 - x0);
    }
  }
  return table[size - 1].y; // fallback
}


#endif