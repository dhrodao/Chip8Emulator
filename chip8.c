#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

static Chip8 chip8;

enum {
    FONTSET_LEN = 16,
    FONTSET_SPRITE = 5
};

const UBIT8 c8_fontset[FONTSET_LEN * FONTSET_SPRITE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

/* This function increments the PC to the next position */
void c8_increment_pc()
{
    chip8.pc += 2; /* Increment in 16bits (2 bits)*/
}

/* This function increments the SP to the next position */
void c8_increment_sp()
{
    chip8.sp += 1; /* Increment in 16bits (2 bits)*/
}

/* This function decrements the SP to the previous position */
void c8_decrement_sp()
{
    chip8.sp -= 1; /* Increment in 16bits (2 bits)*/
}

/* This function clears the chip8 display */
void c8_clear_disp()
{
    for(int i = 0; i < DISP_H; i++)
        memset(&(chip8.display[i]), 0, sizeof(chip8.display[i]));
}

/* This function process the _cls_ instruction */
void c8_process_instruction_cls()
{
    c8_clear_disp();
}

/* This function process the _ret_ instruction */
void c8_process_instruction_ret()
{
    chip8.pc = chip8.stack[--chip8.sp];
}

/* This function process the instruction set 0 */
void c8_process_instruction_0()
{
    if(chip8.opcode == 0x00E0)
    {
        c8_process_instruction_cls();
    }

    if(chip8.opcode == 0x00EE)
    {
        c8_process_instruction_ret();
    }

    c8_increment_pc();
}

/* This function process the instruction jump to location */
void c8_process_instruction_1()
{
    chip8.pc = (chip8.opcode & 0x0FFF);
}

/* This function process the instruction call subroutine */
void c8_process_instruction_2()
{
    chip8.stack[chip8.sp] = chip8.pc;
    c8_increment_sp();
    chip8.pc = (chip8.opcode & 0x0FFF);
}

/* This function process the instruction skip next instruction if Vx == kk */
void c8_process_instruction_3()
{
    UBIT8  vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT16 kk = (chip8.opcode & 0x00FF);

    if(chip8.registers[vx] == kk)
    {
        c8_increment_pc();
    }

    c8_increment_pc();
}

/* This function process the instruction skip next instruction if Vx != kk */
void c8_process_instruction_4()
{
    UBIT8  vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT16 kk = (chip8.opcode & 0x00FF);

    if(chip8.registers[vx] != kk)
    {
        c8_increment_pc();
    }

    c8_increment_pc();
}

/* This function process the instruction skip next instruction if Vx == Vy */
void c8_process_instruction_5()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8 vy = (chip8.opcode & 0x00F0) >> 4;

    if(chip8.registers[vx] == chip8.registers[vy])
    {
        c8_increment_pc();
    }

    c8_increment_pc();
}

/* This function process the instruction LD Vx, byte */
void c8_process_instruction_6()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT16 kk = (chip8.opcode & 0x00FF);

    chip8.registers[vx] = kk;

    c8_increment_pc();
}

/* This function process the instruction ADD Vx, byte */
void c8_process_instruction_7()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT16 kk = (chip8.opcode & 0x00FF);

    chip8.registers[vx] += kk;

    c8_increment_pc();
}

/* This function stores the value Vy in Vx */
void c8_process_ld_reg()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8 vy = (chip8.opcode & 0x00F0) >> 4;

    chip8.registers[vx] = chip8.registers[vy];
}

/* This function performs a bitwise OR and stores the result in register Vx */
void c8_process_or()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8 vy = (chip8.opcode & 0x00F0) >> 4;

    chip8.registers[vx] = chip8.registers[vx] | chip8.registers[vy];
}

/* This function performs a bitwise AND and stores the result in register Vx */
void c8_process_and()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8 vy = (chip8.opcode & 0x00F0) >> 4;

    chip8.registers[vx] = chip8.registers[vx] & chip8.registers[vy];
}

