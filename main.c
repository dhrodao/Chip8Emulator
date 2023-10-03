#include "chip8.h"

#include <SDL3/SDL.h>

/* ---- Functions to handle Main Window ---- */

int window_init(SDL_Window* w, SDL_Surface* screenSurface)
{
    /* Init SDL Subsystems */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    /* Create SDL Window */
    w = SDL_CreateWindow("Chip8 Emulator", 500, 500, 0);
    if(w == NULL)
    {
        return 1;
    }

    /* Create SDL Surface */
    screenSurface = SDL_GetWindowSurface(w);
    
    /* Fill Surface with white pixels */
    SDL_FillSurfaceRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));

    /* Update SDL Window surface */
    SDL_UpdateWindowSurface(w);

    return 0;
}

int main_loop(Chip8* chip8)
{
    STD_BOOL quit = STD_FALSE;
    SDL_Event e;

    while( quit == STD_FALSE )
    { 
        while(SDL_PollEvent(&e))
        {
            /* Run CPU Cycle */
            chip8->loop();

            /* Handle window events */
            if(e.type == SDL_EVENT_QUIT)
            {
                quit = STD_TRUE; 
            }
        } 
    }

    return 0;
}

void window_deinit(SDL_Window* w)
{
    /* Frees memory */
    SDL_DestroyWindow(w);
    
    /* Shuts down all SDL subsystems */
    SDL_Quit(); 
}

/* ---- Main Function ---- */

int main()
{
    SDL_Window *w = NULL;
    SDL_Surface* screenSurface = NULL;

    Chip8* chip8 = c8_init();

    if(window_init(w, screenSurface) != 0)
    {
        return 1;
    }
    
    main_loop(chip8);

    window_deinit(w);

    return 0;
}