/**
 * @file
 * @author Hamik Mukelyan
 *
 * Drives a text-based Flappy Bird knock-off that is intended to run in an
 * 80 x 24 console.
 */

/**
 * @author Toufupi
 * @data 2021.7.24
 * 1.添加中文批注
 * 2.管道生成后可以运动
 * 3.结束界面分数和空格键重新开始，修复历史最高分的BUG
 * @data 2021.7.25
 * 1.数据持久保存化历史最高分
 * 2.难度选择界面
 * 3.多人游戏选择界面
 * 4.音频部分添加完毕
 * @data 2021.7.26
 * 1.合并界面窗口
 * @TODO
 * 1.修改代码风格，调整选择界面函数，将其合并到函数定义区
 * 2.多人游戏逻辑，选择多人后进入登录界面
 * 3.socket编程研究，
 * 4.地图同步问题的解决
 * @data 2021.7.27
 * 研究socket
 * @data 2021.7.28
 * 研究socket
 * 优化部分代码，历史最高分分别统计
 * @data 2021..729
 *
 */

#include <ncurses.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include "playaudio.h"
#include <arpa/inet.h>
#include <sys/socket.h>



//-------------------------------- Definitions --------------------------------

/**
 * Represents a vertical pipe through which Flappy The Bird is supposed to fly .
 */

 //垂直的管道，相对屏幕高度的百分比是管道高度，center是在整个屏幕里的列数位置。
typedef struct vpipe {

	float opening_height;

	int center;
} vpipe;

/** Represents Flappy the Bird.鸟的列位置不变,只需要高度数据 */
typedef struct flappy {
	/* Height of Flappy the Bird at the last up arrow press. 记录高度*/
	int h0;
	/* Time since last up arrow pressed.记录时间 */
	int t;
} flappy;

typedef struct bestscore {
    int easy;
    int mid;
    int hard;
}bestscore;

typedef struct Gamedata{
    int point_p1;
    int point_p2;
    int pos_p1;
    int pos_p2;
    int start; //标记是否可以开始游戏
}Gamedata;

typedef struct Updata{
    int point; //得分
    int pos;  //位置
    int isstart;
    int regame; //重置
}Updata;

//------------------------------ Global Constants -----------------------------

/** Gravitational acceleration constant */
const float GRAV = 0.05;

/** Initial velocity with up arrow press 一按下去就改速度？*/
const float V0 = -0.5;

/** Number of rows in the console window. 行数*/
const int NUM_ROWS = 24;

/** Number of columns in the console window. 列数*/
const int NUM_COLS = 80;

/** Radius of each vertical pipe. 柱子的半径，难度控制*/
int PIPE_RADIUS = 3; //easy 3 mid 5 hard 7

/** Width of the opening in each pipe. 垂直柱子中间的开口，难度控制*/
int OPENING_WIDTH = 7; //easy 7 mid 5 hard 3

/** Flappy stays in this column. 暂时不知道是什么*/
const int FLAPPY_COL = 10;

/** Aiming for this many frames per second. */
float TARGET_FPS = 24;  //easy 24 mid 35 hard 45

/** Amount of time the splash screen stays up. */
const float START_TIME_SEC = 3;

/** Length of the "progress bar" on the status screen. */
const int PROG_BAR_LEN = 76;

/** Row number at which the progress bar will show. */
const int PROG_BAR_ROW = 22;

const int SCORE_START_COL = 62;

FILE *fscore;

pthread_t bgm; //背景音乐播放进程

pthread_t maingame;

pthread_t wingsound; //wing音效进程

pthread_t hitsound;

pthread_t pointsound;

pthread_t comm_socket;  //网络通讯进程

#define MAXBUFF 1024
#define PORT 8081 //8081 for clietn1 8082 for client 2
char* SERV_ADDR  = "1.116.101.74"; // 修改为服务器地址

Gamedata data;
Updata up;

char buffer[MAXBUFF];
char recv_buffer[MAXBUFF];

//------------------------------ Global Variables -----------------------------

/** Frame number. */
int frame = 0;

