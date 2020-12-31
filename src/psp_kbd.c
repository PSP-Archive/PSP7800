/*
 *  Copyright (C) 2006 Ludovic Jacomme (ludovic.jacomme@gmail.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

#include <pspctrl.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <SDL.h>

#include "global.h"
#include "psp_kbd.h"
#include "psp_menu.h"
#include "psp_sdl.h"
#include "psp_danzeff.h"
#include "psp_irkeyb.h"
#include "psp_battery.h"

# define KBD_MIN_ANALOG_TIME      150000
# define KBD_MIN_START_TIME       800000
# define KBD_MAX_EVENT_TIME       500000
# define KBD_MIN_PENDING_TIME     300000
# define KBD_MIN_HOTKEY_TIME      600000
# define KBD_MIN_IR_PENDING_TIME  100000
//# define KBD_MIN_DANZEFF_TIME   150000
# define KBD_MIN_DANZEFF_TIME      10000
# define KBD_MIN_COMMAND_TIME     100000
# define KBD_MIN_BATTCHECK_TIME 90000000 
# define KBD_MIN_AUTOFIRE_TIME   1000000

 static SceCtrlData    loc_button_data;
 static unsigned int   loc_last_event_time = 0;
 static unsigned int   loc_last_hotkey_time = 0;
#ifdef USE_PSP_IRKEYB
 static unsigned int   loc_last_irkbd_event_time = 0;
#endif
 static unsigned int   loc_last_analog_time = 0;
 static long           first_time_stamp = -1;
 static long           first_time_batt_stamp = -1;
 static long           first_time_auto_stamp = -1;
 static char           loc_button_press[ KBD_MAX_BUTTONS ]; 
 static char           loc_button_release[ KBD_MAX_BUTTONS ]; 
 static unsigned int   loc_button_mask[ KBD_MAX_BUTTONS ] =
 {
   PSP_CTRL_UP         , /*  KBD_UP         */
   PSP_CTRL_RIGHT      , /*  KBD_RIGHT      */
   PSP_CTRL_DOWN       , /*  KBD_DOWN       */
   PSP_CTRL_LEFT       , /*  KBD_LEFT       */
   PSP_CTRL_TRIANGLE   , /*  KBD_TRIANGLE   */
   PSP_CTRL_CIRCLE     , /*  KBD_CIRCLE     */
   PSP_CTRL_CROSS      , /*  KBD_CROSS      */
   PSP_CTRL_SQUARE     , /*  KBD_SQUARE     */
   PSP_CTRL_SELECT     , /*  KBD_SELECT     */
   PSP_CTRL_START      , /*  KBD_START      */
   PSP_CTRL_HOME       , /*  KBD_HOME       */
   PSP_CTRL_HOLD       , /*  KBD_HOLD       */
   PSP_CTRL_LTRIGGER   , /*  KBD_LTRIGGER   */
   PSP_CTRL_RTRIGGER   , /*  KBD_RTRIGGER   */
 };

 static char loc_button_name[ KBD_ALL_BUTTONS ][20] =
 {
   "UP",
   "RIGHT",
   "DOWN",
   "LEFT",
   "TRIANGLE",
   "CIRCLE",
   "CROSS",
   "SQUARE",
   "SELECT",
   "START",
   "HOME",
   "HOLD",
   "LTRIGGER",
   "RTRIGGER",
   "JOY_UP",
   "JOY_RIGHT",
   "JOY_DOWN",
   "JOY_LEFT"
 };

 static char loc_button_name_L[ KBD_ALL_BUTTONS ][20] =
 {
   "L_UP",
   "L_RIGHT",
   "L_DOWN",
   "L_LEFT",
   "L_TRIANGLE",
   "L_CIRCLE",
   "L_CROSS",
   "L_SQUARE",
   "L_SELECT",
   "L_START",
   "L_HOME",
   "L_HOLD",
   "L_LTRIGGER",
   "L_RTRIGGER",
   "L_JOY_UP",
   "L_JOY_RIGHT",
   "L_JOY_DOWN",
   "L_JOY_LEFT"
 };
 
  static char loc_button_name_R[ KBD_ALL_BUTTONS ][20] =
 {
   "R_UP",
   "R_RIGHT",
   "R_DOWN",
   "R_LEFT",
   "R_TRIANGLE",
   "R_CIRCLE",
   "R_CROSS",
   "R_SQUARE",
   "R_SELECT",
   "R_START",
   "R_HOME",
   "R_HOLD",
   "R_LTRIGGER",
   "R_RTRIGGER",
   "R_JOY_UP",
   "R_JOY_RIGHT",
   "R_JOY_DOWN",
   "R_JOY_LEFT"
 };
 
  struct atari_key_trans psp_atari_key_info[ATARI_MAX_KEY]=
  {
    { ATARI_JOY1_RIGHT, "J1 RIGHT" },
    { ATARI_JOY1_LEFT , "J1 LEFT" },
    { ATARI_JOY1_DOWN , "J1 DOWN" },
    { ATARI_JOY1_UP   , "J1 UP" },
    { ATARI_JOY1_FIRE1, "J1 FIRE1" },
    { ATARI_JOY1_FIRE2, "J1 FIRE2" },

    { ATARI_JOY2_RIGHT, "J2 RIGHT" },
    { ATARI_JOY2_LEFT,  "J2 LEFT" },
    { ATARI_JOY2_DOWN,  "J2 DOWN" },
    { ATARI_JOY2_UP,    "J2 UP" },
    { ATARI_JOY2_FIRE1, "J2 FIRE1" },
    { ATARI_JOY2_FIRE2, "J2 FIRE2" },

    { ATARI_RESET,     "RESET" },
    { ATARI_SELECT,    "SELECT" },
    { ATARI_PAUSE,     "PAUSE" },
    { ATARI_LDIFF,     "LDIFF" },
    { ATARI_RDIFF,     "RDIFF" },

    { ATARIC_FPS,      "C_FPS" },
    { ATARIC_JOY,      "C_JOY" },
    { ATARIC_RENDER,   "C_RENDER" },
    { ATARIC_LOAD,     "C_LOAD" },
    { ATARIC_SAVE,     "C_SAVE" },
    { ATARIC_RESET,    "C_RESET" },
    { ATARIC_AUTOFIRE, "C_AUTOFIRE" },
    { ATARIC_INCFIRE,  "C_INCFIRE" },
    { ATARIC_DECFIRE,  "C_DECFIRE" },
    { ATARIC_INCDELTA, "C_INCDELTA" },
    { ATARIC_DECDELTA, "C_DECDELTA" },
    { ATARIC_SCREEN,   "C_SCREEN" }
  };

 static int loc_default_mapping[ KBD_ALL_BUTTONS ] = {
    ATARI_JOY1_UP          , /*  KBD_UP         */
    ATARI_JOY1_RIGHT       , /*  KBD_RIGHT      */
    ATARI_JOY1_DOWN        , /*  KBD_DOWN       */
    ATARI_JOY1_LEFT        , /*  KBD_LEFT       */
    ATARI_LDIFF            , /*  KBD_TRIANGLE   */
    ATARI_RDIFF            , /*  KBD_CIRCLE     */
    ATARI_JOY1_FIRE1       , /*  KBD_CROSS      */
    ATARI_JOY1_FIRE2       , /*  KBD_SQUARE     */
    -1                     , /*  KBD_SELECT     */
    -1                     , /*  KBD_START      */
    -1                     , /*  KBD_HOME       */
    -1                     , /*  KBD_HOLD       */
   KBD_LTRIGGER_MAPPING    , /*  KBD_LTRIGGER   */
   KBD_RTRIGGER_MAPPING    , /*  KBD_RTRIGGER   */
    ATARI_JOY1_UP          , /*  KBD_JOY_UP     */
    ATARI_JOY1_RIGHT       , /*  KBD_JOY_RIGHT  */
    ATARI_JOY1_DOWN        , /*  KBD_JOY_DOWN   */
    ATARI_JOY1_LEFT          /*  KBD_JOY_LEFT   */
  };

 static int loc_default_mapping_L[ KBD_ALL_BUTTONS ] = {
    ATARIC_INCDELTA        , /*  KBD_UP         */
    ATARIC_RENDER          , /*  KBD_RIGHT      */
    ATARIC_DECDELTA        , /*  KBD_DOWN       */
    ATARIC_RENDER          , /*  KBD_LEFT       */
    ATARIC_LOAD            , /*  KBD_TRIANGLE   */
    ATARIC_JOY             , /*  KBD_CIRCLE     */
    ATARIC_SAVE            , /*  KBD_CROSS      */
    ATARIC_FPS             , /*  KBD_SQUARE     */
    -1                     , /*  KBD_SELECT     */
    -1                     , /*  KBD_START      */
    -1                     , /*  KBD_HOME       */
    -1                     , /*  KBD_HOLD       */
   KBD_LTRIGGER_MAPPING    , /*  KBD_LTRIGGER   */
   KBD_RTRIGGER_MAPPING    , /*  KBD_RTRIGGER   */
    ATARI_JOY1_UP          , /*  KBD_JOY_UP     */
    ATARI_JOY1_RIGHT       , /*  KBD_JOY_RIGHT  */
    ATARI_JOY1_DOWN        , /*  KBD_JOY_DOWN   */
    ATARI_JOY1_LEFT          /*  KBD_JOY_LEFT   */
  };

 static int loc_default_mapping_R[ KBD_ALL_BUTTONS ] = {
    ATARI_JOY1_UP          , /*  KBD_UP         */
    ATARIC_INCFIRE         , /*  KBD_RIGHT      */
    ATARI_JOY1_DOWN        , /*  KBD_DOWN       */
    ATARIC_DECFIRE         , /*  KBD_LEFT       */
    ATARI_RESET            , /*  KBD_TRIANGLE   */
    ATARI_SELECT           , /*  KBD_CIRCLE     */
    ATARIC_AUTOFIRE        , /*  KBD_CROSS      */
    ATARI_PAUSE            , /*  KBD_SQUARE     */
    -1                     , /*  KBD_SELECT     */
    -1                     , /*  KBD_START      */
    -1                     , /*  KBD_HOME       */
    -1                     , /*  KBD_HOLD       */
   KBD_LTRIGGER_MAPPING    , /*  KBD_LTRIGGER   */
   KBD_RTRIGGER_MAPPING    , /*  KBD_RTRIGGER   */
    ATARI_JOY1_UP          , /*  KBD_JOY_UP     */
    ATARI_JOY1_RIGHT       , /*  KBD_JOY_RIGHT  */
    ATARI_JOY1_DOWN        , /*  KBD_JOY_DOWN   */
    ATARI_JOY1_LEFT          /*  KBD_JOY_LEFT   */
  };

 int psp_kbd_mapping[ KBD_ALL_BUTTONS ];
 int psp_kbd_mapping_L[ KBD_ALL_BUTTONS ];
 int psp_kbd_mapping_R[ KBD_ALL_BUTTONS ];
 int psp_kbd_presses[ KBD_ALL_BUTTONS ];
 int kbd_ltrigger_mapping_active;
 int kbd_rtrigger_mapping_active;

