#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <iostream>
#include <math.h>
#include <vector>

#include "color_ramp.h"
#include "constants.h"
#include "particle.h"

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(
      "SDL Particle with ImGui", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError()
              << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // ImGui initialization
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  ImGui::StyleColorsDark();
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  std::vector<Particle> particles =
      generate_random_particles(INITIAL_PARTICLE_COUNT, WINDOW_WIDTH,
                                WINDOW_HEIGHT, BARRIER_RADIUS, PARTICLE_RADIUS);

  bool quit = false;
  SDL_Event event;
  Uint32 lastFrame = SDL_GetTicks(), currentFrame;
  float deltaTime = 0.0f;

  const int TARGET_FPS = 120;
  const int FRAME_DELAY = 1000 / TARGET_FPS;
  int frameTime = 0;

  static float posX = static_cast<float>(WINDOW_WIDTH) / 2;
  static float posY = static_cast<float>(WINDOW_HEIGHT) / 2;
  static float velX = 0.0f;
  static float velY = 0.0f;
  static float temperature = 0.0f;
  static float radius = 10.0f;

  while (!quit) {
    currentFrame = SDL_GetTicks();
    deltaTime = (currentFrame - lastFrame) / 1000.0f;
    lastFrame = currentFrame;

    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT) {
        quit = true;
      }
    }

    // Simulation update
    for (size_t i = 0; i < particles.size(); ++i) {
      UpdateParticle(particles[i], deltaTime, WINDOW_WIDTH, WINDOW_HEIGHT,
                     BARRIER_RADIUS);
      checkBarrierCollision(particles[i], WINDOW_WIDTH, WINDOW_HEIGHT,
                            BARRIER_RADIUS);
      for (size_t j = i + 1; j < particles.size(); ++j) {
        CheckCollision(particles[i], particles[j]);
      }
    }

    frameTime += SDL_GetTicks() - currentFrame;

    if (frameTime >= FRAME_DELAY) {
      // Rendering
      ImGui_ImplSDL2_NewFrame(window);
      ImGui_ImplSDLRenderer2_NewFrame();
      ImGui::NewFrame();

      // ImGui UI
      ImGui::Begin("Control Panel");
      // Add ImGui widgets here.
      if (ImGui::Button("Delete All Particles")) {
        particles.clear(); // Clears all elements from the particles vector
      }

      // Labeled input fields for new particle properties
      ImGui::Text("New Particle Properties");
      ImGui::SliderFloat("Position X", &posX, 0.0f, WINDOW_WIDTH);
      ImGui::SliderFloat("Position Y", &posY, -100.0f, WINDOW_HEIGHT);
      ImGui::SliderFloat("Velocity X", &velX, -100.0f, 100.0f);
      ImGui::SliderFloat("Velocity Y", &velY, -100.0f, 100.0f);
      ImGui::SliderFloat("Temperature", &temperature, 1.0f, 30.0f);
      ImGui::SliderFloat("Radius", &radius, 1.0f, 30.0f);

      // Button to add the particle
      if (ImGui::Button("Add Particle")) {
        Particle newParticle(posX, posY, velX, velY,
                             temperature, radius);     // Creating a new Particle instance
        particles.push_back(newParticle); // Add the new particle to the vector
      }

      ImGui::End();

      float averageVelocity = 0.0f;
      if (!particles.empty()) {
        float totalVelocity = 0.0f;
        for (const Particle &particle : particles) {
          totalVelocity +=
              sqrt(particle.vx * particle.vx + particle.vy * particle.vy);
        }
        averageVelocity = totalVelocity / particles.size();
      }

      ImGui::Begin("Particle Information");

      ImGui::Text("Average Particle Velocity: %.2f", averageVelocity);

      // Assuming you have an ImGui plot function available
      static std::vector<float> velocities;
      velocities.push_back(averageVelocity); // Store the average velocity
      if (velocities.size() > 50) { // Limit the number of displayed points
        velocities.erase(velocities.begin());
      }
      ImGui::PlotLines("Velocity", velocities.data(), velocities.size());

      ImGui::End();

      SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
      SDL_RenderClear(renderer);

      // Draw barrier rectangle
      SDL_Color barrierColor = {0, 0, 0, 255};
      SDL_SetRenderDrawColor(renderer, barrierColor.r, barrierColor.g,
                             barrierColor.b, barrierColor.a);

      SDL_Rect barrierRect;
      barrierRect.x = WINDOW_WIDTH / 2 - BARRIER_RADIUS / 2;
      barrierRect.y = WINDOW_HEIGHT / 2 - BARRIER_RADIUS / 2;
      barrierRect.w = BARRIER_RADIUS; // Width of the rectangle
      barrierRect.h = BARRIER_RADIUS; // Height of the rectangle

      SDL_RenderDrawRect(renderer, &barrierRect);

      // Update and draw moving particles
      float maxVelocity =
          100.0f; // Adjust this based on your particles' velocity range
      for (size_t i = 0; i < particles.size(); ++i) {
        UpdateParticle(particles[i], deltaTime, WINDOW_WIDTH, WINDOW_HEIGHT,
                       BARRIER_RADIUS);
        for (size_t j = i + 1; j < particles.size(); ++j) {
          CheckCollision(particles[i], particles[j]);
        }
        calculateParticleVelocity(particles[i]);
        SDL_Color particleColor =
            velocityToColor(particles[i].speed, maxVelocity);
        DrawParticle(renderer, particles[i], particleColor);
      }

      ImGui::Render();
      ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
      SDL_RenderPresent(renderer);

      frameTime = 0; // Reset frame time
    }
  }

  // Cleanup
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
