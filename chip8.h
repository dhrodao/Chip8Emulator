/* Type Definition */

#define UBIT8    unsigned char
#define UBIT16   unsigned int

typedef enum {
    STD_FALSE = 0,
    STD_TRUE  = 1
} STD_BOOL;

typedef void (*loop_fn)(void); /* Loop function pointer */
typedef int (*load_rom_fn)(char*); /* Load ROM function pointer */

/* Chip-8 */

#define DISP_W 64
#define DISP_H 32

#define KEYBOARD_SIZE 4

typedef struct
{
    UBIT16 pc;            /* PC (Program Counter) */
    UBIT16 sp;            /* SP (Stack Pointer) */
    UBIT16 opcode;        /* Opcode */
    UBIT16 index;         /* Index register */
    UBIT16 delay_timer;   /* Delay Timer (60Hz freq) */
    UBIT16 sound_timer;   /* Sound Timer (60Hz freq) */
    UBIT8  memory[4096];  /* 4096 bytes */
    UBIT8  registers[16]; /* 16 8bit registers (V0...VF) */
    UBIT16 stack[16];     /* Stack (up to 16 nested levels) */
    UBIT8  display[DISP_H][DISP_W]; /* Display is DISP_WxDISP_H pixels */
    UBIT8  keyboard[KEYBOARD_SIZE * KEYBOARD_SIZE]; /* 0...9 A...F */

    load_rom_fn load_rom; /* Function to load the ROM (Parameters: char* filename) */
    loop_fn loop; /* CPU Cycle Function */
} Chip8;

/* This function inits the Chip8 structure and returns it */
Chip8* c8_init();