# define KBD_MAX_ENTRIES   11


  int kbd_layout[KBD_MAX_ENTRIES][2] = {
    /* Key            Ascii */
    { ATARI_JOY1_FIRE1,    -1 },
    { ATARI_JOY1_FIRE2,    -1 },
    { ATARI_JOY1_UP,       -1 },
    { ATARI_JOY1_DOWN,     -1 },
    { ATARI_JOY1_LEFT,     -1 },
    { ATARI_JOY1_RIGHT,    -1 },
    { ATARI_RESET,        DANZEFF_RESET },
    { ATARI_SELECT,       DANZEFF_SEL    },
    { ATARI_PAUSE,        DANZEFF_PAUSE },
    { ATARI_LDIFF,        DANZEFF_LDIFF },
    { ATARI_RDIFF,        DANZEFF_RDIFF }
  };

        int psp_kbd_mapping[ KBD_ALL_BUTTONS ];

#ifdef USE_PSP_IRKEYB
# define IRKEYB_ATARI_MAX_PENDING 20
 static int irkeyb_atari_pending_max  = 0;
 static int irkeyb_atari_pending_curr = 0;
 static int irkeyb_atari_pending_keys[IRKEYB_ATARI_MAX_PENDING];
# endif

 static int danzeff_atari_key     = 0;
 static int danzeff_atari_pending = 0;
 static int danzeff_mode        = 0;

