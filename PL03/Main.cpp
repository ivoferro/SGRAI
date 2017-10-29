#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define RAD(x)          (M_PI*(x)/180)
#define DEG(x)        (180*(x)/M_PI)

#define DEBUG               0
#define PITCH_WIDTH         400
#define PITCH_HEIGHT        200
#define RACKETS_WIDTH       10
#define RACKETS_HEIGHT      50
#define HALF_WAY_LINE		50
#define BALL_SPEED          10
#define RACKETS_VELOCITY    10
#define BALL_SIZE           10
#define MOVEMENT_DELAY      30
#define KEYS_DELAY          20

/* GLOBAL VARIABLES */

typedef struct {
	GLboolean   q, a, p, l;
} Keys;

typedef struct {
	GLboolean   doubleBuffer;
	GLint       movementDelay;
	GLint       keysDelay;
	Keys        keys;
	GLuint      menu_ball_vel_id;
	GLuint      menu_ball_size_id;
	GLuint      menu_rak_vel_id;
	GLuint      menu_rak_size_id;
	GLuint      menu_id;
	GLboolean   menuActive;
	GLboolean   debug;
} State;

typedef struct {
	GLfloat       x, y;
	GLint         score;
} Racket;

typedef struct {
	GLfloat       x, y;
	GLint         velocity;
	GLfloat       direction;
	GLint		  size;
} Ball;

typedef struct {
	Racket      player1, player2;
	Ball        ball;
	GLint       heightRackets;
	GLint       velocityRackets;
	GLint       service;
	GLboolean   pause;
} Model;

State state;
Model model;

void initiates_game()
{
	model.service = (model.service == 2) ? 1 : 2;

	model.player1.x = HALF_WAY_LINE + RACKETS_WIDTH*.5;
	model.player2.x = PITCH_WIDTH - (HALF_WAY_LINE + RACKETS_WIDTH*.5);
	model.player1.y = model.player2.y = PITCH_HEIGHT*.5;
	model.ball.x = PITCH_WIDTH*.5;
	model.ball.y = PITCH_HEIGHT*.5;

	model.ball.direction = RAD(rand() % 120 - 60);
	if (model.service == 2)
		model.ball.direction = M_PI - model.ball.direction;

	if (model.ball.direction<0)
		model.ball.direction += 2 * M_PI;
}

/* OpenGL Environment Initialization */
void Init(void)
{
	srand((unsigned)time(NULL));

	model.ball.velocity = BALL_SPEED;
	model.ball.size = BALL_SIZE;
	model.heightRackets = RACKETS_HEIGHT;
	model.velocityRackets = RACKETS_VELOCITY;
	model.service = 2;
	model.player1.score = model.player2.score = 0;
	model.pause = GL_FALSE;

	state.menuActive = GL_FALSE;
	state.movementDelay = MOVEMENT_DELAY;
	state.keysDelay = KEYS_DELAY;

	state.keys.a = state.keys.q = state.keys.l = state.keys.p = GL_FALSE;

	initiates_game();

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

	glutIgnoreKeyRepeat(GL_TRUE);
}

/**************************************
***    window/drawing callbacks     ***
**************************************/

// CALLBACK TO RESHAPE WINDOW
void Reshape(int width, int height)
{
	// glViewport(botom, left, width, height)
	// defines part of the window to be used by OpenGL
	glViewport(0, 0, (GLint)width, (GLint)height);

	// Projection Matrix
	// Matrix where we define how the world is shown in the window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// gluOrtho2D(left,right,bottom,top); 
	// 2D orthogonal projection, with depth (Z) between -1 and 1
	gluOrtho2D(0, PITCH_WIDTH, 0, PITCH_HEIGHT);

	// Modelview Matrix
	// Matrix where the drew models transformations are made
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void strokeString(char *str, double x, double y, double z, double s)
{
	int i, n;

	n = (int)strlen(str);
	glPushMatrix();
	glColor3d(0.0, 0.0, 0.0);
	glTranslated(x, y, z);
	glScaled(s, s, s);
	for (i = 0; i<n; i++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, (int)str[i]);

	glPopMatrix();
}

void bitmapString(char *str, double x, double y)
{
	int i, n;

	// font can be:
	// GLUT_BITMAP_8_BY_13
	// GLUT_BITMAP_9_BY_15
	// GLUT_BITMAP_TIMES_ROMAN_10
	// GLUT_BITMAP_TIMES_ROMAN_24
	// GLUT_BITMAP_HELVETICA_10
	// GLUT_BITMAP_HELVETICA_12
	// GLUT_BITMAP_HELVETICA_18
	//
	// int glutBitmapWidth  	(	void *font , int character);
	// retrieves the width of a character
	//
	// int glutBitmapLength 	(	void *font , const unsigned char *string );
	// retrieves the width of a string (sum of the width of all characters)

	n = (int)strlen(str);
	glRasterPos2d(x, y);
	for (i = 0; i<n; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)str[i]);
}

