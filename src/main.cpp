#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include <iostream>
#include <math.h>
#include <vector>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int CIRCLE_RADIUS = 10;
const int BARRIER_RADIUS = (WINDOW_HEIGHT * 4) / 5 / 2;
const int BARRIER_THICKNESS = 10;

struct Circle {
  float x, y;   // Position
  float vx, vy; // Velocity
};

void DrawCircle(SDL_Renderer *renderer, int centerX, int centerY, int radius,
                SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  for (int w = 0; w < radius * 2; w++) {
    for (int h = 0; h < radius * 2; h++) {
      int dx = radius - w; // Distance from center to pixel
      int dy = radius - h;
      if ((dx * dx + dy * dy) <= (radius * radius)) {
        SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
      }
    }
  }
}

void UpdateCircle(Circle &circle, float deltaTime) {
  // Update position
  circle.x += circle.vx * deltaTime;
  circle.y += circle.vy * deltaTime;

  // Wall collision
  if (circle.x - CIRCLE_RADIUS < 0 || circle.x + CIRCLE_RADIUS > WINDOW_WIDTH) {
    circle.vx *= -1;
  }
  if (circle.y - CIRCLE_RADIUS < 0 ||
      circle.y + CIRCLE_RADIUS > WINDOW_HEIGHT) {
    circle.vy *= -1;
  }

  // Barrier collision
  float dx = circle.x - WINDOW_WIDTH / 2;
  float dy = circle.y - WINDOW_HEIGHT / 2;
  float distance = sqrt(dx * dx + dy * dy);
  if (distance + CIRCLE_RADIUS > BARRIER_RADIUS) {
    // Reflect the velocity
    circle.vx *= -1;
    circle.vy *= -1;
  }
}

void CheckCollision(Circle &circle1, Circle &circle2) {
  float dx = circle2.x - circle1.x;
  float dy = circle2.y - circle1.y;
  float distance = sqrt(dx * dx + dy * dy);
  if (distance < 2 * CIRCLE_RADIUS) {
    // Elastic collision response
    std::swap(circle1.vx, circle2.vx);
    std::swap(circle1.vy, circle2.vy);
  }
}

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
              << std::endl;
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(
      "SDL Circle with ImGui", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

  std::vector<Circle> circles = {
      {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 100.0f, 60.0f},
      {WINDOW_WIDTH / 3, WINDOW_HEIGHT / 3, 80.0f, 100.0f},
      {2 * WINDOW_WIDTH / 3, WINDOW_HEIGHT / 3, 60.0f, 80.0f},
      {WINDOW_WIDTH / 3, 2 * WINDOW_HEIGHT / 3, 100.0f, 60.0f},
      {2 * WINDOW_WIDTH / 3, 2 * WINDOW_HEIGHT / 3, 80.0f, 100.0f}};

  bool quit = false;
  SDL_Event event;
  Uint32 frameStart, frameTime;
  const int TARGET_FPS = 60;
  const int FRAME_DELAY = 1000 / TARGET_FPS;
  float deltaTime = 0.0f;

  while (!quit) {
    frameStart = SDL_GetTicks();

    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT) {
        quit = true;
      }
    }

    ImGui_ImplSDL2_NewFrame(window);
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui::NewFrame();

    // ImGui UI
    ImGui::Begin("Control Panel");
    // Add ImGui widgets here.
    ImGui::End();

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    // Draw barrier circle
    SDL_Color barrierColor = {0, 0, 0, 255};
    for (int r = BARRIER_RADIUS - BARRIER_THICKNESS; r <= BARRIER_RADIUS; ++r) {
      DrawCircle(renderer, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, r,
                 barrierColor);
    }

    // Update and draw moving circles
    SDL_Color circleColor = {255, 0, 0, 255};
    for (size_t i = 0; i < circles.size(); ++i) {
      UpdateCircle(circles[i], deltaTime);
      for (size_t j = i + 1; j < circles.size(); ++j) {
        CheckCollision(circles[i], circles[j]);
      }
      DrawCircle(renderer, static_cast<int>(circles[i].x),
                 static_cast<int>(circles[i].y), CIRCLE_RADIUS, circleColor);
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);

    frameTime = SDL_GetTicks() - frameStart;
    deltaTime = frameTime / 1000.0f; // Convert to seconds
    if (frameTime < FRAME_DELAY) {
      SDL_Delay(FRAME_DELAY - frameTime);
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
