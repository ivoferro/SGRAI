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

#define DEBUG               1

#define DELAY_MOVIMENTO     20
#define RAIO_ROTACAO        20

#define LARGURA_BASE        4
#define COMPRIMENTO_BASE    7
#define ALTURA_BASE         1

#define LARGURA_TORRE       2
#define COMPRIMENTO_TORRE   2
#define ALTURA_TORRE        0.5

#define COMPRIMENTO_CANHAO  4
#define RAIO_CANHAO         0.2

/* GLOBAL VARIABLES */

typedef struct {
	GLboolean   q, a, z, x, up, down, left, right;
} Keys;

typedef struct {
	GLfloat    x, y, z;
} Pos;

typedef struct {
	Pos      eye, center, up;
	GLfloat  fov;
} Camera;


typedef struct {
	GLboolean   doubleBuffer;
	GLint       movementDelay;
	Keys        keys;
	GLuint      menu_id;
	GLboolean   menuActive;
	Camera      camera;
	GLboolean   debug;
	GLboolean   ortho;
} State;

typedef struct {
	GLfloat       x, y;
	GLint         score;
} Racket;

typedef struct {
	GLfloat     x, y;
	GLfloat     velocity;
	GLfloat     direction;
	GLfloat     directionWheels;
	GLfloat     angTower;
	GLfloat     angCannon;
} Tank;

typedef struct {
	Tank        tank;
	GLboolean   pause;
} Model;

State state;
Model model;

void initiate_model()
{
	model.tank.x = 20;
	model.tank.y = 0;
	model.tank.velocity = 0;
	model.tank.direction = 0;
	model.tank.directionWheels = 0;
	model.tank.angTower = 0;
	model.tank.angCannon = 0;
}

/* OpenGL Environment Initialization */
void Init(void)
{
	srand((unsigned)time(NULL));

	model.pause = GL_FALSE;

	state.debug = DEBUG;
	state.menuActive = GL_FALSE;
	state.movementDelay = DELAY_MOVIMENTO;
	state.camera.eye.x = 40;
	state.camera.eye.y = 40;
	state.camera.eye.z = 40;
	state.camera.center.x = 0;
	state.camera.center.y = 0;
	state.camera.center.z = 0;
	state.camera.up.x = 0;
	state.camera.up.y = 0;
	state.camera.up.z = 1;
	state.ortho = GL_TRUE;
	state.camera.fov = 60;

	state.keys.a = state.keys.q = state.keys.z = state.keys.x = \
		state.keys.up = state.keys.down = state.keys.left = state.keys.right = GL_FALSE;

	initiate_model();

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	//glutIgnoreKeyRepeat(GL_TRUE);
}

/**************************************
***    window/drawing callbacks     ***
**************************************/

// CALLBACK TO RESHAPE WINDOW
void Reshape(int width, int height)
{
	// glViewport(botom, left, width, height)
	// defines the part of the window to be used by OpenGL

	glViewport(0, 0, (GLint)width, (GLint)height);


	// Projection Matrix
	// Matrix where we define how the world is shown in the window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// gluOrtho2D(left,right,bottom,top); 
	// 2D orthogonal projection, with depth (Z) between -1 and 1
	if (state.debug)
		printf("Reshape %s\n", (state.ortho) ? "ORTHO" : "PERSPECTIVE");

	if (state.ortho)
	{
		if (width < height)
			glOrtho(-20, 20, -20 * (GLdouble)height / width, 20 * (GLdouble)height / width, -100, 100);
		else
			glOrtho(-20 * (GLdouble)width / height, 20 * (GLdouble)width / height, -20, 20, -100, 100);
	}
	else
		gluPerspective(state.camera.fov, (GLfloat)width / height, 1, 100);

	// Modelview Matrix
	// Matrix where the drew models transformations are made
	glMatrixMode(GL_MODELVIEW);
}


void drawPolygon(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat  d[], GLfloat cor[])
{

	glBegin(GL_POLYGON);
	glColor3fv(cor);
	glVertex3fv(a);
	glVertex3fv(b);
	glVertex3fv(c);
	glVertex3fv(d);
	glEnd();
}


