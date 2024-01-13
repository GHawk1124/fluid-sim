#include <SDL2_gfxPrimitives.h>
#include <algorithm>
#include <iostream>

#include "constants.h"
#include "particle.h"

void calculateParticleVelocity(Particle &particle) {
  float speed = particle.speed;
  float vx = particle.vx;
  float vy = particle.vy;

  // Calculate the velocity vector's angle
  float angle = atan2(vy, vx);

  // Calculate the velocity vector's magnitude
  float magnitude = sqrt(vx * vx + vy * vy);

  // Calculate the velocity vector's magnitude based on the particle's
  // temperature
  float maxVelocity = 200.0f;
  float minVelocity = 0.0f;
  float maxTemperature = 100.0f;
  float minTemperature = 0.0f;
  float temperature = particle.temperature;
  float velocity = (temperature - minTemperature) / (maxTemperature - minTemperature) *
                   (maxVelocity - minVelocity) +
                   minVelocity;

  // Update the velocity vector
  vx = cos(angle) * velocity;
  vy = sin(angle) * velocity;

  particle.vx = vx;
  particle.vy = vy;
}

void DrawParticle(SDL_Renderer *renderer, Particle& particle, SDL_Color color) {
  aacircleRGBA(renderer, particle.x, particle.y, particle.radius, color.r, color.g, color.b,
               color.a);
}

void UpdateParticle(Particle &particle, float deltaTime, int window_width,
                  int window_height, int barrier_radius) {
  particle.x += particle.vx * deltaTime;
  particle.y += particle.vy * deltaTime;
  particle.speed = sqrt(particle.vx * particle.vx + particle.vy * particle.vy);
}

void checkBarrierCollision(Particle &particle, int window_width, int window_height,
                           int barrier_radius) {

  int leftEdge = (window_width - barrier_radius) / 2;
  int rightEdge = (window_width + barrier_radius) / 2;
  int topEdge = (window_height - barrier_radius) / 2;
  int bottomEdge = (window_height + barrier_radius) / 2;

  // Check collision with left and right edges of the barrier
  if (particle.x - particle.radius < leftEdge ||
      particle.x + particle.radius > rightEdge) {
    particle.vx *= -1;
    particle.x = std::max(
        (float)(leftEdge + particle.radius),
        std::min(particle.x,
                 (float)(rightEdge - particle.radius))); // Correct position
  }

  // Check collision with top and bottom edges of the barrier
  if (particle.y - particle.radius < topEdge ||
      particle.y + particle.radius > bottomEdge) {
    particle.vy *= -1;
    particle.y = std::max(
        (float)(topEdge + particle.radius),
        std::min(particle.y,
                 (float)(bottomEdge - particle.radius))); // Correct position
  }
}

void CheckCollision(Particle &particle1, Particle &particle2) {
  float dx = particle2.x - particle1.x;
  float dy = particle2.y - particle1.y;
  float distance = sqrt(dx * dx + dy * dy);

  if (distance < particle1.radius + particle2.radius) {
    // Calculate the overlap distance
    float overlap = (particle1.radius + particle2.radius) - distance;

    // Normalize the distance vector
    float nx = dx / distance;
    float ny = dy / distance;

    // Apply a repulsion force proportional to the overlap
    // The force increases as the particles get closer
    float forceStrength =
        overlap * 5.0f;

    // Update velocities based on the repulsion force
    particle1.vx -= nx * forceStrength;
    particle1.vy -= ny * forceStrength;
    particle2.vx += nx * forceStrength;
    particle2.vy += ny * forceStrength;
  }
}

std::vector<Particle> generate_random_particles(int count, int window_width,
                                            int window_height,
                                            int barrier_radius,
                                            int particle_radius) {
  std::vector<Particle> particles;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  int innerWidth = barrier_radius * 0.9f;
  int innerHeight = barrier_radius * 0.9f;
  int leftEdge = (window_width - innerWidth) / 2 + particle_radius;
  int topEdge = (window_height - innerHeight) / 2 + particle_radius;

  const int maxAttempts =
      100; // Maximum attempts to find a non-overlapping position

  for (int i = 0; i < count; ++i) {
    bool validPosition = false;
    float x, y;
    int attempts = 0;

    while (!validPosition && attempts < maxAttempts) {
      x = leftEdge + dis(gen) * (innerWidth - 2 * particle_radius);
      y = topEdge + dis(gen) * (innerHeight - 2 * particle_radius);
      validPosition = true;

      // Check for overlap with existing particles
      for (const auto &existingParticle : particles) {
        float dx = x - existingParticle.x;
        float dy = y - existingParticle.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance < 2 * particle_radius * 1.1) { // 10% more than the diameter
          validPosition = false;
          break;
        }
      }
      ++attempts;
    }

    if (validPosition) {
      float vx = dis(gen) * 200.0f - 100.0f;
      float vy = dis(gen) * 200.0f - 100.0f;
      particles.emplace_back(x, y, vx, vy, dis(gen) * 100.0f);
    }
  }

  return particles;
}
