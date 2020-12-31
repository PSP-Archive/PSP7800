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

# ifndef _PSP_FMGR_H_
# define _PSP_FMGR_H_

#ifdef __cplusplus
extern "C" {
#endif

# define PSP_FMGR_MAX_PATH    512
# define PSP_FMGR_MAX_NAME    256
# define PSP_FMGR_MAX_ENTRY  2048

# define FMGR_FORMAT_ROM   1
# define FMGR_FORMAT_STATE 2
# define FMGR_FORMAT_KBD   3
# define FMGR_FORMAT_CHT   4
# define FMGR_FORMAT_ZIP   5
# define FMGR_FORMAT_SET   6

  extern int psp_fmgr_menu(int format);
  extern int psp_fmgr_getExtId(const char *szFilePath);
  extern int psp_fmgr_get_dir_list(char *basedir, int dirmax, char **dirname);

#ifdef __cplusplus
}
#endif

# endif