void drawCube()
{
	GLfloat vertices[][3] = { { -0.5,-0.5,-0.5 },
	{ 0.5,-0.5,-0.5 },
	{ 0.5,0.5,-0.5 },
	{ -0.5,0.5,-0.5 },
	{ -0.5,-0.5,0.5 },
	{ 0.5,-0.5,0.5 },
	{ 0.5,0.5,0.5 },
	{ -0.5,0.5,0.5 } };

	GLfloat colors[][3] = { { 0.0,1.0,1.0 },
	{ 1.0,0.0,0.0 },
	{ 1.0,1.0,0.0 },
	{ 0.0,1.0,0.0 },
	{ 1.0,0.0,1.0 },
	{ 0.0,0.0,1.0 },
	{ 1.0,1.0,1.0 } };

	drawPolygon(vertices[1], vertices[0], vertices[3], vertices[2], colors[0]);
	drawPolygon(vertices[2], vertices[3], vertices[7], vertices[6], colors[1]);
	drawPolygon(vertices[3], vertices[0], vertices[4], vertices[7], colors[2]);
	drawPolygon(vertices[6], vertices[5], vertices[1], vertices[2], colors[3]);
	drawPolygon(vertices[4], vertices[5], vertices[6], vertices[7], colors[4]);
	drawPolygon(vertices[5], vertices[4], vertices[0], vertices[1], colors[5]);
}


void strokeString(char *str, double x, double y, double z, double s)
{
	int i, n;

	n = strlen(str);
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

	n = strlen(str);
	glRasterPos2d(x, y);
	for (i = 0; i<n; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)str[i]);
}

void bitmapCenterString(char *str, double x, double y)
{
	int i, n;

	n = strlen(str);
	glRasterPos2d(x - glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)str)*0.5, y);
	for (i = 0; i<n; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)str[i]);
}

void wheel() {
	GLint slices = 12, stacks = 8;
	GLUquadricObj* qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluCylinder(qobj, 0.5, 0.5, 0.5, slices, stacks);
	gluDeleteQuadric(qobj);
}

void drawWheel() {
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	wheel();
	glPopMatrix();
}

// ... definition of the auxiliar drawing routines ...

void drawTank(Tank t)
{
	//glRotatef(model.tank.direction, 0, 0, 1);
	glTranslatef(RAIO_ROTACAO, 0, 0.5);

	// Base
	glTranslatef(0, 0, ALTURA_BASE * 0.5 );
	glPushMatrix();
	glScalef(LARGURA_BASE, COMPRIMENTO_BASE, ALTURA_BASE);
	drawCube();
	glPopMatrix();

	// Wheels
	glPushMatrix();
	glTranslatef(LARGURA_BASE/2, COMPRIMENTO_BASE/2, -0.5);
	drawWheel();
	glTranslatef(0, -COMPRIMENTO_BASE+1, 0);
	drawWheel();
	glTranslatef(-LARGURA_BASE-0.5, 0, 0);
	drawWheel();
	glTranslatef(0, COMPRIMENTO_BASE-1, 0);
	drawWheel();
	glPopMatrix();

	// Tower
	glTranslatef(0, 0, ALTURA_BASE * 0.5 + ALTURA_TORRE * 0.5);
	glRotatef(t.angTower, 0, 0, 1);
	glPushMatrix();
	glScalef(LARGURA_TORRE, COMPRIMENTO_TORRE, ALTURA_TORRE);
	glRotatef(180, 0, 1, 0);
	drawCube();
	glPopMatrix();

	// Cannon
	glTranslatef(0, COMPRIMENTO_TORRE * 0.5, 0);
	glRotatef(t.angCannon, 1, 0, 0);
	glTranslatef(0, COMPRIMENTO_CANHAO * 0.5, 0);
	glPushMatrix();
	glScalef(RAIO_CANHAO * 2, COMPRIMENTO_CANHAO, RAIO_CANHAO * 2);
	glRotatef(270, 1, 0, 0);
	drawCube();
	glPopMatrix();

	//...  
}

void drawFloor(GLfloat dimension)
{

	glBegin(GL_POLYGON);
	glVertex3f(dimension, dimension, 0);
	glVertex3f(-dimension, dimension, 0);
	glVertex3f(-dimension, -dimension, 0);
	glVertex3f(dimension, -dimension, 0);
	glEnd();

}