/** Number of pipes that have been passed. */
int score = 0;

/** Number of digits in the score. */
int sdigs = 1;

/** Best score so far. */
int best_score = 0;

bestscore hscore;



/** Number of digits in the best score. */
int bdigs = 1;

/** The vertical pipe obstacles.始终只有两个柱子出现 */
vpipe p1, p2;

// 音效标识
int swooshing = 0;
int hit = 0;
int point = 0;
int failure = 0;

//以下是难度选择需要的变量

#define WIDTH 21
#define HEIGHT 10
int startx = 0;
int starty = 0;
int dif = 0;
char *choices[] = {
        "Choice 1 : easy","Choice 2 : Normal ","Choice 3 : hard","Exit",
};
int n_choices = sizeof(choices) / sizeof(char *);
void print_menu(WINDOW *menu_win, int highlight);

//以下是多人游戏需要的变量

#define WIDTH2 27
#define HEIGHT2 8
int startx2 = 0;
int starty2 = 0;
int gamemode = 0;
char *choices2[] = {
        "Choice 1 : Single","Choice 2 : Multiplayer ","Exit",
};
int n_choices2 = sizeof(choices2) / sizeof(char *);
void print_menu2(WINDOW *menu_game, int highlight2);


//---------------------------------- Functions --------------------------------

/**
 * Converts the given char into a string.
 *
 * @param ch Char to convert to a string.
 * @param[out] str Receives 'ch' into a null-terminated C string. Assumes
 * str had 2 bytes allocated.
 */
void chtostr(char ch, char *str) {
	str[0] = ch;
	str[1] = '\0';
}

/**
 * "Moving" floor and ceiling are written into the window array.
 *
 * @param ceiling_row
 * @param floor_row
 * @param ch Char to use for the ceiling and floor.
 * @param spacing Between chars in the floor and ceiling
 * @param col_start Stagger the beginning of the floor and ceiling chars
 * by this much
 */

void* comm(void* args) {
    int temp = 0;

    //memset(buffer,0,MAXBUFF);
    //memcpy(buffer,&up,sizeof (up));
    //处理发送的结构体，可以考虑共用体,可以不用了，功能实现的很完美；
    //memset(recv_buffer,0,MAXBUFF);
    memset(&data,0,sizeof (data));
    //初始化空间

    //创建套接字

    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(SERV_ADDR);  //具体的IP地址
    serv_addr.sin_port = htons(PORT);  //端口

    while(1) {
        if(temp!=up.point||data.start==0||up.regame==1) {
            memset(buffer, 0, MAXBUFF);
            memset(recv_buffer, 0, MAXBUFF);
            memcpy(buffer, &up, sizeof(up));
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

            // write(sock,str, strlen(str)); //传数据
            send(sock, buffer, sizeof(buffer), 0); //发送数据包

            recv(sock, recv_buffer, 1024, 0);
            memcpy(&data, recv_buffer, sizeof(data));
            temp = up.point;

            if(data.start==0){
                usleep(100000);
            }

            //printf("P1:point %d P2:Point %d",data.point_p1,data.point_p2 );
        }
    }
        //关闭套接字

    return 0;
}

void draw_floor_and_ceiling(int ceiling_row, int floor_row,
		char ch, int spacing, int col_start) {
	char c[2];
	chtostr(ch, c);
	int i;
	for (i = col_start; i < NUM_COLS - 1; i += spacing) {
		if (i < SCORE_START_COL - sdigs - bdigs)
			mvprintw(ceiling_row, i, c);
		mvprintw(floor_row, i, c);
	}
}

/**
 * Updates the pipe center and opening height for each new frame. If the pipe
 * is sufficiently far off-screen to the left the center is wrapped around to
 * the right, at which time the opening height is changed.
 */