void bitmapCenterString(char *str, double x, double y)
{
	int i, n;

	n = (int)strlen(str);
	glRasterPos2d(x - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)str)*0.5, y);
	for (i = 0; i<n; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)str[i]);
}

void draw_pitch(int p1, int p2)
{
	char str[255];

	glLineWidth(3);
	glLineStipple(2, 0xFF00);

	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(PITCH_WIDTH*.5, 0);
	glVertex2f(PITCH_WIDTH*.5, PITCH_HEIGHT);
	glEnd();
	glDisable(GL_LINE_STIPPLE);

	sprintf_s(str, 255, "%d", p1);
	bitmapString(str, PITCH_WIDTH*.4, PITCH_HEIGHT*.9);
	sprintf_s(str, 255, "%d", p2);
	bitmapString(str, PITCH_WIDTH*.6, PITCH_HEIGHT*.9);

	if (state.debug)
	{
		sprintf_s(str, 255, "ang:%.1f*PI\n x:%.1f\n y:%.1f ", model.ball.direction / M_PI, model.ball.x, model.ball.y);
		bitmapCenterString(str, PITCH_WIDTH*.5, PITCH_HEIGHT*.1);
	}
}


void draw_rackets(Racket raq)
{
	glBegin(GL_POLYGON);
	glVertex2f(raq.x - RACKETS_WIDTH*.5, raq.y - model.heightRackets*.5);
	glVertex2f(raq.x + RACKETS_WIDTH*.5, raq.y - model.heightRackets*.5);
	glVertex2f(raq.x + RACKETS_WIDTH*.5, raq.y + model.heightRackets*.5);
	glVertex2f(raq.x - RACKETS_WIDTH*.5, raq.y + model.heightRackets*.5);
	glEnd();
}


void draw_ball(Ball ball)
{
	// we can use the circunference equation 
	// instead of drawing a point, so then
	// we can resize it
	glPointSize(model.ball.size);

	glBegin(GL_POINTS);
	glVertex2f(ball.x, ball.y);
	glEnd();
}


// ... definition of the auxiliar drawing routines ...

// Drawing Callback
void Draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	// ... call of the auxiliar drawing routines ...

	draw_pitch(model.player1.score, model.player2.score);

	glColor3f(1.0f, 0.0f, 0.0f);
	draw_rackets(model.player1);

	glColor3f(0.0f, 0.0f, 1.0f);
	draw_rackets(model.player2);

	glColor3f(1.0f, 1.0f, 0.0f);
	draw_ball(model.ball);

	if (model.pause)
	{
		glColor3f(0.7, 0.7, 0.7);
		bitmapCenterString("The game is paused, use the menu to continue!", PITCH_WIDTH*.5, PITCH_HEIGHT*.5);
	}

	glFlush();
	if (state.doubleBuffer)
		glutSwapBuffers();
}

/*******************************
***   callbacks timer/idle   ***
*******************************/

void Timer(int value)
{
	glutTimerFunc(state.movementDelay, Timer, 0);
	// ... timer actions ... 

	if (state.menuActive || model.pause) // exit if the menu is activated or if the game is paused
		return;

	// moves the ball
	model.ball.x += model.ball.velocity * cos(model.ball.direction);
	model.ball.y += model.ball.velocity * sin(model.ball.direction);

	// tests the score
	if (model.ball.x + model.ball.size < 0)
	{
		model.player2.score++;
		initiates_game();
	}
	if (model.ball.x - model.ball.size > PITCH_WIDTH)
	{
		model.player1.score++;
		initiates_game();
	}

	// tests collision with the wall
	if (model.ball.y + model.ball.size/2 >= PITCH_HEIGHT
		|| model.ball.y - model.ball.size/2 <= 0)
	{
		model.ball.direction = -model.ball.direction;
	}

	// tests collisions the the racket 1
	if (model.ball.x - model.ball.size / 2 <= model.player1.x + RACKETS_WIDTH / 2 &&
		model.ball.x - model.ball.size / 2 >= model.player1.x - RACKETS_WIDTH / 2 &&
		model.ball.y - model.ball.size / 2 <= model.player1.y + model.heightRackets / 2 &&
		model.ball.y - model.ball.size / 2 >= model.player1.y - model.heightRackets / 2 &&
		cos(model.ball.direction) < 0)
	{
		model.ball.direction = M_PI - model.ball.direction;
	}

	// tests collisions the the racket 2
	if (model.ball.x + model.ball.size / 2 <= model.player2.x + RACKETS_WIDTH / 2 &&
		model.ball.x + model.ball.size / 2 >= model.player2.x - RACKETS_WIDTH / 2 &&
		model.ball.y + model.ball.size / 2 <= model.player2.y + model.heightRackets / 2 &&
		model.ball.y + model.ball.size / 2 >= model.player2.y - model.heightRackets / 2 &&
		cos(model.ball.direction) > 0)
	{
		model.ball.direction = M_PI - model.ball.direction;
	}

	// redraws the screen
	glutPostRedisplay();
}