int
atari_key_event(int atari_idx, int button_press)
{
  if (ATARI.psp_active_joystick) {
    if ((atari_idx >= ATARI_JOY1_RIGHT) && 
        (atari_idx <= ATARI_JOY1_FIRE2)) {
      /* Convert Joystick Player 1 -> Player 2 ? */
      atari_idx = atari_idx - ATARI_JOY1_RIGHT + ATARI_JOY2_RIGHT;
    }
  }

  if ((atari_idx >=             0) && 
      (atari_idx <= ATARI_RDIFF)) {

    if (button_press) {
      ATARI.keyboard_data[atari_idx] = 1;
    } else {
      ATARI.keyboard_data[atari_idx] = 0;
    }
  } else
  if ((atari_idx >= ATARIC_FPS) &&
      (atari_idx <= ATARIC_SCREEN)) {

    if (button_press) {
      SceCtrlData c;
      myCtrlPeekBufferPositive(&c, 1);
      if ((c.TimeStamp - loc_last_hotkey_time) > KBD_MIN_HOTKEY_TIME) 
      {
        loc_last_hotkey_time = c.TimeStamp;
        atari_treat_command_key(atari_idx);
      }
    }
  }
  return 0;
}
int 
atari_kbd_reset()
{
  memset(ATARI.keyboard_data, 0, sizeof(ATARI.keyboard_data));
  return 0;
}