void pipe_refresh(vpipe *p) {

	// If pipe exits screen on the left then wrap it to the right side of the
	// screen.
	if(p->center + PIPE_RADIUS < 0) {
		p->center = NUM_COLS + PIPE_RADIUS;

		// Get an opening height fraction.
		p->opening_height = rand() / ((float) INT_MAX) * 0.5 + 0.25;
		score++;
		if(sdigs == 1 && score > 9)
			sdigs++;
		else if(sdigs == 2 && score > 99)
			sdigs++;
	}

	//当距离较远时移动管道位置,每4帧刷新一次刷新一次
	if( p->center + PIPE_RADIUS  > 45 && frame%4 == 0){
        p->opening_height = rand() / ((float) INT_MAX) * 0.5 + 0.25;
	}
	p->center--;
}

/**
 * Gets the row number of the top or bottom of the opening in the given pipe.
 *
 * @param p The pipe obstacle.
 * @param top Should be 1 for the top, 0 for the bottom.
 *
 * @return Row number.
 */
int get_orow(vpipe p, int top) {
	return p.opening_height * (NUM_ROWS - 1) -
			(top ? 1 : -1) * OPENING_WIDTH / 2;
}

/**
 * Draws the given pipe on the window using 'vch' as the character for the
 * vertical part of the pipe and 'hch' as the character for the horizontal
 * part.
 *
 * @param p
 * @param vch Character for vertical part of pipe
 * @param hcht Character for horizontal part of top pipe
 * @param hchb Character for horizontal part of lower pipe
 * @param ceiling_row Start the pipe just below this
 * @param floor_row Star the pipe jut above this
 */
void draw_pipe(vpipe p, char vch, char hcht, char hchb,
		int ceiling_row, int floor_row) {
	int i, upper_terminus, lower_terminus;
	char c[2];

	// Draw vertical part of upper half of pipe.
	for(i = ceiling_row + 1; i < get_orow(p, 1); i++) {
		if ((p.center - PIPE_RADIUS) >= 0 &&
				(p.center - PIPE_RADIUS) < NUM_COLS - 1) {
			chtostr(vch, c);
			mvprintw(i, p.center - PIPE_RADIUS, c);
		}
		if ((p.center + PIPE_RADIUS) >= 0 &&
				(p.center + PIPE_RADIUS) < NUM_COLS - 1) {
			chtostr(vch, c);
			mvprintw(i, p.center + PIPE_RADIUS, c);
		}
	}
	upper_terminus = i;

	// Draw horizontal part of upper part of pipe.
	for (i = -PIPE_RADIUS; i <= PIPE_RADIUS; i++) {
		if ((p.center + i) >= 0 &&
				(p.center + i) < NUM_COLS - 1) {
			chtostr(hcht, c);
			mvprintw(upper_terminus, p.center + i, c);
		}
	}

	// Draw vertical part of lower half of pipe.
	for(i = floor_row - 1; i > get_orow(p, 0); i--) {
		if ((p.center - PIPE_RADIUS) >= 0 &&
				(p.center - PIPE_RADIUS) < NUM_COLS - 1) {
			chtostr(vch, c);
			mvprintw(i, p.center - PIPE_RADIUS, c);
		}
		if ((p.center + PIPE_RADIUS) >= 0 &&
				(p.center + PIPE_RADIUS) < NUM_COLS - 1) {
			chtostr(vch, c);
			mvprintw(i, p.center + PIPE_RADIUS, c);
		}
	}
	lower_terminus = i;

	// Draw horizontal part of lower part of pipe.
	for (i = -PIPE_RADIUS; i <= PIPE_RADIUS; i++) {
		if ((p.center + i) >= 0 &&
				(p.center + i) < NUM_COLS - 1) {
			chtostr(hchb, c);
			mvprintw(lower_terminus, p.center + i, c);
		}
	}
}

/**
 * Get Flappy's height along its parabolic arc.
 *
 * @param f Flappy!
 *
 * @return height as a row count
 */
int get_flappy_position(flappy f) {
	return f.h0 + V0 * f.t + 0.5 * GRAV * f.t * f.t;
}

/**
 * Returns true if Flappy crashed into a pipe.
 *
 * @param f Flappy!
 * @param p The vertical pipe obstacle.
 *
 * @return 1 if Flappy crashed, 0 otherwise.
 */
