#ifndef PARTICLE_H
#define PARTICLE_H

#include "constants.h"
#include <SDL.h>
#include <random>
#include <vector>

struct Particle {
  float x, y;   // Position
  float vx, vy; // Velocity
  float speed;
  float radius;
  float temperature;

  Particle(float x, float y, float vx, float vy, float temperature, float radius = PARTICLE_RADIUS)
      : x(x), y(y), vx(vx), vy(vy), temperature(temperature), radius(radius) {}
};

void calculateParticleVelocity(Particle &particle);

void DrawParticle(SDL_Renderer *renderer, Particle &particle, SDL_Color color);

void UpdateParticle(Particle &particle, float deltaTime, int window_width,
                  int window_height, int barrier_radius);

void checkBarrierCollision(Particle &particle, int window_width, int window_height,
                           int barrier_radius);

void CheckCollision(Particle &particle1, Particle &particle2);

std::vector<Particle> generate_random_particles(int count, int window_width,
                                            int window_height,
                                            int barrier_radius,
                                            int particle_radius);

#endif // PARTICLE_H