int
atari_get_key_from_ascii(int key_ascii)
{
  int index;
  for (index = 0; index < KBD_MAX_ENTRIES; index++) {
   if (kbd_layout[index][1] == key_ascii) return kbd_layout[index][0];
  }
  return -1;
}

int
psp_kbd_reset_mapping(void)
{
  memcpy(psp_kbd_mapping, loc_default_mapping, sizeof(loc_default_mapping));
  memcpy(psp_kbd_mapping_L, loc_default_mapping_L, sizeof(loc_default_mapping_L));
  memcpy(psp_kbd_mapping_R, loc_default_mapping_R, sizeof(loc_default_mapping_R));
  return 0;
}

int
psp_kbd_reset_hotkeys(void)
{
  int index;
  int key_id;
  for (index = 0; index < KBD_ALL_BUTTONS; index++) {
    key_id = loc_default_mapping[index];
    if ((key_id >= ATARIC_FPS) && (key_id <= ATARIC_SCREEN)) {
      psp_kbd_mapping[index] = key_id;
    }
    key_id = loc_default_mapping_L[index];
    if ((key_id >= ATARIC_FPS) && (key_id <= ATARIC_SCREEN)) {
      psp_kbd_mapping_L[index] = key_id;
    }
    key_id = loc_default_mapping_R[index];
    if ((key_id >= ATARIC_FPS) && (key_id <= ATARIC_SCREEN)) {
      psp_kbd_mapping_R[index] = key_id;
    }
  }
  return 0;
}

int
psp_kbd_load_mapping_file(FILE *KbdFile)
{
  char     Buffer[512];
  char    *Scan;
  int      tmp_mapping[KBD_ALL_BUTTONS];
  int      tmp_mapping_L[KBD_ALL_BUTTONS];
  int      tmp_mapping_R[KBD_ALL_BUTTONS];
  int      atari_key_id = 0;
  int      kbd_id = 0;

  memcpy(tmp_mapping, loc_default_mapping, sizeof(loc_default_mapping));
  memcpy(tmp_mapping_L, loc_default_mapping_L, sizeof(loc_default_mapping_R));
  memcpy(tmp_mapping_R, loc_default_mapping_R, sizeof(loc_default_mapping_R));

  while (fgets(Buffer,512,KbdFile) != (char *)0) {
      
      Scan = strchr(Buffer,'\n');
      if (Scan) *Scan = '\0';
      /* For this #@$% of windows ! */
      Scan = strchr(Buffer,'\r');
      if (Scan) *Scan = '\0';
      if (Buffer[0] == '#') continue;

      Scan = strchr(Buffer,'=');
      if (! Scan) continue;
    
      *Scan = '\0';
      atari_key_id = atoi(Scan + 1);

      for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++) {
        if (!strcasecmp(Buffer,loc_button_name[kbd_id])) {
          tmp_mapping[kbd_id] = atari_key_id;
          //break;
        }
      }
      for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++) {
        if (!strcasecmp(Buffer,loc_button_name_L[kbd_id])) {
          tmp_mapping_L[kbd_id] = atari_key_id;
          //break;
        }
      }
      for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++) {
        if (!strcasecmp(Buffer,loc_button_name_R[kbd_id])) {
          tmp_mapping_R[kbd_id] = atari_key_id;
          //break;
        }
      }
  }

  memcpy(psp_kbd_mapping, tmp_mapping, sizeof(psp_kbd_mapping));
  memcpy(psp_kbd_mapping_L, tmp_mapping_L, sizeof(psp_kbd_mapping_L));
  memcpy(psp_kbd_mapping_R, tmp_mapping_R, sizeof(psp_kbd_mapping_R));
  
  return 0;
}

