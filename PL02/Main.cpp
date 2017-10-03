#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glut.h>

#pragma warning (disable:4996)

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define rtd(x)   (180*(x)/M_PI)
#define dtr(x)   (M_PI*(x)/180)

#define DEBUG 1

/* GLOBAL VARIABLES */

typedef struct {
	GLboolean   double_buffer;
	GLint       delay;
} State;

typedef struct {
	GLint       hour, min, sec;
} Hours;

typedef struct {
	GLint       num_sides;
	GLfloat     radius;
	GLfloat     side_size;
	Hours       hour;
} Model;


State state;
Model model;

void polygon(GLint n, GLfloat x0, GLfloat y0, GLfloat r);
void marks(GLfloat x0, GLfloat y0, GLfloat r0, GLfloat r1, GLfloat r2);
void seconds(GLfloat x0, GLfloat y0, GLfloat r0);
void minutes(GLfloat x0, GLfloat y0, GLfloat r0);
void hour(GLfloat x0, GLfloat y0, GLfloat r0);

/* Initialization of the OPENGL environment */
void Init(void)
{

	struct tm *current_time;
	time_t timer = time(0);

	// delay for the timer
	state.delay = 1000;

	model.side_size = 1;
	model.num_sides = 5;
	model.radius = 0.75;


	// Reads the system time
	current_time = localtime(&timer);
	model.hour.hour = current_time->tm_hour;
	model.hour.min = current_time->tm_min;
	model.hour.sec = current_time->tm_sec;

	glClearColor(0.3, 0.3, 0.3, 0.0);

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

}

/**************************************
***  callbacks from window/drawing  ***
**************************************/

// CALLBACK TO REDIMENSION THE WINDOW

void Reshape(int width, int height)
{
	GLint size;
	GLfloat ratio = (GLfloat)width / height;
	GLfloat ratio1 = (GLfloat)height / width;

	if (width < height)
		size = width;
	else
		size = height;

	// glViewport(botom, left, width, height)
	// define the part of the window to be used by OpenGL

	//glViewport(0, 0, (GLint) size, (GLint) size);
	glViewport(0, 0, width, height);


	// The projection matrix
	// Matriz onde se define como o mundo e apresentado na janela
	// Matrix where you define how the world is shown in the window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// gluOrtho2D(left,right,bottom,top); 
	// 2D orthogonal projection, with a depth of (Z) between -1 and 1
	if (width < height)
		gluOrtho2D(-1, 1, -1 * ratio1, 1 * ratio1);
	else
		gluOrtho2D(-1 * ratio, 1 * ratio, -1, 1);

	// Matrix Modelview
	// Matrix where the designed models transforms are made
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


// ... definition of the auxiliar drawing routines ...

// Draw callback

void Draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	// ... call of the auxiliar drawing routines ...

	polygon(30, 0.0, 0.0, 0.9);
	marks(0.0, 0.0, 0.6, 0.7, 0.8);

	seconds(0.0, 0.0, 0.5);
	minutes(0.0, 0.0, 0.4);
	hour(0.0, 0.0, 0.3);

	//draw a square
	/*glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(model.side_size / 2, model.side_size / 2);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(-model.side_size / 2, model.side_size / 2);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-model.side_size / 2, -model.side_size / 2);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex2f(model.side_size / 2, -model.side_size / 2);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(1.0, 1.0, 1.0);
	glVertex2f(-1.0, -1.0);
	glVertex2f(1.0, -1.0);
	glVertex2f(1.0, 1.0);
	glVertex2f(-1.0, 1.0);
	glEnd();*/

	glFlush();
	if (state.double_buffer)
		glutSwapBuffers();
}

/*******************************
***   callbacks timer/idle   ***
*******************************/

// Callback Idle

void Idle(void)
{

	// actions to take when the glut is idle

	// redraw the screen
	// glutPostRedisplay();
}

// Callback of the timer (don't put drawing instructions here...)

void Timer(int value)
{
	glutTimerFunc(state.delay, Timer, 0);
	// ... timer actions ... 

	model.hour.sec++;

	if (model.hour.sec >= 60)
	{
		model.hour.sec = 0;
		model.hour.min++;
	}

	if (model.hour.min >= 60)
	{
		model.hour.min = 0;
		model.hour.hour++;
	}

	// redraw the screen (calls the drawing callback)
	glutPostRedisplay();
}


void print_help(void)
{
	printf("\n\nDrawing of a polygon\n");
	printf("h,H - Help \n");
	printf("+   - Increase the side size\n");
	printf("-   - Decrease the side size\n");
	printf("ESC - Quit\n");
}

/*******************************
***    keyboard callbacks    ***
*******************************/

// Callback to interact with the keyboard (press a key)

void Key(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:  // Escape key
		exit(1);
		// ... other keys actions ... 

	case 'h':
	case 'H':
		print_help();
		break;
	case '+':
		if (model.side_size < 1.8)
		{
			model.side_size += 0.05;
			glutPostRedisplay(); // redraw the screen
		}
		break;
	case '-':
		if (model.side_size > 0.2)
		{
			model.side_size -= 0.05;
			glutPostRedisplay(); // redraw the screen
		}
		break;

	}

	if (DEBUG)
		printf("You pressed the key %c\n", key);

}

