#include "chip8.h"

#include <time.h>
#include <math.h>

#include <SDL3/SDL.h>

/* ---- Defines ----*/

#define SECOND_TO_US    1000000

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

    struct timespec last_time;
    struct timespec curr_time;
    uint64_t timedelta_us = 0;
    int intructions_per_60_hz = 0;

    if(clock_gettime(CLOCK_MONOTONIC_RAW, &last_time))
    {
        // TODO Log
        return 1;
    }

    while(quit == STD_FALSE)
    { 
        /* Calculate timedelta */
        if(clock_gettime(CLOCK_MONOTONIC_RAW, &curr_time))
        {
            // TODO Log
            return 1;
        }

        timedelta_us += (curr_time.tv_sec - last_time.tv_sec) * 1000000 +
                        (curr_time.tv_nsec - last_time.tv_nsec) / 1000;

        /* Update timers with a 60Hz frequency */
        if(timedelta_us > (1.0 / 60.0) * SECOND_TO_US)
        {
            /* Update the timers */
            if(chip8->delay_timer > 0)
            {
                chip8->delay_timer--;
            }

            if(chip8->sound_timer > 0)
            {
                chip8->sound_timer--;
                if(chip8->sound_timer == 0)
                {
                    // TODO play sound
                    printf("Beeep\n");
                }
            }

            timedelta_us -= (1.0 / 60.0) * SECOND_TO_US;
            intructions_per_60_hz = 0;
        }

        /* Run CPU Cycle */
        chip8->loop();

        while(SDL_PollEvent(&e))
        {
            /* Handle window events */
            if(e.type == SDL_EVENT_QUIT)
            {
                quit = STD_TRUE; 
            }
        } 

        intructions_per_60_hz++;

        last_time = curr_time;
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

void debug_display(Chip8* chip8)
{
    for(int i = 0; i < DISP_H; i++)
    {
        for(int j = 0; j < DISP_W; j++)
            (chip8->display[i][j] == 1)? printf("X"): printf(" ");
        printf("\n");
    }
}

/* ---- Main Function ---- */

int main()
{
    SDL_Window *w = NULL;
    SDL_Surface* screenSurface = NULL;

    Chip8* chip8 = c8_init();

    /* Load Chip8 ROM */
    if(chip8->load_rom("test_opcode.ch8") != 0)
    {
        return 1;
    }

    /* Init SDL Window */
    if(window_init(w, screenSurface) != 0)
    {
        return 1;
    }
    
    main_loop(chip8);

    window_deinit(w);

    debug_display(chip8);

    return 0;
}