int crashed_into_pipe(flappy f, vpipe p) {
	if (FLAPPY_COL >= p.center - PIPE_RADIUS - 1 &&
			FLAPPY_COL <= p.center + PIPE_RADIUS + 1) {

		if (get_flappy_position(f) >= get_orow(p, 1)  + 1 &&
				get_flappy_position(f) <= get_orow(p, 0) - 1) {
			return 0;
		}
		else {
			return 1;
		}
	}
	return 0;
}

/**
 * Prints a failure screen asking the user to either play again or quit.
 *
 * @return 1 if the user wants to play again. Exits the program otherwise.
 */
int failure_screen() {
	char ch;
	clear();
	if(gamemode==1){
	mvprintw(NUM_ROWS / 2 - 1, NUM_COLS / 2 - 22,
			"Flappy died :-(. <Enter> to flap, 'q' to quit.\n");

    mvprintw(NUM_ROWS / 2 +1, NUM_COLS / 2 - 23,
             " Score: %d  Best: %d \n", score, best_score);
	refresh();
	timeout(-1); // Block until user enters something.
	ch = getch();
	switch(ch) {
        case 'q': // Quit.

            endwin();

            failure = 1;
            break;

        default:
            if (score > best_score)
                best_score = score;
            if (bdigs == 1 && best_score > 9)
                bdigs++;
            else if (bdigs == 2 && best_score > 99)
                bdigs++;
            score = 0;
            sdigs = 1;
            hit = 0;
            return 1; // Restart game.
        }

	}else if(gamemode==2){

	    up.regame == 1;

        if (data.point_p1 > data.point_p2){
            mvprintw(NUM_ROWS / 2 - 1, NUM_COLS / 2 - 22,
                     "YUO WIN!!! Press any key to exit\n");
        }

        if(data.point_p1 < data.point_p2){
            mvprintw(NUM_ROWS / 2 - 1, NUM_COLS / 2 - 22,
                     "You lose! Try next time.Press any key to exit\n");
        }

        if(data.point_p1 == data.point_p2){
            mvprintw(NUM_ROWS / 2 - 1, NUM_COLS / 2 - 22,
                     "GOOD!! You two are both great! Press any key to exit\n");
        }
        refresh();
        getchar();
	    endwin();
	    exit(0);

	}
	//endwin();
	//exit(0);
}

/**
 * Draws Flappy to the screen and shows death message if Flappy collides with
 * ceiling or floor. The user can continue to play or can exit if Flappy
 * dies.
 *
 * @param f Flappy the bird!
 *
 * @return 0 if Flappy was drawn as expected, 1 if the game should restart.
 */
int draw_flappy(flappy f) {
	char c[2];
	int h = get_flappy_position(f);

	// If Flappy crashed into the ceiling or the floor...
	if (h <= 0 || h >= NUM_ROWS - 1){
        pthread_create(&hitsound,NULL,playhit,NULL);
        hit = 1;
        return failure_screen();
	}


	// If Flappy crashed into a pipe...
	if (crashed_into_pipe(f, p1) || crashed_into_pipe(f, p2)) {
        pthread_create(&hitsound,NULL,playhit,NULL);
        hit = 1;
		return failure_screen();
	}else{
        //pthread_create(&pointsound,NULL,playpoint,NULL);
	}

	// If going down, don't flap
	if (GRAV * f.t + V0 > 0) {
		chtostr('\\', c);
		mvprintw(h, FLAPPY_COL - 1, c);
		mvprintw(h - 1, FLAPPY_COL - 2, c);
		chtostr('o', c);
		mvprintw(h, FLAPPY_COL, c);
		chtostr('/', c);
		mvprintw(h, FLAPPY_COL + 1, c);
		mvprintw(h - 1, FLAPPY_COL + 2, c);
	}

	// If going up, flap!
	else {
		// Left wing
		if (frame % 6 < 3) {
			chtostr('/', c);
			mvprintw(h, FLAPPY_COL - 1, c);
			mvprintw(h + 1, FLAPPY_COL - 2, c);
		}
		else {
			chtostr('\\', c);
			mvprintw(h, FLAPPY_COL - 1, c);
			mvprintw(h - 1, FLAPPY_COL - 2, c);
		}

		// Body
		chtostr('O', c);
		mvprintw(h, FLAPPY_COL, c);

		// Right wing
		if (frame % 6 < 3) {
			chtostr('\\', c);
			mvprintw(h, FLAPPY_COL + 1, c);
			mvprintw(h + 1, FLAPPY_COL + 2, c);
		}
		else {
			chtostr('/', c);
			mvprintw(h, FLAPPY_COL + 1, c);
			mvprintw(h - 1, FLAPPY_COL + 2, c);
		}
	}

	return 0;
}

