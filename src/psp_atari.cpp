/*
   Ludovic Jacomme <Ludovic.Jacomme@gmail.com>
*/
#include <stdlib.h>
#include <stdio.h>

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspthreadman.h>

#include <zlib.h>
#include <string>
#include <psppower.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <SDL.h>

#include "global.h"
#include "psp_fmgr.h"
#include "psp_kbd.h"
#include "psp_danzeff.h"
#include "psp_sdl.h"

#include "Cartridge.h"
#include "ProSystem.h"
#include "Database.h"
#include "Sound.h"

extern "C" {

word atari_pal16[256] = {0};

void
psp_atari_reset()
{
  prosystem_Reset();
}

int
psp_atari_load_state(char *filename)
{
  int error;

  std::string std_filename(filename);
  error = prosystem_Load(filename) ? 0 : 1;

  return error;
}

int
psp_atari_save_state(char *filename)
{
  int error;

  std::string std_filename(filename);
  error = prosystem_Save(std_filename, true) ? 0 : 1;

  return error;
}

int 
psp_atari_load_rom(char *filename) 
{
  std::string std_filename(filename);
  if(! cartridge_Load(std_filename)) return 1;

# if 0
    sound_Stop( );
    display_Clear( );
# endif
  database_Load(cartridge_digest);

  prosystem_Reset();

# if 0
    std::string title = std::string(CONSOLE_TITLE) + " - " + common_Trim(cartridge_title);
    SetWindowText(console_hWnd, title.c_str( ));
    console_AddRecent(filename);
    display_ResetPalette( );
    console_SetZoom(display_zoom);
    sound_Play( );
# endif
  return 0;
}

int 
psp_atari_load_rom_buffer(char *rom_buffer, int rom_size) 
{
  if(! cartridge_Load_buffer(rom_buffer, rom_size)) return 1;

# if 0
    sound_Stop( );
    display_Clear( );
# endif
  database_Load(cartridge_digest);

  prosystem_Reset();

# if 0
    std::string title = std::string(CONSOLE_TITLE) + " - " + common_Trim(cartridge_title);
    SetWindowText(console_hWnd, title.c_str( ));
    console_AddRecent(filename);
    display_ResetPalette( );
    console_SetZoom(display_zoom);
    sound_Play( );
# endif
  return 0;
}

/* LUDO: */
void
atari_synchronize(void)
{
  static u32 nextclock = 1;

  if (nextclock) {
    u32 curclock;
    do {
     curclock = SDL_GetTicks();
    } while (curclock < nextclock);
    nextclock = curclock + (u32)( 1000 / ATARI.atari_speed_limiter);
  }
}

void
atari_update_fps()
{
  static u32 next_sec_clock = 0;
  static u32 cur_num_frame = 0;
  cur_num_frame++;
  u32 curclock = SDL_GetTicks();
  if (curclock > next_sec_clock) {
    next_sec_clock = curclock + 1000;
    ATARI.atari_current_fps = cur_num_frame;
    cur_num_frame = 0;
  }
}

void
psp_atari_init_palette16()
{
  uint rsize  =  6;
  uint gsize  =  5;
  uint bsize  =  6;
      
  for(uint index = 0; index < 256; index++) {
    word r = palette_data[(index * 3) + 0];
    word g = palette_data[(index * 3) + 1];
    word b = palette_data[(index * 3) + 2];
    atari_pal16[index] = psp_sdl_rgb(r, g, b);
  }
}

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

/* 

  LUDO: 16-bit HiColor (565 format) 
  see http://www.compuphase.com/graphic/scale3.htm

 */
static inline word loc_coloraverage(word a, word b)
{
  return (word)(((a ^ b) & 0xf7deU) >> 1) + (a & b);
}

# define ATARI_BLIT_TOP_Y 16

static inline void
loc_split_src_rect( SDL_Rect* r )
{
  if (r->x < 0) r->x = 0;
  if (r->y < 0) r->y = 0;
  if (r->w > ATARI_BLIT_WIDTH) r->w = ATARI_BLIT_WIDTH;
  if (r->h > ATARI_BLIT_HEIGHT) r->h = ATARI_BLIT_HEIGHT;
}

static inline void
loc_split_dst_rect( SDL_Rect* r )
{
  if (r->x < 0) r->x = 0;
  if (r->y < 0) r->y = 0;
  if (r->w > 480) r->w = 480;
  if (r->h > 272) r->h = 272;
}

static void
psp_atari_put_image_gu_normal()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

  srcRect.w = ATARI_WIDTH;
  srcRect.h = ATARI_HEIGHT;
  srcRect.x = 0;
  srcRect.y = ATARI_BLIT_TOP_Y;
  loc_split_src_rect( &srcRect );

  dstRect.w = ATARI_WIDTH;
  dstRect.h = ATARI_HEIGHT;
  dstRect.x = (480 - dstRect.w) / 2;
  dstRect.y = (272 - dstRect.h) / 2;
  loc_split_dst_rect( &dstRect );

  psp_sdl_gu_stretch(&srcRect, &dstRect);
}