/* This function performs a bitwise XOR and stores the result in register Vx */
void c8_process_xor()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8 vy = (chip8.opcode & 0x00F0) >> 4;

    chip8.registers[vx] = chip8.registers[vx] ^ chip8.registers[vy];
}

/* This function performs ADD Vx, Vy with carry (stores result in Vx) */
void c8_process_add_regs()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8 vy = (chip8.opcode & 0x00F0) >> 4;
    UBIT8 vf = 0xF;
    UBIT16 sum = 0x00;
    
    sum = chip8.registers[vx] + chip8.registers[vy];
    if(sum > 255)
    {
        chip8.registers[vf] = 1;
    }
    else
    {
        chip8.registers[vf] = 0;
    }

    chip8.registers[vx] = (sum & 0xFF);
}

/* This function performs SUB Vx, Vy and sets Vf = NOT borrow (stores result in Vx)*/
void c8_process_sub_regs()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8 vy = (chip8.opcode & 0x00F0) >> 4;
    UBIT8 vf = 0xF;

    if(chip8.registers[vx] > chip8.registers[vy])
    {
        chip8.registers[vf] = 1;
    }
    else
    {
        chip8.registers[vf] = 0;
    }

    chip8.registers[vx] -= chip8.registers[vy];
}

/* This function performs SHR Vx {, Vy} If the least-significant bit of Vx is 1, */
/* then VF is set to 1, otherwise 0. Then Vx is divided by 2.                    */
void c8_process_shr()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8 vf = 0xF;

    chip8.registers[vf] = (chip8.registers[vx] & 0x01) != 0 ? 1: 0;
    chip8.registers[vx] >>= 1;
}

/* This function performs SUBN Vx, Vy If Vy > Vx, then VF is set to 1, otherwise 0. */
/* Then Vx is subtracted from Vy, and the results stored in Vx                      */
void c8_process_subn_regs()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8 vy = (chip8.opcode & 0x00F0) >> 4;
    UBIT8 vf = 0xF;

    if(chip8.registers[vx] > chip8.registers[vy])
    {
        chip8.registers[vf] = 1;
    }
    else
    {
        chip8.registers[vf] = 0;
    }

    chip8.registers[vx] = chip8.registers[vy] - chip8.registers[vx];
}

/* This function performs SHL Vx {, Vy} If the most-significant bit of Vx is 1, */
/* then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.             */
void c8_process_shl()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8 vf = 0xF;

    chip8.registers[vf] = (chip8.registers[vx] & 0x80) != 0 ? 1 : 0;
    chip8.registers[vx] <<= 1;
}

/* This function process the instruction set 8 */
void c8_process_instruction_8()
{
    UBIT8 last = (chip8.opcode & 0x000F);

    switch (last)
    {
    case 0x0:
        c8_process_ld_reg();
        break;
    case 0x1:
        c8_process_or();
        break;
    case 0x2:
        c8_process_and();
        break;
    case 0x3:
        c8_process_xor();
        break;
    case 0x4:
        c8_process_add_regs();
        break;
    case 0x5:
        c8_process_sub_regs();
        break;
    case 0x6:
        c8_process_shr();
        break;
    case 0x7:
        c8_process_subn_regs();
        break;
    case 0xE:
        c8_process_shl();
        break;
    default:
        break;
    }

    c8_increment_pc();
}

/* This function performs SNE Vx, Vy */
void c8_process_instruction_9()
{
    UBIT8  vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8  vy = (chip8.opcode & 0x00F0) >> 4;

    if(chip8.registers[vx] != chip8.registers[vy])
    {
        c8_increment_pc();
    }
    c8_increment_pc();
}

/* This function performs LD I, addr */
void c8_process_instruction_A()
{
    chip8.index = (chip8.opcode & 0x0FFF);
    c8_increment_pc();
}

/* This function performs JP V0, addr */
void c8_process_instruction_B()
{
    /* Jump to nnn + V0 */
    chip8.pc = (chip8.opcode & 0x0FFF) + chip8.registers[0x0];
}