/**
 * Print a splash screen and show a progress bar. NB the ASCII art was
 * generated by patorjk.com.
 */
void splash_screen() {
	int i;
	int r = NUM_ROWS / 2 - 6;
	int c = NUM_COLS / 2 - 22;

	// Print the title.
	mvprintw(r, c,     " ___ _                       ___ _        _ ");
	mvprintw(r + 1, c, "| __| |__ _ _ __ _ __ _  _  | _ |_)_ _ __| |");
	mvprintw(r + 2, c, "| _|| / _` | '_ \\ '_ \\ || | | _ \\ | '_/ _` |");
	mvprintw(r + 3, c, "|_| |_\\__,_| .__/ .__/\\_, | |___/_|_| \\__,_|");
	mvprintw(r + 4, c, "           |_|  |_|   |__/                  ");
	mvprintw(NUM_ROWS / 2 + 1, NUM_COLS / 2 - 10,
			"Press <space> to flap!");

	// Print the progress bar.
	mvprintw(PROG_BAR_ROW, NUM_COLS / 2 - PROG_BAR_LEN / 2 - 1, "[");
	mvprintw(PROG_BAR_ROW, NUM_COLS / 2 + PROG_BAR_LEN / 2, "]");
	refresh();
	for(i = 0; i < PROG_BAR_LEN; i++) {
		usleep(1000000 * START_TIME_SEC / (float) PROG_BAR_LEN);
		mvprintw(PROG_BAR_ROW, NUM_COLS / 2 - PROG_BAR_LEN / 2 + i, "=");
		refresh();
	}
	usleep(1000000 * 0.5);
}
//------------------------------------ Menu ---------------------------------------
void splash_menu(WINDOW *menu_win, int highlight)
{

    int x,y,i;
    x= 2;
    y= 2;
    int r = NUM_ROWS / 2 - 10;
    int c = NUM_COLS / 2 - 22;

    // Print the title.
    mvprintw(r, c,     " ___ _                       ___ _        _ ");
    mvprintw(r + 1, c, "| __| |__ _ _ __ _ __ _  _  | _ |_)_ _ __| |");
    mvprintw(r + 2, c, "| _|| / _` | '_ \\ '_ \\ || | | _ \\ | '_/ _` |");
    mvprintw(r + 3, c, "|_| |_\\__,_| .__/ .__/\\_, | |___/_|_| \\__,_|");
    mvprintw(r + 4, c, "           |_|  |_|   |__/                  ");
    mvprintw(10,16,"Choose Your Difficulty!!!:");

    box(menu_win,0,0);
    refresh();
    for (int (i) = 0; (i) < n_choices; ++(i)) {
        if(highlight == i + 1) {
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        }
        else
            mvwprintw(menu_win,y,x,"%s",choices[i]);
        y += 2;
    }
    wrefresh(menu_win);
}