// Callback to interact with the keyboard (release the key)

void KeyUp(unsigned char key, int x, int y)
{

	if (DEBUG)
		printf("You released the key %c\n", key);
}

// Callback to interact with special keys (press the key)

void SpecialKey(int key, int x, int y)
{
	// ... actions about other special keys ... 
	//    GLUT_KEY_F1 ... GLUT_KEY_F12
	//    GLUT_KEY_UP
	//    GLUT_KEY_DOWN
	//    GLUT_KEY_LEFT
	//    GLUT_KEY_RIGHT
	//    GLUT_KEY_PAGE_UP
	//    GLUT_KEY_PAGE_DOWN
	//    GLUT_KEY_HOME
	//    GLUT_KEY_END
	//    GLUT_KEY_INSERT 

	switch (key) {

		// redraw the screen
		//glutPostRedisplay();
	}


	if (DEBUG)
		printf("You pressed the special key %d\n", key);
}

// Callback to interact with special keys (release the key)

void SpecialKeyUp(int key, int x, int y)
{

	if (DEBUG)
		printf("You released the special key %d\n", key);

}


void main(int argc, char **argv)
{
	state.double_buffer = GL_TRUE;  // set to GL_TRUE to turn on the Double Buffer

	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(300, 300);
	glutInitDisplayMode(((state.double_buffer) ? GLUT_DOUBLE : GLUT_SINGLE) | GLUT_RGB);
	if (glutCreateWindow("Example") == GL_FALSE)
		exit(1);

	Init();

	print_help();

	// Register the GLUT callbacks

	// window/draw callbacks
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Draw);

	// keyboard callbacks
	glutKeyboardFunc(Key);
	//glutKeyboardUpFunc(KeyUp);
	//glutSpecialFunc(SpecialKey);
	//glutSpecialUpFunc(SpecialKeyUp);

	// callbacks timer/idle
	glutTimerFunc(state.delay, Timer, 0);
	//glutIdleFunc(Idle);


	// BEGIN...
	glutMainLoop();
}

void polygon(GLint n, GLfloat x0, GLfloat y0, GLfloat r) {
	GLfloat t, x, y;
	t = 0.0;

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	for (size_t i = 0; i < n; i++)
	{
		x = r * cos(t) + x0;
		y = r * sin(t) + y0;
		glVertex2f(x, y);
		t += 2 * M_PI / n;
	}
	glEnd();
}

void marks(GLfloat x0, GLfloat y0, GLfloat r0, GLfloat r1, GLfloat r2) {
	GLfloat t, x, y;
	t = 0.0;

	glBegin(GL_LINES);

	glColor3f(0.0, 0.0, 1.0);
	for (size_t i = 0; i < 12; i++)
	{
		x = r1 * cos(t) + x0;
		y = r1 * sin(t) + y0;

		glVertex2f(x, y);

		x = r2 * cos(t) + x0;
		y = r2 * sin(t) + y0;
		glVertex2f(x, y);

		t += 2 * M_PI / 12;
	}

	glColor3f(0.0, 1.0, 1.0);
	for (size_t i = 0; i < 4; i++)
	{
		x = r0 * cos(t) + x0;
		y = r0 * sin(t) + y0;

		glVertex2f(x, y);

		x = r2 * cos(t) + x0;
		y = r2 * sin(t) + y0;
		glVertex2f(x, y);

		t += 2 * M_PI / 4;
	}

	glEnd();
}

void seconds(GLfloat x0, GLfloat y0, GLfloat r0) {
	GLfloat t, x, y;
	glLineWidth(1.0);
	glColor3f(0.0, 1.0, 0.0);

	t = -2 * M_PI * model.hour.sec / 60 + M_PI / 2;

	glBegin(GL_LINES);
	glVertex2f(x0, y0);

	x = r0 * cos(t) + x0;
	y = r0 * sin(t) + y0;
	glVertex2f(x, y);
	glEnd();
}

void minutes(GLfloat x0, GLfloat y0, GLfloat r0) {
	GLfloat t, x, y;
	glLineWidth(1.0);
	glColor3f(1.0, 0.0, 0.0);

	t = -2 * M_PI * model.hour.min / 60 + M_PI / 2;

	glBegin(GL_LINES);
	glVertex2f(x0, y0);

	x = r0 * cos(t) + x0;
	y = r0 * sin(t) + y0;
	glVertex2f(x, y);
	glEnd();
}

void hour(GLfloat x0, GLfloat y0, GLfloat r0) {
	GLfloat t, x, y;
	glLineWidth(1.0);
	glColor3f(0.8, 0.4, 1.0);

	t = -2 * M_PI * model.hour.hour / 12 + M_PI / 2;

	glBegin(GL_LINES);
	glVertex2f(x0, y0);

	x = r0 * cos(t) + x0;
	y = r0 * sin(t) + y0;
	glVertex2f(x, y);
	glEnd();
}