static void
psp_atari_put_image_gu_x125()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

  srcRect.w = ATARI_WIDTH;
  srcRect.h = ATARI_HEIGHT;
  srcRect.x = 0;
  srcRect.y = ATARI_BLIT_TOP_Y;

  dstRect.w = 400;
  dstRect.h = 272;
  dstRect.x = (480 - dstRect.w) / 2;
  dstRect.y = 0;
  loc_split_dst_rect( &dstRect );

  psp_sdl_gu_stretch(&srcRect, &dstRect);
}

static void
psp_atari_put_image_gu_x15()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

  srcRect.w = ATARI_WIDTH;
  srcRect.h = 200;
  srcRect.x = 0;
  srcRect.y = ATARI_BLIT_TOP_Y + 10 + ATARI.atari_delta_y;
  loc_split_src_rect( &srcRect );

  dstRect.w = 480;
  dstRect.h = 272;
  dstRect.x = 0;
  dstRect.y = 0;

  psp_sdl_gu_stretch(&srcRect, &dstRect);
}

static void
psp_atari_put_image_gu_max()
{
  SDL_Rect srcRect;
  SDL_Rect dstRect;

  srcRect.w = ATARI_WIDTH;
  srcRect.h = 180;
  srcRect.x = 0;
  srcRect.y = ATARI_BLIT_TOP_Y + 20 + ATARI.atari_delta_y;
  loc_split_src_rect( &srcRect );

  dstRect.w = 480;
  dstRect.h = 272;
  dstRect.x = 0;
  dstRect.y = 0;

  psp_sdl_gu_stretch(&srcRect, &dstRect);
}

void
psp_atari_wait_vsync()
{
# ifndef LINUX_MODE
  static int loc_pv = 0;
  int cv = sceDisplayGetVcount();
  if (loc_pv == cv) {
    sceDisplayWaitVblankCB();
  }
  loc_pv = sceDisplayGetVcount();
# endif
}

void
psp_atari_refresh_screen()
{
  if (ATARI.psp_skip_cur_frame <= 0) {

    ATARI.psp_skip_cur_frame = ATARI.psp_skip_max_frame;

    if (ATARI.atari_render_mode == ATARI_RENDER_NORMAL) psp_atari_put_image_gu_normal(); 
    else
    if (ATARI.atari_render_mode == ATARI_RENDER_X125  ) psp_atari_put_image_gu_x125(); 
    else
    if (ATARI.atari_render_mode == ATARI_RENDER_FIT   ) psp_atari_put_image_gu_x15(); 
    else
    if (ATARI.atari_render_mode == ATARI_RENDER_MAX   ) psp_atari_put_image_gu_max(); 

    if (psp_kbd_is_danzeff_mode()) {
      danzeff_moveTo(-165, -50);
      danzeff_render();
    }

    if (ATARI.atari_view_fps) {
      char buffer[32];
      sprintf(buffer, "%3d", (int)ATARI.atari_current_fps);
      psp_sdl_fill_print(0, 0, buffer, 0xffffff, 0 );
    }

    if (ATARI.psp_display_lr) {
      psp_kbd_display_active_mapping();
    }

    if (ATARI.atari_vsync) {
      psp_atari_wait_vsync();
    }
    psp_sdl_flip();

    if (psp_screenshot_mode) {
      psp_screenshot_mode--;
      if (psp_screenshot_mode <= 0) {
        psp_sdl_save_screenshot();
        psp_screenshot_mode = 0;
      }
    }
  } else if (ATARI.psp_skip_max_frame) {
    ATARI.psp_skip_cur_frame--;
  }

  if (ATARI.atari_speed_limiter) {
    atari_synchronize();
  }

  if (ATARI.atari_view_fps) {
    atari_update_fps();
  }
}

void
psp_atari_main_loop()
{
  if (bios_Load("./7800.rom")) {
    bios_enabled = true;
  }
  /* Open default rom */
  if (psp_atari_load_rom("./default.a78")) {
    psp_sdl_exit(1);
  }

  psp_atari_init_palette16();
  psp_sdl_black_screen();

  while (1) {
    psp_update_keys();

    if(prosystem_active && !prosystem_paused) {
      prosystem_ExecuteFrame(ATARI.keyboard_data);
      sound_Store();
      psp_atari_refresh_screen();
    }
  }
}

};
