#include <SDL2_gfxPrimitives.h>
#include <algorithm>
#include <iostream>

#include "constants.h"
#include "particle.h"

void DrawCircle(SDL_Renderer *renderer, Circle& circle, SDL_Color color) {
  aacircleRGBA(renderer, circle.x, circle.y, circle.radius, color.r, color.g, color.b,
               color.a);
}

void UpdateCircle(Circle &circle, float deltaTime, int window_width,
                  int window_height, int barrier_radius) {
  circle.x += circle.vx * deltaTime;
  circle.y += circle.vy * deltaTime;

  int leftEdge = (window_width - barrier_radius) / 2;
  int rightEdge = (window_width + barrier_radius) / 2;
  int topEdge = (window_height - barrier_radius) / 2;
  int bottomEdge = (window_height + barrier_radius) / 2;

  // Check collision with left and right edges of the barrier
  if (circle.x - CIRCLE_RADIUS < leftEdge ||
      circle.x + CIRCLE_RADIUS > rightEdge) {
    circle.vx *= -1;
    circle.x = std::max(
        (float)(leftEdge + CIRCLE_RADIUS),
        std::min(circle.x,
                 (float)(rightEdge - CIRCLE_RADIUS))); // Correct position
  }

  // Check collision with top and bottom edges of the barrier
  if (circle.y - CIRCLE_RADIUS < topEdge ||
      circle.y + CIRCLE_RADIUS > bottomEdge) {
    circle.vy *= -1;
    circle.y = std::max(
        (float)(topEdge + CIRCLE_RADIUS),
        std::min(circle.y,
                 (float)(bottomEdge - CIRCLE_RADIUS))); // Correct position
  }
}

void CheckCollision(Circle &circle1, Circle &circle2) {
  float dx = circle2.x - circle1.x;
  float dy = circle2.y - circle1.y;
  float distance = sqrt(dx * dx + dy * dy);

  if (distance < 2 * CIRCLE_RADIUS) {
    // Calculate the overlap distance
    float overlap = 2 * CIRCLE_RADIUS - distance;

    // Normalize the distance vector
    float nx = dx / distance;
    float ny = dy / distance;

    // Apply a repulsion force proportional to the overlap
    // The force increases as the circles get closer
    float forceStrength =
        overlap * 5.0f;

    // Update velocities based on the repulsion force
    circle1.vx -= nx * forceStrength;
    circle1.vy -= ny * forceStrength;
    circle2.vx += nx * forceStrength;
    circle2.vy += ny * forceStrength;
  }
}

std::vector<Circle> generate_random_circles(int count, int window_width,
                                            int window_height,
                                            int barrier_radius,
                                            int circle_radius) {
  std::vector<Circle> circles;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  int innerWidth = barrier_radius * 0.9f;
  int innerHeight = barrier_radius * 0.9f;
  int leftEdge = (window_width - innerWidth) / 2 + circle_radius;
  int topEdge = (window_height - innerHeight) / 2 + circle_radius;

  const int maxAttempts =
      100; // Maximum attempts to find a non-overlapping position

  for (int i = 0; i < count; ++i) {
    bool validPosition = false;
    float x, y;
    int attempts = 0;

    while (!validPosition && attempts < maxAttempts) {
      x = leftEdge + dis(gen) * (innerWidth - 2 * circle_radius);
      y = topEdge + dis(gen) * (innerHeight - 2 * circle_radius);
      validPosition = true;

      // Check for overlap with existing circles
      for (const auto &existingCircle : circles) {
        float dx = x - existingCircle.x;
        float dy = y - existingCircle.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance < 2 * circle_radius * 1.1) { // 10% more than the diameter
          validPosition = false;
          break;
        }
      }
      ++attempts;
    }

    if (validPosition) {
      float vx = dis(gen) * 200.0f - 100.0f;
      float vy = dis(gen) * 200.0f - 100.0f;
      circles.emplace_back(x, y, vx, vy);
    }
  }

  return circles;
}