int
psp_kbd_load_mapping(char *kbd_filename)
{
  FILE    *KbdFile;
  int      error = 0;

  KbdFile = fopen(kbd_filename, "r");
  error   = 1;

  if (KbdFile != (FILE*)0) {
  psp_kbd_load_mapping_file(KbdFile);
  error = 0;
    fclose(KbdFile);
  }

  kbd_ltrigger_mapping_active = 0;
  kbd_rtrigger_mapping_active = 0;
    
  return error;
}

int
psp_kbd_save_mapping(char *kbd_filename)
{
  FILE    *KbdFile;
  int      kbd_id = 0;
  int      error = 0;

  KbdFile = fopen(kbd_filename, "w");
  error   = 1;

  if (KbdFile != (FILE*)0) {

    for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++)
    {
      fprintf(KbdFile, "%s=%d\n", loc_button_name[kbd_id], psp_kbd_mapping[kbd_id]);
    }
    for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++)
    {
      fprintf(KbdFile, "%s=%d\n", loc_button_name_L[kbd_id], psp_kbd_mapping_L[kbd_id]);
    }
    for (kbd_id = 0; kbd_id < KBD_ALL_BUTTONS; kbd_id++)
    {
      fprintf(KbdFile, "%s=%d\n", loc_button_name_R[kbd_id], psp_kbd_mapping_R[kbd_id]);
    }
    error = 0;
    fclose(KbdFile);
  }

  return error;
}

int 
psp_kbd_is_danzeff_mode()
{
  return danzeff_mode;
}

int
psp_kbd_enter_danzeff()
{
  unsigned int danzeff_key = 0;
  int          atari_key   = 0;
  int          key_code    = ATARI_SELECT;
  SceCtrlData  c;

  if (! danzeff_mode) {
    psp_init_keyboard();
    danzeff_mode = 1;
  }

  myCtrlPeekBufferPositive(&c, 1);
  c.Buttons &= PSP_ALL_BUTTON_MASK;

  if (danzeff_atari_pending) 
  {
    atari_key_event(danzeff_atari_key, 1);

    if ((c.TimeStamp - loc_last_event_time) > KBD_MIN_PENDING_TIME) {
      loc_last_event_time = c.TimeStamp;
      danzeff_atari_pending = 0;
      atari_key_event(danzeff_atari_key, 0);
    }
    return 0;
  }

  if ((c.TimeStamp - loc_last_event_time) > KBD_MIN_DANZEFF_TIME) {
    loc_last_event_time = c.TimeStamp;
  
    myCtrlPeekBufferPositive(&c, 1);
    c.Buttons &= PSP_ALL_BUTTON_MASK;
# ifdef USE_PSP_IRKEYB
    psp_irkeyb_set_psp_key(&c);
# endif
    danzeff_key = danzeff_readInput(c);
  }

  if (danzeff_key > DANZEFF_START) {
    atari_key = atari_get_key_from_ascii(danzeff_key);

    if (atari_key != -1) {
      danzeff_atari_key     = atari_key;
      danzeff_atari_pending = 1;
      atari_key_event(danzeff_atari_key, 1);
    }

    return 1;

  } else if (danzeff_key == DANZEFF_START) {
    danzeff_mode          = 0;
    danzeff_atari_pending = 0;
    danzeff_atari_key     = 0;

    psp_kbd_wait_no_button();

  } else if (danzeff_key == DANZEFF_SELECT) {
    danzeff_mode          = 0;
    danzeff_atari_pending = 0;
    danzeff_atari_key     = 0;
    psp_main_menu();
    psp_init_keyboard();

    psp_kbd_wait_no_button();
  }

  return 0;
}

