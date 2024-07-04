#include "render_screen.h"

// Initialise the SDL components
int render_initialise(RenderContext *ctx, char const *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialise! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    ctx->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (ctx->window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_ACCELERATED);
    if (ctx->renderer == NULL) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(ctx->window);
        return -1;
    }

    ctx->texture = SDL_CreateTexture(ctx->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
    if (ctx->texture == NULL) {
        fprintf(stderr, "Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(ctx->renderer);
        SDL_DestroyWindow(ctx->window);
        return -1;
    }

    return 0;
}

// Update the screen with the current buffer
void render_update(RenderContext *ctx, void const *buffer, int pitch) {
    SDL_UpdateTexture(ctx->texture, NULL, buffer, pitch);
    SDL_RenderClear(ctx->renderer);
    SDL_RenderCopy(ctx->renderer, ctx->texture, NULL, NULL);
    SDL_RenderPresent(ctx->renderer);
}

// Process input and update the keys array
int render_process_input(uint8_t *keys) {
    SDL_Event event;
    int quit = 0;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: quit = 1; break;
                    case SDLK_x: keys[0] = 1; break;
                    case SDLK_1: keys[1] = 1; break;
                    case SDLK_2: keys[2] = 1; break;
                    case SDLK_3: keys[3] = 1; break;
                    case SDLK_q: keys[4] = 1; break;
                    case SDLK_w: keys[5] = 1; break;
                    case SDLK_e: keys[6] = 1; break;
                    case SDLK_a: keys[7] = 1; break;
                    case SDLK_s: keys[8] = 1; break;
                    case SDLK_d: keys[9] = 1; break;
                    case SDLK_z: keys[0xA] = 1; break;
                    case SDLK_c: keys[0xB] = 1; break;
                    case SDLK_4: keys[0xC] = 1; break;
                    case SDLK_r: keys[0xD] = 1; break;
                    case SDLK_f: keys[0xE] = 1; break;
                    case SDLK_v: keys[0xF] = 1; break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_x: keys[0] = 0; break;
                    case SDLK_1: keys[1] = 0; break;
                    case SDLK_2: keys[2] = 0; break;
                    case SDLK_3: keys[3] = 0; break;
                    case SDLK_q: keys[4] = 0; break;
                    case SDLK_w: keys[5] = 0; break;
                    case SDLK_e: keys[6] = 0; break;
                    case SDLK_a: keys[7] = 0; break;
                    case SDLK_s: keys[8] = 0; break;
                    case SDLK_d: keys[9] = 0; break;
                    case SDLK_z: keys[0xA] = 0; break;
                    case SDLK_c: keys[0xB] = 0; break;
                    case SDLK_4: keys[0xC] = 0; break;
                    case SDLK_r: keys[0xD] = 0; break;
                    case SDLK_f: keys[0xE] = 0; break;
                    case SDLK_v: keys[0xF] = 0; break;
                }
                break;
        }
    }

    return quit;
}

void render_cleanup(RenderContext *ctx) {
    	SDL_DestroyTexture(ctx->texture);
		SDL_DestroyRenderer(ctx->renderer);
		SDL_DestroyWindow(ctx->window);
		SDL_Quit();
}