/* This function performs RND Vx, byte */
void c8_process_instruction_C()
{
    UBIT8  vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT16 kk = (chip8.opcode & 0x00FF);
    chip8.registers[vx] = (rand() % 256) & kk; /* random number [0, 255] AND kk */
    c8_increment_pc();
}

/* This function performs DRW Vx, Vy, nibble */
void c8_process_instruction_D()
{
    UBIT8 vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8 vy = (chip8.opcode & 0x00F0) >> 4;
    UBIT8 vz = 0xF;
    UBIT8 nibble = (chip8.opcode & 0x000F);

    chip8.registers[vz] = 0;

    UBIT8 y = 0;
    while(y < nibble)
    {
        UBIT8 pixel = chip8.memory[chip8.index + y];
        UBIT8 x = 0;
        while(x < 8)
        {
            UBIT8 msb = 0x80;
            if((pixel & (msb >> x)) != 0)
            {
                UBIT8 tX = (chip8.registers[vx] + x) % DISP_W;
                UBIT8 tY = (chip8.registers[vy] + y) % DISP_H;

                chip8.display[tY][tX] ^= 1;

                /* In case that the pixel has been deleted */
                if(chip8.display[tY][tX] == 0)
                {
                    chip8.registers[vz] = 1;
                }
            }
            x++;
        }
        y++;
    }

    c8_increment_pc();
}

/* This function performs the instruction SKP Vx */
void c8_process_instruction_skp()
{
    UBIT8  vx = (chip8.opcode & 0x0F00) >> 8;

    if(chip8.keyboard[chip8.registers[vx]] == 1)
    {
        c8_increment_pc();
    }
}

/* This function performs the instruction SKNP Vx */
void c8_process_instruction_sknp()
{
    UBIT8  vx = (chip8.opcode & 0x0F00) >> 8;

    if(chip8.keyboard[chip8.registers[vx]] != 1)
    {
        c8_increment_pc();
    }
}

/* This function process the instruction set E */
void c8_process_instruction_E()
{
    UBIT16 last = (chip8.opcode & 0x00FF);

    if(last == 0x9E)
    {
        c8_process_instruction_skp();
    }

    if(last == 0xA1)
    {
        c8_process_instruction_sknp();
    }

    c8_increment_pc();
}

/* This function waits until a key is pressed and sets Vx to the value of the key pressed */
void c8_process_keypress(const UBIT8 vx)
{
    STD_BOOL loop = STD_TRUE;

    while(STD_TRUE == loop)
    {
        // loop keys checking a pressed key
        for(UBIT8 i = 0; i < (KEYBOARD_SIZE * KEYBOARD_SIZE); i++)
        {
            if(chip8.keyboard[i] == 1)
            {
                chip8.registers[vx] = i;
                loop = STD_FALSE;
            }
        }
    }
}

/* This function process the instruction set F */
void c8_process_instruction_F()
{
    UBIT16 last = (chip8.opcode & 0x00FF);
    UBIT8  vx = (chip8.opcode & 0x0F00) >> 8;
    UBIT8  aux = 0x0;

    switch (last)
    {
    case 0x07:
        chip8.registers[vx] = chip8.delay_timer;
        break;
    case 0x0A:
        c8_process_keypress(vx);
        break;
    case 0x15:
        chip8.delay_timer = chip8.registers[vx];
        break;
    case 0x18:
        chip8.sound_timer = chip8.registers[vx];
        break;
    case 0x1E:
        chip8.registers[0xF] = (chip8.index + chip8.registers[vx] > 0xFFF) ? 1 : 0;
        chip8.index += chip8.registers[vx];
        break;
    case 0x29:
        chip8.index = chip8.registers[vx] * FONTSET_SPRITE;
        break;
    case 0x33:
        chip8.memory[chip8.index] = (chip8.registers[vx] / 100) % 10;
        chip8.memory[chip8.index + 1] = (chip8.registers[vx] / 10) % 10;
        chip8.memory[chip8.index + 2] = (chip8.registers[vx]) % 10;
        break;
    case 0x55:
        aux = 0x0;
        while(aux <= vx)
        {
            chip8.memory[chip8.index + aux] = chip8.registers[aux];
            aux += 0x1;
        }
        break;
    case 0x65:
        aux = 0x0;
        while(aux <= vx)
        {
            chip8.registers[aux] = chip8.memory[chip8.index + aux];
            aux += 0x1;
        }
        break;
    default:
        return;
    }

    c8_increment_pc();
}