// Drawing Callback
void Draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(state.camera.eye.x, state.camera.eye.y, state.camera.eye.z, \
		state.camera.center.x, state.camera.center.y, state.camera.center.z, \
		state.camera.up.x, state.camera.up.y, state.camera.up.z);

	// ... call of the auxiliar drawing routines ...


	glColor3f(0.5f, 0.5f, 0.5f);
	drawFloor(RAIO_ROTACAO + 5);

	glPushMatrix();
	//....    
	drawTank(model.tank);

	glPopMatrix();

	glFlush();
	if (state.doubleBuffer)
		glutSwapBuffers();
}

/*******************************
***   timer/idle callbacks   ***
*******************************/

void Timer(int value)
{
	glutTimerFunc(state.movementDelay, Timer, 0);
	// ... timer actions ... 

	if (state.keys.z)
	{
		model.tank.angTower += 2;
	}
	if (state.keys.x)
	{
		model.tank.angTower -= 2;
	}
	if (state.keys.q && model.tank.angCannon < 45)
	{
		model.tank.angCannon += 2;
	}
	if (state.keys.a && model.tank.angCannon > -2)
	{
		model.tank.angCannon -= 2;
	}

	if (!model.pause)
	{
		model.tank.direction += 1;
	}

	if (state.menuActive || model.pause)  // exit if the menu is activated or if the game is paused
		return;

	// redraws the screen
	glutPostRedisplay();
}



void prints_help(void)
{
	printf("\n\nHELP MENU\n");
	printf("h,H - Help \n");
	printf("z,Z - Spins tower to the left\n");
	printf("x,X - Spins tower to the right\n");
	printf("q,Q - Lift up cannon\n");
	printf("a,A - Lift down cannon\n");
	printf("i,I - Resets model\n");
	printf("o,O - Switches between orthographic and perspection projections\n");
	printf("f,F - Polygon Fill \n");
	printf("l,L - Polygon Line \n");
	printf("p,P - Polygon Point \n");
	printf("s,S - Initiates/stops movement\n");
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
		initiate_model();
		break;
	case 'o':
	case 'O':
		state.ortho = !state.ortho;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;
	case 'Q':
	case 'q': state.keys.q = GL_TRUE;
		break;
	case 'A':
	case 'a': state.keys.a = GL_TRUE;
		break;
	case 'Z':
	case 'z': state.keys.z = GL_TRUE;
		break;
	case 'X':
	case 'x': state.keys.x = GL_TRUE;
		break;
	case 'D':
	case 'd': state.debug = !state.debug;
		if (state.menuActive || model.pause)
			glutPostRedisplay();
		printf("DEBUG is %s\n", (state.debug) ? "ON" : "OFF");
		break;
	case 'f':
	case 'F':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'p':
	case 'P':
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case 'l':
	case 'L':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 's':
	case 'S':
		model.pause = !model.pause;
		break;


	}

	if (state.debug)
		printf("The key %c was pressed\n", key);

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
	case 'Z':
	case 'z': state.keys.z = GL_FALSE;
		break;
	case 'X':
	case 'x': state.keys.x = GL_FALSE;
		break;

	}

	if (state.debug)
		printf("The key %c was released\n", key);
}


// Callback to interact via special keys (press a key)
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

		// redraws the screen
		//glutPostRedisplay();
	}


	if (state.debug)
		printf("The special key %d was pressed\n", key);
}


// Callback to interact via special keys (release a key)
void SpecialKeyUp(int key, int x, int y)
{
	switch (key) {
	}
	if (state.debug)
		printf("The special key %d was released\n", key);

}

int main(int argc, char **argv)
{
	char str[] = " makefile MAKEFILE Makefile ";
	state.doubleBuffer = 1;

	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(((state.doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE) | GLUT_RGB | GLUT_DEPTH);
	if (glutCreateWindow("Example") == GL_FALSE)
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
	glutSpecialFunc(SpecialKey);
	glutSpecialUpFunc(SpecialKeyUp);

	// timer/idle callbacks
	glutTimerFunc(state.movementDelay, Timer, 0);

	// BEGIN...
	glutMainLoop();
	return 0;
}