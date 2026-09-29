// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2022

#include <stdint.h>
#include <string.h>

#define GBA_SCREEN_W            240
#define GBA_SCREEN_H            160

#define REG_DISPCNT             *((volatile uint16_t *)0x04000000)

#define DISPCNT_BG_MODE_MASK    (0x7)
#define DISPCNT_BG_MODE(n)      ((n) & DISPCNT_BG_MODE_MASK) // 0 to 5

#define DISPCNT_BG2_ENABLE      (1 << 10)

#define MEM_VRAM_MODE3_FB       ((uint16_t *)0x06000000)

#define REG_VCOUNT             *((volatile uint16_t *)0x04000006)

const int32_t size_x = 32;
const int32_t size_y = 32;

int32_t mod_x = 0;
int32_t mod_y = 0;
int32_t mod_x2 = 0;
int32_t mod_y2 = 0;
int32_t mod_x3 = GBA_SCREEN_W;
int32_t mod_y3 = GBA_SCREEN_H;
int32_t mod_x4 = GBA_SCREEN_W;
int32_t mod_y4 = GBA_SCREEN_H;

static inline uint16_t RGB15(uint16_t r, uint16_t g, uint16_t b)
{
    return (r & 0x1F) | ((g & 0x1F) << 5) | ((b & 0x1F) << 10);
}

static inline int is_vdraw(void)
{
	return(REG_VCOUNT >= 160);
}

static inline int is_vblank(void)
{
	return(REG_VCOUNT < 160);
}

static inline void draw(void)
{
	const uint16_t mod_w = GBA_SCREEN_W + 1;
	const uint16_t mod_h = GBA_SCREEN_H + 1;

	static int32_t group = 0;

	group++;
	if (group > 1) group = 0;

	if (group == 0)
	{
		for (int32_t y = 0; y < GBA_SCREEN_H; y++)
		{
		    for (int32_t x = 0; x < GBA_SCREEN_W; x++)
		    {
			MEM_VRAM_MODE3_FB[x + (y * GBA_SCREEN_W)] = RGB15(0,0,0);
		    } 
		}
	}

        for (int32_t y = 0; y < size_y; y+=2)
        {
            for (int32_t x = 0; x < size_x; x+=2)
            {
		uint16_t r = x % 32;
		uint16_t g = y % 32;
		uint16_t b = (x+y) % 32;

		switch(group)
		{
			case 0:
				MEM_VRAM_MODE3_FB[((x+mod_x) % mod_w) + (((y+mod_y) % mod_h) * GBA_SCREEN_W)] = RGB15(r,g,b);
				MEM_VRAM_MODE3_FB[((x+mod_x2) % mod_w) + (((y+mod_y2) % mod_h) * GBA_SCREEN_W)] = RGB15(r,g,b);
				break;
			case 1:
				MEM_VRAM_MODE3_FB[((x+mod_x3) % mod_w) + (((y+mod_y3) % mod_h) * GBA_SCREEN_W)] = RGB15(r,g,b);
				MEM_VRAM_MODE3_FB[((x+mod_x4) % mod_w) + (((y+mod_y4) % mod_h) * GBA_SCREEN_W)] = RGB15(r,g,b);
				break;
			default:
				break;
		}
            }
        }

	/*MEM_VRAM_MODE3_FB[120 + 80 * GBA_SCREEN_W] = RGB15(31, 0, 0);
	MEM_VRAM_MODE3_FB[136 + 80 * GBA_SCREEN_W] = RGB15(0, 31, 0);
	MEM_VRAM_MODE3_FB[120 + 96 * GBA_SCREEN_W] = RGB15(0, 0, 31);*/
}

static inline void loop(void)
{
	while(1)
	{
		mod_x += 1;
		mod_y +=2;
		mod_x2 += 2;
		mod_y2 += 1;
		mod_x3 -= 1;
		mod_y3 -= 2;
		mod_x4 -= 2;
		mod_y4 -= 2;
		if (mod_x > GBA_SCREEN_W) mod_x = 0;
		if (mod_y > GBA_SCREEN_H) mod_y = 0;
		if (mod_x2 > GBA_SCREEN_W) mod_x2 = 0;
		if (mod_y2 > GBA_SCREEN_H) mod_y2 = 0;
		if (mod_x3 < 0) mod_x3 = GBA_SCREEN_W;
		if (mod_y3 < 0) mod_y3 = GBA_SCREEN_H;
		if (mod_x4 < 0) mod_x4 = GBA_SCREEN_W;
		if (mod_y4 < 0) mod_y4 = GBA_SCREEN_H;
		draw();
		while(is_vblank());
		while(is_vdraw());
	}
}

int main(int argc, char *argv[])
{
    REG_DISPCNT = DISPCNT_BG_MODE(3) | DISPCNT_BG2_ENABLE;

    loop();

    while(1);

    return 0;
}