void splash_multi(WINDOW *menu_game, int highlight2)
{

    int x2,y2,j;
    x2= 2;
    y2= 2;
    int r2 = NUM_ROWS / 2 - 10;
    int c2 = NUM_COLS / 2 - 22;

    // Print the title.
    mvprintw(r2, c2,     " ___ _                       ___ _        _ ");
    mvprintw(r2 + 1, c2, "| __| |__ _ _ __ _ __ _  _  | _ |_)_ _ __| |");
    mvprintw(r2 + 2, c2, "| _|| / _` | '_ \\ '_ \\ || | | _ \\ | '_/ _` |");
    mvprintw(r2 + 3, c2, "|_| |_\\__,_| .__/ .__/\\_, | |___/_|_| \\__,_|");
    mvprintw(r2 + 4, c2, "           |_|  |_|   |__/                  ");
    mvprintw(10,18,"Try Multiplayer mode!!!:");
    box(menu_game,0,0);
    refresh();
    for (int (j) = 0; (j) < n_choices2; ++(j)) {
        if(highlight2 == j + 1) {
            wattron(menu_game, A_REVERSE);
            mvwprintw(menu_game, y2, x2, "%s", choices2[j]);
            wattroff(menu_game, A_REVERSE);
        }
        else
            mvwprintw(menu_game,y2,x2,"%s",choices2[j]);
        y2 += 2;
    }
    wrefresh(menu_game);
}

void setdif(){
    switch (dif){
        case 1:
            best_score = hscore.easy; break;
        case 2:
            best_score = hscore.mid;
            PIPE_RADIUS = 5;
            OPENING_WIDTH = 5;
            TARGET_FPS = 35;
            break;
        case 3:
            best_score = hscore.hard;
            PIPE_RADIUS = 7;
            OPENING_WIDTH = 3;
            TARGET_FPS = 45;
            break;


    }
}
//------------------------------------ MainGame -----------------------------------
 void* game(void* args)
{
	int leave_loop = 0;
	int ch;
	flappy f;
	int restart = 1;

	srand(time(NULL));

	// Initialize ncurses
	initscr();
	raw();					// Disable line buffering
	keypad(stdscr, TRUE);
	noecho();				// Don't echo() for getch
	curs_set(0);
	timeout(0);
	splash_screen();

    //-----------------------------------Multiplayer-----------------------------
    clear();
    WINDOW *menu_game;
    int highlight2 = 1;
    int choice2 = 0;
    int d;
    startx2 = 27;
    starty2 = 12;
    menu_game = newwin(HEIGHT2,WIDTH2,starty2,startx2);
    keypad(menu_game,TRUE);
    splash_multi(menu_game,highlight2);
    refresh();

    while(1) {
        d = wgetch(menu_game);
        switch (d) {
            case KEY_UP:
                if (highlight2 == 1)
                    highlight2 = n_choices2;
                else
                    --highlight2;
                break;

            case KEY_DOWN:
                if (highlight2 == n_choices2)
                    highlight2 = 1;
                else
                    ++highlight2;
                break;
            case 10:
                choice2 = highlight2;
                break;

        }
        splash_multi(menu_game, highlight2);
        if(choice2 != 0){
            if(highlight2 == 1) gamemode = 1;

            if(highlight2 == 2) gamemode = 2;

            if (highlight2 == 3){
                endwin();
                exit(0);
                break;
            }
            break;
        }
    }

//以下是难度选择
if(gamemode==1) {
    clear();
    wclear(menu_game);
    refresh();
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;
    startx = 30;
    starty = 12;
    menu_win = newwin(HEIGHT, WIDTH, starty, startx);
    keypad(menu_win, TRUE);
    splash_menu(menu_win, highlight);
    refresh();

    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = n_choices;
                else
                    --highlight;
                break;

            case KEY_DOWN:
                if (highlight == n_choices)
                    highlight = 1;
                else
                    ++highlight;
                break;
            case 10:
                choice = highlight;
                break;
        }

        splash_menu(menu_win, highlight);
        if (choice != 0) {
            if (highlight == 1) dif = 1;

            if (highlight == 2) dif = 2;

            if (highlight == 3)dif = 3;

            if (highlight == 4) {
                endwin();
                exit(0);
                break;
            }
            break;
        }
    }
    setdif();

}else if(gamemode==2) {
    up.isstart = 1;
    pthread_create(&comm_socket, NULL, comm, NULL);
    mvprintw(8, 18, "Waititng for your opponent to join\n");
    refresh();
    while (!data.start) {}
}




    while(!leave_loop) {
        up.point = score;
		// If we're just starting a game then do some initializations.
		if (restart) {
			timeout(0); // Don't block on input.

			// Start the pipes just out of view on the right.
			p1.center = (int)(1.2 * (NUM_COLS - 1));
			p1.opening_height = rand() / ((float) INT_MAX) * 0.5 + 0.25;
			p2.center = (int)(1.75 * (NUM_COLS - 1));
			p2.opening_height = rand() / ((float) INT_MAX) * 0.5 + 0.25;

			// Initialize flappy
			f.h0 = NUM_ROWS / 2;
			f.t = 0;
			restart = 0;
		}

		usleep((unsigned int) (1000000 / TARGET_FPS));

		// Process keystrokes.
		ch = -1;
		ch = getch();
		switch (ch) {
		case 'q': // Quit.
			endwin();
			exit(0);
			break;
		case 0x20: // Give Flappy a boost! space keycode is 0x20.
            swooshing =1;
			f.h0 = get_flappy_position(f);
			if(gamemode==2){
                up.pos= get_flappy_position(f);
			}
			f.t = 0;
			break;
		default: // Let Flappy fall along his parabola.
			f.t++;
		}

		clear();

		// Print "moving" floor and ceiling
		draw_floor_and_ceiling(0, NUM_ROWS - 1, '/', 2, frame % 2);

		// Update pipe locations and draw them.
		draw_pipe(p1, '|', '=', '=', 0, NUM_ROWS - 1);
		draw_pipe(p2, '|', '=', '=', 0, NUM_ROWS - 1);
		pipe_refresh(&p1);
		pipe_refresh(&p2);

		// Draw Flappy. If Flappy crashed and user wants a restart...
		if(draw_flappy(f)) {
			restart = 1;
			continue; // ...then restart the game.
		}
        if (best_score < score){
            best_score = score; //修复显示分数的BUG
        }


        if(gamemode==1){
            mvprintw(0, SCORE_START_COL - bdigs - sdigs,
                     " Score: %d  Best: %d", score, best_score);
        }else{
            mvprintw(0, SCORE_START_COL - bdigs - sdigs - 10,
                     " Your Score: %d  Opponent: %d", data.point_p1, data.point_p2);
        }
		// Display all the chars for this frame.
		refresh();
		frame++;
	}

	endwin();

	return 0;
}