#ifdef USE_PSP_IRKEYB
int
psp_kbd_enter_irkeyb()
{
  int atari_key   = 0;
  int psp_irkeyb = PSP_IRKEYB_EMPTY;

  SceCtrlData  c;
  myCtrlPeekBufferPositive(&c, 1);

  if (irkeyb_atari_pending_max) 
  {
    if ((c.TimeStamp - loc_last_irkbd_event_time) > KBD_MIN_IR_PENDING_TIME) {
      loc_last_irkbd_event_time = c.TimeStamp;
      atari_key_event(irkeyb_atari_pending_keys[irkeyb_atari_pending_curr], 0);
      irkeyb_atari_pending_curr++;
      if (irkeyb_atari_pending_curr >= irkeyb_atari_pending_max) {
        irkeyb_atari_pending_curr = 0;
        irkeyb_atari_pending_max  = 0;
      }
    }

    return 0;
  }

  loc_last_irkbd_event_time = c.TimeStamp;
  
  psp_irkeyb = psp_irkeyb_read_key();
  if (psp_irkeyb != PSP_IRKEYB_EMPTY) {

    if (psp_irkeyb == PSP_IRKEYB_UP) {
      atari_key = ATARI_JOY1_UP;
    } else
    if (psp_irkeyb == PSP_IRKEYB_DOWN) {
      atari_key = ATARI_JOY1_DOWN;
    } else
    if (psp_irkeyb == PSP_IRKEYB_LEFT) {
      atari_key = ATARI_JOY1_LEFT;
    } else
    if (psp_irkeyb == PSP_IRKEYB_RIGHT) {
      atari_key = ATARI_JOY1_RIGHT;
    } else {
      atari_key = atari_get_key_from_ascii(psp_irkeyb);
    }
    if (atari_key != -1) {
      if ((irkeyb_atari_pending_max+1) < IRKEYB_ATARI_MAX_PENDING) {
        irkeyb_atari_pending_keys[irkeyb_atari_pending_max++] = atari_key;
        atari_key_event(atari_key, 1);
      }
      return 1;
    }
  }

  return 0;
}
# endif

void
psp_kbd_display_active_mapping()
{
  if (kbd_ltrigger_mapping_active) {
    psp_sdl_fill_rectangle(0, 0, 10, 3, psp_sdl_rgb(0x0, 0x0, 0xff), 0);
  } else {
    psp_sdl_fill_rectangle(0, 0, 10, 3, 0x0, 0);
  }

  if (kbd_rtrigger_mapping_active) {
    psp_sdl_fill_rectangle(470, 0, 10, 3, psp_sdl_rgb(0x0, 0x0, 0xff), 0);
  } else {
    psp_sdl_fill_rectangle(470, 0, 10, 3, 0x0, 0);
  }
}

int
atari_decode_key(int psp_b, int button_pressed)
{
  int wake = 0;
  int reverse_analog = ATARI.psp_reverse_analog;

  if (reverse_analog) {
    if ((psp_b >= KBD_JOY_UP  ) &&
        (psp_b <= KBD_JOY_LEFT)) {
      psp_b = psp_b - KBD_JOY_UP + KBD_UP;
    } else
    if ((psp_b >= KBD_UP  ) &&
        (psp_b <= KBD_LEFT)) {
      psp_b = psp_b - KBD_UP + KBD_JOY_UP;
    }
  }

  if (psp_b == KBD_START) {
     if (button_pressed) psp_kbd_enter_danzeff();
  } else
  if (psp_b == KBD_SELECT) {
    if (button_pressed) {
      psp_main_menu();
      psp_init_keyboard();
    }
  } else {
 
    if (psp_kbd_mapping[psp_b] >= 0) {
      wake = 1;
      if (button_pressed) {
        // Determine which buton to press first (ie which mapping is currently active)
        if (kbd_ltrigger_mapping_active) {
          // Use ltrigger mapping
          psp_kbd_presses[psp_b] = psp_kbd_mapping_L[psp_b];
          atari_key_event(psp_kbd_presses[psp_b], button_pressed);
        } else
        if (kbd_rtrigger_mapping_active) {
          // Use rtrigger mapping
          psp_kbd_presses[psp_b] = psp_kbd_mapping_R[psp_b];
          atari_key_event(psp_kbd_presses[psp_b], button_pressed);
        } else {
          // Use standard mapping
          psp_kbd_presses[psp_b] = psp_kbd_mapping[psp_b];
          atari_key_event(psp_kbd_presses[psp_b], button_pressed);
        }
      } else {
          // Determine which button to release (ie what was pressed before)
          atari_key_event(psp_kbd_presses[psp_b], button_pressed);
      }

    } else {
      if (psp_kbd_mapping[psp_b] == KBD_LTRIGGER_MAPPING) {
        kbd_ltrigger_mapping_active = button_pressed;
        kbd_rtrigger_mapping_active = 0;
      } else
      if (psp_kbd_mapping[psp_b] == KBD_RTRIGGER_MAPPING) {
        kbd_rtrigger_mapping_active = button_pressed;
        kbd_ltrigger_mapping_active = 0;
      }
    }
  }
  return 0;
}

