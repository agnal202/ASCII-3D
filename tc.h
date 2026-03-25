#ifndef TC_H
#define TC_H

#include<stdio.h>
#include<termios.h>
#include<unistd.h>
#include <sys/ioctl.h>

///////////////////////////////////////////////////////
////                  PROTOTYPES                   ////
///////////////////////////////////////////////////////

void set_raw_mode(int enable);
void get_terminal_size(int *rows, int *cols);

void save_cursor_pos(int *row, int *col);
void move_cursor(int row,int col);

void refresh(){ fflush(stdout); }
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


// ========== RESET ==========
#define NRM "\x1b[0m"

// ========== TEXT ATTRIBUTES ==========
#define BLD "\x1b[1m"
#define DIM "\x1b[2m"
#define ITL "\x1b[3m"
#define UL "\x1b[4m"
#define BLINK "\x1b[5m"
#define REVERSE "\x1b[7m"
#define HIDDEN "\x1b[8m"
#define STRIKE "\x1b[9m"

// Reset individual attributes
#define RESET_BLD "\x1b[21m"
#define RESET_DIM "\x1b[22m"
#define RESET_ITL "\x1b[23m"
#define RESET_UL  "\x1b[24m"
#define RESET_BLINK "\x1b[25m"
#define RESET_REVERSE "\x1b[27m"
#define RESET_HIDDEN "\x1b[28m"
#define RESET_STRIKE "\x1b[29m"

// ========== STANDARD COLORS ==========
#define BLK "\x1b[30m"
#define RED "\x1b[31m"
#define GRN "\x1b[32m"
#define YLW "\x1b[33m"
#define BLU "\x1b[34m"
#define MGN "\x1b[35m"
#define CYN "\x1b[36m"
#define WHT "\x1b[37m"

#define BLKBG "\x1b[40m"
#define REDBG "\x1b[41m"
#define GRNBG "\x1b[42m"
#define YLWBG "\x1b[43m"
#define BLUBG "\x1b[44m"
#define MGNBG "\x1b[45m"
#define CYNBG "\x1b[46m"
#define WHTBG "\x1b[47m"

// ========== BRIGHT COLORS ==========
#define BBLK "\x1b[90m"
#define BRED "\x1b[91m"
#define BGRN "\x1b[92m"
#define BYLW "\x1b[93m"
#define BBLU "\x1b[94m"
#define BMGN "\x1b[95m"
#define BCYN "\x1b[96m"
#define FG_BWHT "\x1b[97m"

#define BBLKBG "\x1b[100m"
#define BREDBG "\x1b[101m"
#define BGRNBG "\x1b[102m"
#define BYLWBG "\x1b[103m"
#define BBLUBG "\x1b[104m"
#define BMGNBG "\x1b[105m"
#define BCYNBG "\x1b[106m"
#define BWHTBG "\x1b[107m"

// ========== 256-COLOR MODE ==========
#define FG256(n) "\x1b[38;5;" #n "m"
#define BG256(n) "\x1b[48;5;" #n "m"

// ========== TRUE COLOR (24-bit RGB) ==========
#define FGRGB(r, g, b) "\x1b[38;2;" #r ";" #g ";" #b "m"
#define BGRGB(r, g, b) "\x1b[48;2;" #r ";" #g ";" #b "m"

// ========== CURSOR MOVEMENT ==========
#define CURUP(n) "\x1b[" #n "A"
#define CURDWN(n) "\x1b[" #n "B"
#define CURRIGHT(n) "\x1b[" #n "C"
#define CURLEFT(n) "\x1b[" #n "D"
#define CURNL(n) "\x1b[" #n "E"
#define CURPL(n) "\x1b[" #n "F"
#define CURCOL(n) "\x1b[" #n "G"
#define CURPOS(r, c) "\x1b[" #r ";" #c "H"

#define HIDECUR "\x1b[?25l"
#define SHOWCUR "\x1b[?25h"

// ========== SAVE/RESTORE CURSOR ==========
#define REQCUR "\x1b[6n"
#define CURSAVE "\x1b[s"
#define CURRESTORE "\x1b[u"

// ========== ERASE ==========
#define CLRSCR "\x1b[2J"
#define CLRBLW "\x1b[0J"
#define CLRABV "\x1b[1J"

#define CLRLINE "\x1b[2K"
#define CLRRIGHT "\x1b[0K"
#define CLRLEFT "\x1b[1K"

// ========== SCROLLING ==========
#define SCROLLUP(n) "\x1b[" #n "S" 
#define SCROLLDOWN(n) "\x1b[" #n "T"

// ========== OTHER CONTROLS ==========
#define BELL "\x07"
#define ALTSCRON "\x1b[?1049h"
#define ALTSCROFF "\x1b[?1049l"

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

void set_raw_mode(int enable)
{
    struct termios ttystate;

    tcgetattr(STDIN_FILENO, &ttystate);

    if (enable) {
        ttystate.c_lflag &= ~(ICANON | ECHO); // Raw mode
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
        
    } else {
        ttystate.c_lflag |= ICANON | ECHO; // Back to normal
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
    }
}

void get_terminal_size(int *rows, int *cols) {
    struct winsize w;
    
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    *rows = w.ws_row;
    *cols = w.ws_col;
}

void save_cursor_pos(int *row, int *col)
{
	unsigned int i=0;
	char buff[10];
	char ch;
	
	printf(REQCUR);
	fflush(stdout);
	
	set_raw_mode(1);
	
	while(1)
	{
		ch = getchar();
		if (ch == 'R')	break;
		buff[i++] = ch;
	}
	
	set_raw_mode(0);
	
	buff[i] = '\0';
	
    sscanf(buff, "\033[%d;%d", row, col);
}

void move_cursor(int row,int col)
{
	printf("\033[%d;%dH", row,col);
} 

#endif // TC_H