int main(){
    hscore.easy=0;
    hscore.mid=0;
    hscore.hard=0;

    fscore = fopen("hscore.dat","a+b");
    if(fscore==NULL){
        puts("Failed to load score data");
        exit(0);
    }else{
        rewind(fscore);
        fread(&hscore,sizeof(bestscore),1, fscore);
        if(hscore.easy<0) {
            hscore.easy = 0;
            hscore.mid = 0;
            hscore.hard =0 ;
            //地一次创建数据文件
        }
        if (bdigs == 1 && best_score > 9)
            bdigs++;
        else if(bdigs == 2 && best_score > 99)
            bdigs++;
        fclose(fscore);
    }
    //历史记录读取部分

    pthread_create(&bgm,NULL,playbgm,NULL);
    pthread_create(&maingame,NULL,game,NULL);
    
    while(1){
        if(swooshing==1){
            pthread_create(&wingsound,NULL,playswooshing,NULL);
            swooshing = 0;
        }
        if((FLAPPY_COL == p1.center || FLAPPY_COL == p2.center) && hit == 0){
            pthread_create(&pointsound,NULL,playpoint,NULL);
            usleep(1000000);//防止重复播放
        }
        if (failure == 1){
            switch(dif){
                case 1: hscore.easy = best_score; break;
                case 2: hscore.mid = best_score; break;
                case 3: hscore.hard = best_score; break;
            }
            fscore = fopen("hscore.dat","wb");
            fwrite(&hscore,sizeof(bestscore),1,fscore);
            fclose(fscore);
            exit(0);
        }

        if(gamemode==2){
            up.point=score;
        }

    }
    //以上是loop部分，保证触发对应线程。

    return 0;
}