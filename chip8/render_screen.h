#include <SDL2/SDL.h>
#include <stdint.h>
#include "state.h"

// Structure to hold SDL components
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} RenderContext;

// Function declarations
int render_initialise(RenderContext *ctx, char const *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
void render_update(RenderContext *ctx, void const *buffer, int pitch);
int render_process_input(uint8_t *keys);
void render_cleanup(RenderContext *ctx);