# define ANALOG_THRESHOLD 60

void 
kbd_get_analog_direction(int Analog_x, int Analog_y, int *x, int *y)
{
  int DeltaX = 255;
  int DeltaY = 255;
  int DirX   = 0;
  int DirY   = 0;

  *x = 0;
  *y = 0;

  if (Analog_x <=        ANALOG_THRESHOLD)  { DeltaX = Analog_x; DirX = -1; }
  else 
  if (Analog_x >= (255 - ANALOG_THRESHOLD)) { DeltaX = 255 - Analog_x; DirX = 1; }

  if (Analog_y <=        ANALOG_THRESHOLD)  { DeltaY = Analog_y; DirY = -1; }
  else 
  if (Analog_y >= (255 - ANALOG_THRESHOLD)) { DeltaY = 255 - Analog_y; DirY = 1; }

  *x = DirX;
  *y = DirY;
}

void
kbd_change_auto_fire(int auto_fire)
{
  ATARI.atari_auto_fire = auto_fire;
  if (ATARI.atari_auto_fire_pressed) {
    atari_key_event(psp_kbd_mapping[KBD_CROSS], 0);
    ATARI.atari_auto_fire_pressed = 0;
  }
}

static int 
kbd_reset_button_status(void)
{
  int b = 0;
  /* Reset Button status */
  for (b = 0; b < KBD_MAX_BUTTONS; b++) {
    loc_button_press[b]   = 0;
    loc_button_release[b] = 0;
  }
  psp_init_keyboard();
  return 0;
}

int
kbd_scan_keyboard(void)
{
  SceCtrlData c;
  long        delta_stamp;
  int         event;
  int         b;

  int new_Lx;
  int new_Ly;
  int old_Lx;
  int old_Ly;

  event = 0;
  myCtrlPeekBufferPositive( &c, 1 );
  c.Buttons &= PSP_ALL_BUTTON_MASK;

# ifdef USE_PSP_IRKEYB
  psp_irkeyb_set_psp_key(&c);
# endif

  if ((c.Buttons & (PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|PSP_CTRL_START)) ==
      (PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|PSP_CTRL_START)) {
    /* Exit ! */
    psp_sdl_exit(0);
  }

  delta_stamp = c.TimeStamp - first_time_stamp;
  if ((delta_stamp < 0) || (delta_stamp > KBD_MIN_BATTCHECK_TIME)) {
    first_time_stamp = c.TimeStamp;
    if (psp_is_low_battery()) {
      psp_main_menu();
      psp_init_keyboard();
      return 0;
    }
  }

  if (ATARI.atari_auto_fire) {
    delta_stamp = c.TimeStamp - first_time_auto_stamp;
    if ((delta_stamp < 0) || 
        (delta_stamp > (KBD_MIN_AUTOFIRE_TIME / (1 + ATARI.atari_auto_fire_period)))) {
      first_time_auto_stamp = c.TimeStamp;
      atari_key_event(psp_kbd_mapping[KBD_CROSS], ATARI.atari_auto_fire_pressed);
      ATARI.atari_auto_fire_pressed = ! ATARI.atari_auto_fire_pressed;
    }
  }

  /* Check Analog Device */
  kbd_get_analog_direction(loc_button_data.Lx,loc_button_data.Ly,&old_Lx,&old_Ly);
  kbd_get_analog_direction( c.Lx, c.Ly, &new_Lx, &new_Ly);

  /* Analog device has moved */
  if (new_Lx > 0) {
    if (old_Lx <  0) atari_decode_key(KBD_JOY_LEFT , 0);
    if (old_Lx <= 0) atari_decode_key(KBD_JOY_RIGHT, 1);
  } else 
  if (new_Lx < 0) {
    if (old_Lx >  0) atari_decode_key(KBD_JOY_RIGHT, 0);
    if (old_Lx >= 0) atari_decode_key(KBD_JOY_LEFT , 1);
  } else {
    if (old_Lx >  0) atari_decode_key(KBD_JOY_RIGHT , 0);
    else
    if (old_Lx <  0) atari_decode_key(KBD_JOY_LEFT, 0);
  }

  if (new_Ly > 0) {
    if (old_Ly <  0) atari_decode_key(KBD_JOY_UP , 0);
    if (old_Ly <= 0) atari_decode_key(KBD_JOY_DOWN, 1);
  } else 
  if (new_Ly < 0) {
    if (old_Ly >  0) atari_decode_key(KBD_JOY_DOWN, 0);
    if (old_Ly >= 0) atari_decode_key(KBD_JOY_UP , 1);
  } else {
    if (old_Ly >  0) atari_decode_key(KBD_JOY_DOWN , 0);
    else
    if (old_Ly <  0) atari_decode_key(KBD_JOY_UP, 0);
  }

  for (b = 0; b < KBD_MAX_BUTTONS; b++) 
  {
    if (c.Buttons & loc_button_mask[b]) {
# if 0  //GAME MODE !
      if (!(loc_button_data.Buttons & loc_button_mask[b])) 
# endif
      {
        loc_button_press[b] = 1;
        event = 1;
      }
    } else {
      if (loc_button_data.Buttons & loc_button_mask[b]) {
        loc_button_release[b] = 1;
        loc_button_press[b] = 0;
        event = 1;
      }
    }
  }
  memcpy(&loc_button_data,&c,sizeof(SceCtrlData));

  return event;
}