void KeysTimer(int value)
{
	glutTimerFunc(state.keysDelay, KeysTimer, 0);
	// ... keys actions ... 

	if (state.menuActive || model.pause)
		return;

	if (state.keys.a /*&& condition*/)
	{
		model.player1.y -= model.velocityRackets;
		if (model.player1.y < model.heightRackets/2.0)
		{
			model.player1.y = model.heightRackets / 2.0;
		}
	}
	if (state.keys.q /*&& condition*/)
	{
		model.player1.y += model.velocityRackets;
		if (model.player1.y > PITCH_HEIGHT - model.heightRackets / 2.0)
		{
			model.player1.y = PITCH_HEIGHT - model.heightRackets / 2.0;
		}
	}
	if (state.keys.l /*&& condition*/)
	{
		model.player2.y -= model.velocityRackets;
		if (model.player2.y < model.heightRackets / 2.0)
		{
			model.player2.y = model.heightRackets / 2.0;
		}
	}
	if (state.keys.p /*&& condition*/)
	{
		model.player2.y += model.velocityRackets;
		if (model.player2.y > PITCH_HEIGHT - model.heightRackets / 2.0)
		{
			model.player2.y = PITCH_HEIGHT - model.heightRackets / 2.0;
		}
	}

	// there is no need to redraw the screen, the Timer already does it
}

void prints_help(void)
{
	printf("\n\nHELP MENU\n");
	printf("h,H - Help \n");
	printf("q,Q - Moves the player 1 up\n");
	printf("a,A - Moves the player 1 down\n");
	printf("P,p - Moves the player 2 up\n");
	printf("L,l - Moves the player 2 down\n");
	printf("D,d - Debug ON/OFF\n");
	printf("ESC - Exit\n");
}

/*******************************
***    Keyboard Callbacks    ***
*******************************/

// Callback to interact via keyboard (press a key)
void Key(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(1);
		// ... actions about other keys ... 

	case 'h':
	case 'H':
		prints_help();
		break;
	case 'i':
	case 'I':
		initiates_game();
		break;
	case 'Q':
	case 'q': state.keys.q = GL_TRUE;
		break;
	case 'A':
	case 'a': state.keys.a = GL_TRUE;
		break;
	case 'P':
	case 'p': state.keys.p = GL_TRUE;
		break;
	case 'L':
	case 'l': state.keys.l = GL_TRUE;
		break;
	case 'D':
	case 'd': state.debug = !state.debug;
		if (state.menuActive || model.pause)
			glutPostRedisplay();
		printf("DEBUG is %s\n", (state.debug) ? "ON" : "OFF");
		break;

	}

	if (state.debug)
		printf("Pressed the key %c\n", key);

}

// Callback to interact via keyboard (release a key)
void KeyUp(unsigned char key, int x, int y)
{
	switch (key) {
		// ... actions about other keys ... 

	case 'Q':
	case 'q': state.keys.q = GL_FALSE;
		break;
	case 'A':
	case 'a': state.keys.a = GL_FALSE;
		break;
	case 'P':
	case 'p': state.keys.p = GL_FALSE;
		break;
	case 'L':
	case 'l': state.keys.l = GL_FALSE;
		break;

	}

	if (state.debug)
		printf("Released the key %c\n", key);
}

void MenuStatus(int status, int x, int y)
{
	/* status => GLUT_MENU_IN_USE, GLUT_MENU_NOT_IN_USE
	x,y    => coordinates of the pointer when we get into the menu
	*/

	if (status == GLUT_MENU_IN_USE)
	{
		state.menuActive = GL_TRUE;
	}
	else
	{
		state.menuActive = GL_FALSE;

	}

	if (state.debug)
		printf("MenuStatus status:%d coord:%d %d\n", status, x, y);

}

