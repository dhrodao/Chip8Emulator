#include "chip8.h"

#include <time.h>
#include <math.h>

#include <SDL3/SDL.h>

typedef struct {
    SDL_Window* w;
    SDL_Renderer* r;
    SDL_Texture* t;
} window_context;

/* ---- Defines ----*/

#define SECOND_TO_US    1000000

/* ---- Functions to handle Main Window ---- */

int window_init(window_context* ctx)
{
    /* Init SDL Subsystems */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    /* Create SDL Window */
    ctx->w = SDL_CreateWindow("Chip8 Emulator", 1024, 512, 0);
    if(ctx->w == NULL)
    {
        return 1;
    }

    /* Create renderer */
    ctx->r = SDL_CreateRenderer(ctx->w, NULL, 0);

    /* Create texture */
    ctx->t = SDL_CreateTexture(ctx->r, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, DISP_W, DISP_H);

    return 0;
}

void window_draw(window_context* ctx, Chip8* chip8)
{
    SDL_FRect rect;
    void* pixels;
    int pitch;

    SDL_RenderClear(ctx->r);

    if(SDL_LockTexture(ctx->t, NULL, &pixels, &pitch))
    {
        /* Ignore error */
        return;
    }

    for(int i = 0; i < DISP_H;  i++)
    {
        for(int j = 0; j < DISP_W; j++)
        {
            ((uint32_t*)pixels)[i * DISP_W + j] = (chip8->display[i][j] == 1) ? 0xFFFFFFFF : 0x00000000;
        }
    }

    SDL_UnlockTexture(ctx->t);

    rect.x = 0;
    rect.y = 0;
    rect.w = 1024;
    rect.h = 512;

    SDL_RenderTexture(ctx->r, ctx->t, NULL, &rect);

    SDL_RenderPresent(ctx->r);
}

int main_loop(window_context* ctx, Chip8* chip8)
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

        window_draw(ctx, chip8);

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
    window_context ctx;

    Chip8* chip8 = c8_init();

    /* Load Chip8 ROM */
    if(chip8->load_rom("test_opcode.ch8") != 0)
    {
        return 1;
    }

    /* Init SDL Window */
    if(window_init(&ctx) != 0)
    {
        return 1;
    }
    
    main_loop(&ctx, chip8);

    window_deinit(ctx.w);

#ifdef DEBUG

    debug_display(chip8);

#endif

    return 0;
}