/* This function processes an instruction contained in chip8.opcode */
void c8_process_instruction()
{
    UBIT8 first = chip8.opcode >> 12; /* Get first 4 bytes (instruction type)*/

    switch (first)
    {
    case 0x0:
        c8_process_instruction_0();
        break;
    case 0x1:
        c8_process_instruction_1();
        break;
    case 0x2:
        c8_process_instruction_2();
        break;
    case 0x3:
        c8_process_instruction_3();
        break;
    case 0x4:
        c8_process_instruction_4();
        break;
    case 0x5:
        c8_process_instruction_5();
        break;
    case 0x6:
        c8_process_instruction_6();
        break;
    case 0x7:
        c8_process_instruction_7();
        break;
    case 0x8:
        c8_process_instruction_8();
        break;
    case 0x9:
        c8_process_instruction_9();
        break;
    case 0xA:
        c8_process_instruction_A();
        break;
    case 0xB:
        c8_process_instruction_B();
        break;
    case 0xC:
        c8_process_instruction_C();
        break;
    case 0xD:
        c8_process_instruction_D();
        break;
    case 0xE:
        c8_process_instruction_E();
        break;
    case 0xF:
        c8_process_instruction_F();
        break;
    default:
        return;
    }
}

/* This function loads the chip8 rom memory */
int c8_load_rom(char *filename)
{
    FILE* f = NULL;
    size_t bytes_read = 0;
    size_t total_bytes_read = 0;
    UBIT8 buffer[128];
    UBIT8* p_mem = NULL;

    if(0 == strlen(filename))
    {
        return 1;
    }

    if(!(f = fopen(filename, "rb")))
    {
        return 1;
    }

    while((bytes_read = fread(buffer, 1, sizeof(buffer), f)))
    {
        if((total_bytes_read + bytes_read) >= 0xFFF - 0x200)
        {
            return 1;
        }

        p_mem = &(chip8.memory[0x200 + total_bytes_read]);
        memcpy((void*)p_mem, buffer, bytes_read);

        total_bytes_read += bytes_read;
    }

    fclose(f);

    return 0;
}

/* This function emulates a cycle of chip8 */
void c8_loop()
{
    chip8.opcode = (chip8.memory[chip8.pc] << 8) | chip8.memory[chip8.pc + 1];
    c8_process_instruction();
}

Chip8* c8_init()
{
    chip8.pc = 0x200;
    chip8.sp = 0;
    chip8.opcode = 0;
    chip8.index = 0;
    chip8.delay_timer = 0;
    chip8.sound_timer = 0;

    memset(&chip8.memory, 0, sizeof(chip8.memory));
    memset(&chip8.registers, 0, sizeof(chip8.registers));
    memset(&chip8.stack, 0, sizeof(chip8.stack));
    memset(&chip8.keyboard, 0, sizeof(chip8.keyboard));

    c8_clear_disp();

    for(int i = 0; i < KEYBOARD_SIZE; i++)
        memset(&(chip8.keyboard[i]), 0, sizeof(chip8.keyboard[i]));

    memcpy(&(chip8.memory[0]), c8_fontset, 80 * sizeof(UBIT8));

    chip8.load_rom = &c8_load_rom;
    chip8.loop = &c8_loop;

    return &chip8;
}