void menu(int opcao)
{

	switch (opcao) {
	case 0:
		exit(1);
		break;
		// ball velocity
	case 1:
		if (model.ball.velocity>2)
			model.ball.velocity -= 2;
		break;
	case 2:
		if (model.ball.velocity<16)
			model.ball.velocity += 2;
		break;
		// ball size
	case 3:
		if (model.ball.size>5)
			model.ball.size--;
		break;
	case 4:
		if (model.ball.size<15)
			model.ball.size++;
		break;
		// racket velocity
	case 5:
		if (model.velocityRackets>10)
			model.velocityRackets -= 5;
		break;
	case 6:
		if (model.velocityRackets<30)
			model.velocityRackets += 5;
		break;
		// racket size
	case 7:
		if (model.heightRackets>30)
			model.heightRackets -= 10;
		break;
	case 8:
		if (model.velocityRackets<100)
			model.heightRackets += 10;
		break;
	case 9:
		model.pause = !model.pause;
		if (model.pause)
		{
			glutChangeToMenuEntry(1, "Resume", 9);
			glutPostRedisplay();
		}
		else
		{
			glutChangeToMenuEntry(1, "Pause", 9);
		}
		break;
	}

}

void create_menu()
{
	// functions to manipulate menus
	// id=glutCreateMenu(menu)  - creates a menu and retrieves the id
	// glutDestroyMenu(id)      - destroys the menu with the given id
	// id=glutGetMenu()         - retrieves the id of the active menu
	// glutSetMenu(id)          - sets the active menu with the one with the given id
	//
	// glutAddMenuEntry("text", value) -  adds an entry to the active menu
	// glutChangeToMenuEntry(item, "new text", new_value) 
	//                                  -  changes the item entry (1,2,3..) of the active menu
	// glutAddSubMenu("text", id)      -  adds the submenu with the given id to the active menu
	// glutChangeToSubMenu(item, "new text", new_id) 
	//                                  -  changes the submeny entry (1,2,3..) of the active menu
	// glutRemoveMenuItem(item)         - deletes the item entry (1,2,3...)
	//
	// glutAttachMenu(button)            - associates the active menu to the given button
	// glutDetachMenu(button)            - deassociates the active menu and reassociates to the button GLUT_RIGHT_BUTTON
	// button = GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON ou GLUT_RIGHT_BUTTON

	/* DEFINES MENUS */
	state.menu_ball_vel_id = glutCreateMenu(menu);
	glutAddMenuEntry("Decrease", 1);
	glutAddMenuEntry("Increase", 2);

	state.menu_ball_size_id = glutCreateMenu(menu);
	glutAddMenuEntry("Decrease", 3);
	glutAddMenuEntry("Increase", 4);

	state.menu_rak_vel_id = glutCreateMenu(menu);
	glutAddMenuEntry("Decrease", 5);
	glutAddMenuEntry("Increase", 6);

	state.menu_rak_size_id = glutCreateMenu(menu);
	glutAddMenuEntry("Decrease", 7);
	glutAddMenuEntry("Increase", 8);

	state.menu_id = glutCreateMenu(menu);
	glutAddMenuEntry("Pause", 9);

	// adds submenus
	glutAddSubMenu("Ball Velocity", state.menu_ball_vel_id);
	glutAddSubMenu("Ball Size", state.menu_ball_size_id);
	glutAddSubMenu("Rackets Velocity", state.menu_rak_vel_id);
	glutAddSubMenu("Rackets Height", state.menu_rak_size_id);

	glutAddMenuEntry("Exit", 0);

	glutAttachMenu(GLUT_RIGHT_BUTTON); // associates the active menu to the right mouse button

}


void main(int argc, char **argv)
{
	state.doubleBuffer = GL_TRUE;

	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(PITCH_WIDTH, PITCH_HEIGHT);
	glutInitDisplayMode(((state.doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE) | GLUT_RGB);
	if (glutCreateWindow("Ping-Pong") == GL_FALSE)
		exit(1);

	Init();

	prints_help();

	// Register GLUT Callbacks

	// Window/Drawing Callbacks
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Draw);

	// Keyboard Callbacks
	glutKeyboardFunc(Key);
	glutKeyboardUpFunc(KeyUp);
	//glutSpecialFunc(SpecialKey);
	//glutSpecialUpFunc(SpecialKeyUp);

	// Timer/Idle Callbacks
	glutTimerFunc(state.movementDelay, Timer, 0);
	glutTimerFunc(state.keysDelay, KeysTimer, 0);

	//Menus
	create_menu();
	glutMenuStatusFunc(MenuStatus);

	// BEGIN...
	glutMainLoop();
}