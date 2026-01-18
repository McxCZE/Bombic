#ifndef _SDLWRAPPER_H_
#define _SDLWRAPPER_H_

#include <SDL.h>
#include <memory>

// Custom deleters for SDL resources
struct SDLSurfaceDeleter {
    void operator()(SDL_Surface* surface) const {
        if (surface) SDL_FreeSurface(surface);
    }
};

struct SDLWindowDeleter {
    void operator()(SDL_Window* window) const {
        if (window) SDL_DestroyWindow(window);
    }
};

struct SDLRendererDeleter {
    void operator()(SDL_Renderer* renderer) const {
        if (renderer) SDL_DestroyRenderer(renderer);
    }
};

struct SDLTextureDeleter {
    void operator()(SDL_Texture* texture) const {
        if (texture) SDL_DestroyTexture(texture);
    }
};

// Type aliases for smart pointers with SDL resources
using SDLSurfacePtr = std::unique_ptr<SDL_Surface, SDLSurfaceDeleter>;
using SDLWindowPtr = std::unique_ptr<SDL_Window, SDLWindowDeleter>;
using SDLRendererPtr = std::unique_ptr<SDL_Renderer, SDLRendererDeleter>;
using SDLTexturePtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;

// Helper function to create managed surface
inline SDLSurfacePtr make_surface(SDL_Surface* surface) {
    return SDLSurfacePtr(surface);
}

// Helper function to create managed window
inline SDLWindowPtr make_window(SDL_Window* window) {
    return SDLWindowPtr(window);
}

#endif // _SDLWRAPPER_H_
