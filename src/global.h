#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL.h>

# define ATARI_RENDER_NORMAL     0
# define ATARI_RENDER_X125       1
# define ATARI_RENDER_FIT        2
# define ATARI_RENDER_MAX        3
# define ATARI_LAST_RENDER       3

# define ATARI_WIDTH  320
# define ATARI_HEIGHT 223

# define ATARI_BLIT_WIDTH  320
# define ATARI_BLIT_HEIGHT 242

# define SNAP_WIDTH   (ATARI_WIDTH*10/25)
# define SNAP_HEIGHT  (ATARI_HEIGHT*10/25)

# define MAX_PATH   256
# define ATARI_MAX_SAVE_STATE 5
# define ATARI_MAX_SAVE_STATE    5
# define ATARI_MAX_CHEAT        10

# define ATARI_RAM_SIZE  65536

#include <psptypes.h>
#include <pspctrl.h>

#define ATARI_CHEAT_NONE    0
#define ATARI_CHEAT_ENABLE  1
#define ATARI_CHEAT_DISABLE 2

#define ATARI_CHEAT_COMMENT_SIZE 25
  
  typedef struct Atari_cheat_t {
    unsigned char  type;
    unsigned short addr;
    unsigned char  value;
    char           comment[ATARI_CHEAT_COMMENT_SIZE];
  } Atari_cheat_t;


  typedef struct Atari_save_t {

    SDL_Surface    *surface;
    char            used;
    char            thumb;
    ScePspDateTime  date;

  } Atari_save_t;

  typedef struct ATARI_t {
 
    Atari_save_t atari_save_state[ATARI_MAX_SAVE_STATE];
    Atari_cheat_t atari_cheat[ATARI_MAX_CHEAT];

    int  comment_present;
    char atari_save_name[MAX_PATH];
    char atari_home_dir[MAX_PATH];
    int  psp_screenshot_id;
    int  psp_cpu_clock;
    int  psp_reverse_analog;
    int  psp_display_lr;
    int  atari_view_fps;
    int  atari_current_fps;
    int  atari_snd_enable;
    int  atari_render_mode;
    int  atari_vsync;
    int  atari_delta_y;
    int  psp_active_joystick;
    int  psp_skip_max_frame;
    int  psp_skip_cur_frame;
    int  atari_speed_limiter;
    int  atari_auto_fire;
    int  atari_auto_fire_pressed;
    int  atari_auto_fire_period;

    unsigned char keyboard_data[19];

  } ATARI_t;

  extern ATARI_t ATARI;


//END_LUDO:
  extern void atari_default_settings(void);
  extern int atari_save_settings(void);
  extern int atari_load_settings();
  extern int atari_load_file_settings(char *FileName);

  extern void atari_update_save_name(char *Name);
  extern void reset_save_name();
  extern void atari_kbd_load(void);
  extern int atari_kbd_save(void);
  extern void emulator_reset(void);
  extern int atari_load_rom(char *FileName, int zip_format);
  extern int atari_load_state(char *FileName);
  extern int atari_snapshot_save_slot(int save_id);
  extern int atari_snapshot_load_slot(int load_id);
  extern int atari_snapshot_del_slot(int save_id);

  extern void myCtrlPeekBufferPositive( SceCtrlData* pc, int count );
  extern int psp_exit_now;

#ifdef __cplusplus
}
#endif

#endif
