
    Welcome to PSP7800

Original Author of ProSystem

  Greg Stanton (see http://home.comcast.net/~gscottstanton/) 

Author of the PSP port version 

  Ludovic.Jacomme also known as Zx-81 (see http://zx81.zx81.free.fr)


1. INTRODUCTION
   ------------

  ProSystem is the best emulator of Atari 7800 game console, 
  running on Windows system.

  PSP7800 is a port on PSP of the version v1.1 of ProSystem.

  Big thanks to Danno aka Carpy for his help on game compatibility issues,
  sound code and NTSC color palette !

  Thanks to Gelon for his beautiful eboot icons and to all PSPSDK developpers.

  Eboot music written by NeXuS (see from http://www.jamendo.com/en/artist/nexus)

  This package is under GPL Copyright, read COPYING file for
  more information about it.


2. INSTALLATION
   ------------

  Unzip the zip file, and copy the content of the directory fw5x
  on the directory psp/game5xx

  Put your roms files on "roms" sub-directory. 

  If you want to use the original 7800 Bios, then put the file called
  7800.rom in the psp/game5xx/psp7800 directory.

  It has been developped on linux for Firmware 5.01-M33 but it should 
  work for all others M33 firmwares.

  For any comments or questions on this version, please visit 
  http://zx81.zx81.free.fr or http://zx81.dcemu.co.uk


3. CONTROL
   ------------

3.1 - Virtual keyboard

In the 7800 emulator window, there are three different mapping (standard, left
trigger, and right Trigger mappings).  You can toggle between while playing
inside the emulator using the two PSP trigger keys.

    -------------------------------------
    PSP        Atari 7800  (standard)
  
    Cross      Fire1
    Square     Fire2
    Triangle   L Diff
    Circle     R Diff

    Left       Left
    Down       Down
    Right      Right
    Up         Up

    Analog     Joystick
    -------------------------------------
    PSP        Atari 7800  (left trigger)

    Square     Hotkey FPS
    Triangle   Hotkey Load state
    Circle     Swap digital / Analog
    Cross      SAVE Snapshot
    Up         Inc delta Y
    Down       Dec delta Y
    Left       Render mode
    Right      Render mode

    -------------------------------------
    PSP        Atari 7800  (right trigger)

    Square     Pause 
    Triangle   Reset 
    Cross      Auto-fire
    Circle     Select

    Up         Up
    Down       Down
    Left       Dec Fire
    Right      Inc Fire

    Analog     Joystick


Press Start+L+R to exit and return to XMB
Press Select    to enter in emulator main menu.
Press Start     open/close the On-Screen keyboard

In the main menu

  RTrigger   Reset the emulator

  Triangle   Go Up directory
  Cross      Valid
  Circle     Valid
  Square     Go Back to the emulator window

The On-Screen Keyboard of "Danzel" and "Jeff Chen"

Use Analog stick to choose one of the 9 squares, and use Triangle, Square,
Cross and Circle to choose one of the 4 letters of the highlighted square.

Use LTrigger and RTrigger to see other 9 squares figures.

3.2 - IR keyboard

  You can also use IR keyboard. Edit the
pspirkeyb.ini file to specify your IR keyboard
model, and modify eventually layout keyboard files
in the keymap directory.

The following mapping is done :

  IR-keyboard   PSP

  Cursor        Digital Pad

  Tab           Start
  Ctrl-W        Start

  Escape        Select
  Ctrl-Q        Select

  Ctrl-E        Triangle
  Ctrl-X        Cross
  Ctrl-S        Square
  Ctrl-F        Circle
  Ctrl-Z        L-trigger
  Ctrl-C        R-trigger

In the emulator window you can use the IR keyboard to
enter letters, special characters and digits.

4. LOADING ROM FILES (.A78)
   ------------

If you want to load rom images in the virtual drive of your emulator, you have
to put your rom file (with .zip or .a78 file extension) on your PSP memory
stick in the 'roms' directory. 

Then, while inside Atari 7800 emulator, just press SELECT to enter in the
emulator main menu, choose "Load ROM" and then using the file selector choose
one game file to load in your emulator. 

You can use the virtual keyboard in the file requester menu to choose the
first letter of the game you search (it might be useful when you have tons of
games in the same folder). Entering several time the same letter let you
choose sequentially files beginning with the given letter. You can use the Run
key of the virtual keyboard to launch the rom.

5. CHEAT CODE (.CHT)
   ----------

You can use cheat codes with PSP-7800 emulator.  You can add your own cheat
codes in the cheat.txt file and then import them in the cheat menu.  

All cheat codes you have specified for a game can be save in a CHT file in
'cht' folder.  Those cheat codes would then be automatically loaded when you
start the game.

The CHT file format is the following :
#
# Enable, Address, Value, Comment
#
1,36f,3,Cheat comment

Using the Cheat menu you can search for modified bytes in RAM between current
time and the last time you saved the RAM. It might be very usefull to find
"poke" address by yourself, monitoring for example life numbers.

To find a new "poke address" you can proceed as follow :

Let's say you're playing Xevious and you want to find the memory address where
"number lives" is stored.

. Start a new game in Xevious
. Enter in the cheat menu. 
. Choose Save Ram to save initial state of the memory. 
. Specify the number of lives you want to find in
  "Scan Old Value" field.
  (for Xevious the initial lives number is 4)
. Go back to the game and loose a life.
. Enter in the cheat menu. 
. Specify the number of lives you want to find in
  "Scan New Value" field.
  (for Xevious the lives number is now 3)
. In Add Cheat you have now one matching Address
  (for Xevious it's 1A6E)
. Specify the Poke value you want (for example 4) 
  and add a new cheat with this address / value.

The cheat is now activated in the cheat list and you can save it using the
"Save cheat" menu.

Let's enjoy Xevious with infinite life !!

6. COMMENTS
   ------------

You can write your own comments for games using the "Comment" menu.  The first
line of your comments would then be displayed in the file requester menu while
selecting the given file name (roms, keyboard, settings).


7. SETTINGS
   ------------

You can modify several settings value in the settings
menu of this emulator.  The following parameters are
available :

  Sound enable : 
    enable or disable the sound

  Display fps : 
    display real time fps value 

  Speed limiter :
    limit the speed to a given fps value

  Skip frame : 
    to skip frame and increase emulator speed

  Render mode : 
    many render modes are available with different
    geometry that should covered all games
    requirements

  Delta Y : 
    move the center of the screen vertically

  Vsync : 
    wait for vertical signal between each frame displayed

  Swap Analog/Cursor : 
    swap key mapping between PSP analog pad and PSP
    digital pad

  Active Joystick : 
    Joystick player, it could be 1 or 2

  Auto fire period : 
    auto fire period

  Auto fire mode : 
    auto fire mode active or not

  Display LR led : 
    draw a small blue rectangle in top of the screen
    when trigger keys are pressed

  Clock frequency : 
    PSP clock frequency, by default the value is set
    to 222Mhz, and should be enough for most of all
    games.

  Enjoy,

  If you want to load rom images in the virtual drive of your emulator,
  you have to put your rom file (with .zip or .a78 file extension) on your PSP
  memory stick in the 'roms' directory. 

  Then, while inside Atari 7800 emulator, just press SELECT to enter in the emulator 
  main menu, choose "Load ROM" and then using the file selector choose one game 
  file to load in your emulator. Back to the emulator window, your game should 
  run automatically.


8. LOADING KEY MAPPING FILES
   ------------

  For given games, the default keyboard mapping between PSP Keys and Atari 7800
  keys, is not suitable, and the game can't be played on PSP7800.

  To overcome the issue, you can write your own mapping file. Using notepad for
  example you can edit a file with the .kbd extension and put it in the kbd 
  directory.

  For the exact syntax of those mapping files, have a look on sample files already
  presents in the kbd directory (default.kbd etc ...).

  After writting such keyboard mapping file, you can load them using 
  the main menu inside the emulator.

  If the keyboard filename is the same as the rom file (.a78) then 
  when you load this rom file, the corresponding keyboard file is 
  automatically loaded !

  You can now use the Keyboard menu and edit, load and save your
  keyboard mapping files inside the emulator. The Save option save the .kbd
  file in the kbd directory using the "Game Name" as filename. The game name
  is displayed on the right corner in the emulator menu.

  
9. COMPILATION
   ------------

  It has been developped under Linux using gcc with PSPSDK. 
  To rebuild the homebrew run the Makefile in the src archive.

