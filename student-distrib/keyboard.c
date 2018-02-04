#include "keyboard.h"
#include "syscall.h"
#include "scheduling.h"
#define keyboard_port  0x60

/* Declare characters to display on screen (no modifier flags)*/
static char array_of_char[128] = {
  0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
'9', '0', '-', '=', '\b',	/* Backspace */
'\t',			/* Tab */
'q', 'w', 'e', 'r',	/* 19 */
't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
 0,			/* 29   - Control */
'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
'\'', '`',   0,		/* Left shift */
'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
'm', ',', '.', '/',   0,				/* Right shift */
'*',
 0,	/* Alt */
' ',	/* Space bar */
 0,	/* Caps lock */
 0,	/* 59 - F1 key ... > */
 0,   0,   0,   0,   0,   0,   0,   0,
 0,	/* < ... F10 */
 0,	/* 69 - Num lock*/
 0,	/* Scroll Lock */
 0,	/* Home key */
 0,	/* Up Arrow */
 0,	/* Page Up */
'-',
 0,	/* Left Arrow */
 0,
 0,	/* Right Arrow */
'+',
 0,	/* 79 - End key*/
 0,	/* Down Arrow */
 0,	/* Page Down */
 0,	/* Insert Key */
 0,	/* Delete Key */
 0,   0,   0,
 0,	/* F11 Key */
 0,	/* F12 Key */
 0,	/* All other keys are undefined */
};

/* Declare characters to display on screen. (modifier flag shift) */
static char shift_char[128] = {
  0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
'(', ')', '_', '+', '\b',	/* Backspace */
'\t',			/* Tab */
'Q', 'W', 'E', 'R',	/* 19 */
'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
 0,			/* 29   - Control */
'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
'\'', '~',   0,		/* Left shift */
'\\', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
'M', '<', '>', '?',   0,				/* Right shift */
'*',
 0,	/* Alt */
' ',	/* Space bar */
 0,	/* Caps lock */
 0,	/* 59 - F1 key ... > */
 0,   0,   0,   0,   0,   0,   0,   0,
 0,	/* < ... F10 */
 0,	/* 69 - Num lock*/
 0,	/* Scroll Lock */
 0,	/* Home key */
 0,	/* Up Arrow */
 0,	/* Page Up */
'-',
 0,	/* Left Arrow */
 0,
 0,	/* Right Arrow */
'+',
 0,	/* 79 - End key*/
 0,	/* Down Arrow */
 0,	/* Page Down */
 0,	/* Insert Key */
 0,	/* Delete Key */
 0,   0,   0,
 0,	/* F11 Key */
 0,	/* F12 Key */
 0,	/* All other keys are undefined */
};

int shift_flag = 0;
int ctrl_flag = 0;
int alt_flag = 0;
int caps_lock = 0;

/*
* void keyboard_init()
*   Inputs: none
*   Return Value: none
*	Function: Enables IRQ 1 for keyboard interrupts
*/
void keyboard_init(){
  enable_irq(1);

  shift_flag = 0;
  ctrl_flag = 0;
  alt_flag = 0;
  caps_lock = 0;
}

/*
* void keyboard_handler()
*   Inputs: none
*   Return Value: none
*	Function: echoes pressed key onto kernel
*/
void keyboard_handler(){
    // if(head->screen != indicator) return;
    char z = getScancode();
    // printf("%d", (int)z);
    send_eoi(1);

    // key up
    if(z & 0x80) {
        if (z == -86 || z == -74) {
            shift_flag = 0;
        }
        if (z == -99) {
            ctrl_flag = 0;
        }
        if (z == -72){
            alt_flag = 0;
        }
        return;

    // key down
    } else {
        // REALLY JANKY BUT FUNCTIONAL CODE. might need to CHANGE LATER

        if ((z >= 2 && z <= 53) || (z == 55) || (z == 56) || (z == 57)|| (z == 58)|| (z == 74) || (z == 78) || (z == 59) || (z == 60) ||(z == 61) || (z == 62)) {

          if (z == 42 || z == 54) {
              shift_flag = 1;
              return;
          }
          if (z == 29) {
              ctrl_flag = 1;
              return;
          }

          if (z == 56){
              alt_flag = 1;
              return;
          }

          if (z == 58) {
              if (caps_lock > 0) {
                  caps_lock = 0;
              } else {
                  caps_lock = 1;
              }
              return;
          }

          if (ctrl_flag && (z==38)) { // reset screen when ctrl l
              reset_screen();
              return;
          }

          if (ctrl_flag && (z == 46)) {
              halt(0);
          }

          if (alt_flag && (z == 59)){
              screen1();
              return;
          }

          if (alt_flag && (z == 60)){
              screen2();
              return;
          }



          if (alt_flag && (z == 61)){
              screen3();
              return;
          }

          // if (z == 62){
          //     currProcess = (currProcess % 3) + 1;
          //     // printf("%d\n", currProcess);
          //     return;
          // }

          if (!alt_flag && (z == 59)){
              return;
          }

          if (!alt_flag && (z == 60)){
              return;
          }

          if (!alt_flag && (z == 61)){
              return;
          }

          // need to adjust bounds code NEEDS FIX/CLEANUP (unclean code)
          if (caps_lock && ((z > 15 && z < 26) || (z > 29 && z < 39) || (z > 43 && z < 51))) {
              putc(shift_char[(int)z]);
          } else if (shift_flag && z > 1 && z < 54) {
              putc(shift_char[(int)z]);
          } else {
              putc(array_of_char[(int)z]);
          }

      }
    }
}
/*
* void getScancode()
*   Inputs: none
*   Return Value: char
*	Function: returns keyboard port value
*/
char getScancode()
{
  //char c=0;
  char inkey = inb(keyboard_port);
  return inkey;
}


/* unused for now */
char getchar()
{
  // return 'A';
  return array_of_char[(int)getScancode()];
}