void
kbd_wait_start(void)
{
  while (1)
  {
    SceCtrlData c;
    myCtrlPeekBufferPositive(&c, 1);
    c.Buttons &= PSP_ALL_BUTTON_MASK;
    if (c.Buttons & PSP_CTRL_START) break;
  }
  psp_kbd_wait_no_button();
}

void
psp_init_keyboard(void)
{
  atari_kbd_reset();
  kbd_ltrigger_mapping_active = 0;
  kbd_rtrigger_mapping_active = 0;
}

void
psp_kbd_wait_no_button(void)
{
  SceCtrlData c;

  do {
   myCtrlPeekBufferPositive(&c, 1);
   c.Buttons &= PSP_ALL_BUTTON_MASK;

  } while (c.Buttons != 0);
} 

void
psp_kbd_wait_button(void)
{
  SceCtrlData c;

  do {
   myCtrlPeekBufferPositive(&c, 1);
  } while (c.Buttons == 0);
} 

int
psp_update_keys(void)
{
  int         b;

  static char first_time = 1;
  static int release_pending = 0;

  if (first_time) {

    memcpy(psp_kbd_mapping, loc_default_mapping, sizeof(loc_default_mapping));
    memcpy(psp_kbd_mapping_L, loc_default_mapping_L, sizeof(loc_default_mapping_L));
    memcpy(psp_kbd_mapping_R, loc_default_mapping_R, sizeof(loc_default_mapping_R));

    atari_kbd_load();

    SceCtrlData c;
    myCtrlPeekBufferPositive(&c, 1);
    c.Buttons &= PSP_ALL_BUTTON_MASK;

    if (first_time_stamp == -1) first_time_stamp = c.TimeStamp;
    if ((! c.Buttons) && ((c.TimeStamp - first_time_stamp) < KBD_MIN_START_TIME)) return 0;

    first_time      = 0;
    release_pending = 0;

    for (b = 0; b < KBD_MAX_BUTTONS; b++) {
      loc_button_release[b] = 0;
      loc_button_press[b] = 0;
    }
    myCtrlPeekBufferPositive(&loc_button_data, 1);
    loc_button_data.Buttons &= PSP_ALL_BUTTON_MASK;

    psp_main_menu();
    psp_init_keyboard();

    return 0;
  }

  atari_apply_cheats();

  if (danzeff_mode) {
    psp_kbd_enter_danzeff();
    return 0;
  }

# ifdef USE_PSP_IRKEYB
  if (psp_kbd_enter_irkeyb()) {
    return 0;
  }
# endif

  if (release_pending)
  {
    release_pending = 0;
    for (b = 0; b < KBD_MAX_BUTTONS; b++) {
      if (loc_button_release[b]) {
        loc_button_release[b] = 0;
        loc_button_press[b] = 0;
        atari_decode_key(b, 0);
      }
    }
  }

  kbd_scan_keyboard();

  /* check press event */
  for (b = 0; b < KBD_MAX_BUTTONS; b++) {
    if (loc_button_press[b]) {
      loc_button_press[b] = 0;
      release_pending     = 0;
      atari_decode_key(b, 1);
    }
  }
  /* check release event */
  for (b = 0; b < KBD_MAX_BUTTONS; b++) {
    if (loc_button_release[b]) {
      release_pending = 1;
      break;
    }
  }

  return 0;
}
