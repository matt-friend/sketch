/* Display blue sky for 10 seconds. */

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

// Fail, printing the SDL error message, and stop the program.
void fail() {
    fprintf(stderr, "%s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
}

// Check the results from SDL functions for errors.
int I(int n) { if (n < 0) fail(); return n; }
void *P(void *p) { if (p == NULL) fail(); return p; }

int main() {
    int w=640, h=480;
    I(SDL_Init(SDL_INIT_VIDEO));
    SDL_Window *window = P(SDL_CreateWindow("Sky", 100, 100, w, h, 0));

    SDL_Surface *surface = P(SDL_GetWindowSurface(window));
    Uint32 skyBlue = SDL_MapRGB(surface->format, 100, 149, 237);
    I(SDL_FillRect(surface, NULL, skyBlue));
    I(SDL_UpdateWindowSurface(window));

    SDL_Delay(10000);
    SDL_Quit();
    return 0;
}
