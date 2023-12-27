#ifndef PARTICLE_H
#define PARTICLE_H

#include "constants.h"
#include <SDL.h>
#include <random>
#include <vector>

struct Circle {
  float x, y;   // Position
  float vx, vy; // Velocity
  float radius;

  Circle(float x, float y, float vx, float vy, float radius=CIRCLE_RADIUS) : x(x), y(y), vx(vx), vy(vy),radius(radius) {}
};

void DrawCircle(SDL_Renderer *renderer, Circle& circle, SDL_Color color);

void UpdateCircle(Circle &circle, float deltaTime, int window_width,
                  int window_height, int barrier_radius);

void CheckCollision(Circle &circle1, Circle &circle2);

std::vector<Circle> generate_random_circles(int count, int window_width,
                                            int window_height,
                                            int barrier_radius,
                                            int circle_radius);

#endif // PARTICLE_H