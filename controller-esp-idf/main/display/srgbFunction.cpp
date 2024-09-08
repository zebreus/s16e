#include "srgbFunction.hpp"
#include <array>
#include <cmath>
constexpr unsigned char srgbFunction(unsigned char x) {
  return std::floor(std::pow(x / 255.0, 2.2) * 255.0);
};
unsigned char srgbToLinear(unsigned char srgb) {
  constexpr std::array srgbEncodingTable{
      []<auto... I>(std::index_sequence<I...>) {
        return std::array<unsigned char, 256>{srgbFunction(I)...};
      }(std::make_index_sequence<256>{})};
  return srgbEncodingTable[srgb];
  // return srgb;
}