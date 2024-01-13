#include "color_ramp.h"

#include <algorithm>

// Function to map velocity to color
SDL_Color velocityToColor(float speed, float maxVelocity) {
  float ratio = speed / maxVelocity;
  ratio = std::min(ratio, 1.0f); // Ensure the ratio is between 0 and 1

  Uint8 red = static_cast<Uint8>(255 * ratio);
  Uint8 green = 0;
  Uint8 blue = static_cast<Uint8>(255 * (1 - ratio)); // Keeping blue constant

  return {red, green, blue, 255};
}