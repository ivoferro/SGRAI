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
#define DEG(x)          (180*(x)/M_PI)

#define DEBUG        1
#define SIDE_MAX     2
#define SIDE_MIN     0.3
#define SIDE_DELTA   0.1


/* FUNCTIONS SIGNATURES*/

void axes();


/* GLOBAL VARIABLES */

typedef struct {
	GLboolean   doubleBuffer;
	GLint       movementDelay;
	GLboolean   debug;
	GLboolean   translationMovement;      // if the small cubes spin
	GLboolean   rotationMovement;         // if the big cube spins
} State;


typedef struct {
	GLfloat   theta[3];             // 0-Rotation on X; 1-Rotation on Y; 2-Rotation on Z

	GLint     rotatingAxis;         // eixo que está a rodar (mudar com o rato)
	GLfloat   cubeSideWidth;        // the width of the cube side
	GLfloat   deltaRotation;        // angle increment when it spins
	GLboolean translationDirection; // translation direction of the small cubes
	GLfloat   cubeTranslation;      // cube translation
	GLfloat   deltaTranslacao;      // translation increment
	GLboolean rotationDirection;    // rotation direction of the small cubes
	GLfloat   thetaCube;            // rotation of the small cubes
	GLfloat   cubesSpeed;           // the speed of the small cubes (increment)
} Model;

State state;
Model model;


/* OpenGL Environment Initialization */
void initiate_model()
{
	state.movementDelay = 50;
	state.translationMovement = GL_FALSE;
	state.rotationMovement = GL_FALSE;
	state.debug = GL_FALSE;

	model.theta[0] = 45;
	model.theta[1] = 45;
	model.theta[2] = 90;
	model.rotatingAxis = 0;  // X axis;
	model.cubeSideWidth = 1;
	model.cubesSpeed = 0.1;
}

void Init(void)
{
	initiate_model();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_DEPTH_TEST);
}

/**************************************
***    window/drawing callbacks     ***
**************************************/

// CALLBACK TO RESHAPE WINDOW
void Reshape(int width, int height)
{
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (width < height)
		glOrtho(-5, 5, -5 * (GLdouble)height / width, 5 * (GLdouble)height / width, -10, 10);
	else
		glOrtho(-5 * (GLdouble)width / height, 5 * (GLdouble)width / height, -5, 5, -10, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


// ... definition of the auxiliar drawing routines ...

void drawPolygon(GLfloat a[], GLfloat b[], GLfloat c[], GLfloat  d[], GLfloat cor[])
{
	/* draw a polygon via list of vertices */

	glBegin(GL_POLYGON);
	glColor3fv(cor);
	glVertex3fv(a);
	glVertex3fv(b);
	glVertex3fv(c);
	glVertex3fv(d);
	glEnd();
}

void cube()
{
	GLfloat vertices[][3] = { 
		{ -0.5, -0.5, -0.5 },
		{ 0.5, -0.5, -0.5 },
		{ 0.5, 0.5, -0.5 },
		{ -0.5, 0.5, -0.5 },
		{ -0.5, -0.5, 0.5},
		{ 0.5, -0.5, 0.5},
		{ 0.5, 0.5, 0.5 },
		{ -0.5, 0.5, 0.5} };

	GLfloat cores[][3] = { { 0.0,1.0,1.0 },
		{ 1.0,0.0,0.0 },
		{ 1.0,1.0,0.0 },
		{ 0.0,1.0,0.0 },
		{ 1.0,0.0,1.0 },
		{ 0.0,0.0,1.0 },
		{ 1.0,1.0,1.0 } };

	drawPolygon(vertices[1], vertices[0], vertices[3], vertices[2], cores[0]);
	drawPolygon(vertices[1], vertices[2], vertices[6], vertices[5], cores[1]);
	drawPolygon(vertices[4], vertices[5], vertices[6], vertices[7], cores[2]);
	drawPolygon(vertices[2], vertices[3], vertices[7], vertices[6], cores[3]);
	drawPolygon(vertices[4], vertices[7], vertices[3], vertices[0], cores[4]);
	drawPolygon(vertices[1], vertices[5], vertices[4], vertices[0], cores[5]);
}


// Drawing Callback
void Draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// ... call of the auxiliar drawing routines ...

	glPushMatrix();
	glRotatef(model.theta[0], 1, 0, 0);
	glRotatef(model.theta[1], 0, 1, 0);
	glRotatef(model.theta[2], 0, 0, 1);
	glScalef(model.cubeSideWidth, model.cubeSideWidth, model.cubeSideWidth);
	cube();
	axes();
	glPopMatrix();

	glPushMatrix();
	glRotatef(model.theta[0], 1, 0, 0);
	glRotatef(model.theta[1], 0, 1, 0);
	glRotatef(model.theta[2], 0, 0, 1);
	glTranslatef(model.cubeTranslation, 0, 0);
	glScalef(0.3, 0.3, 0.3);
	cube();
	glPopMatrix();

	glPushMatrix();
	glRotatef(model.theta[0], 1, 0, 0);
	glRotatef(model.theta[1], 0, 1, 0);
	glRotatef(model.theta[2], 0, 0, 1);
	glTranslatef(0, model.cubeTranslation, 0);
	glScalef(0.3, 0.3, 0.3);
	cube();
	glPopMatrix();

	glPushMatrix();
	glRotatef(model.theta[0], 1, 0, 0);
	glRotatef(model.theta[1], 0, 1, 0);
	glRotatef(model.theta[2], 0, 0, 1);
	glTranslatef(0, 0, model.cubeTranslation);
	glScalef(0.3, 0.3, 0.3);
	cube();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.5, -2.5, 0);
	glRotatef(model.theta[0], 1, 0, 0);
	glRotatef(model.theta[1], 0, 1, 0);
	glRotatef(model.theta[2], 0, 0, 1);
	glScalef(0.2, 0.2, 0.2);
	axes();
	glPopMatrix();

	if (state.doubleBuffer)
		glutSwapBuffers();
	else
		glFlush();
}

/*******************************
***      Timer Callbacks     ***
*******************************/

/* Timer Callback */
void Timer(int value)
{

	glutTimerFunc(state.movementDelay, Timer, 0);

	/* timer actions, do not make transformations here, only change the variables values*/

	//change model.theta[] using the variable model.rotatingAxis as index
	if (state.rotationMovement)
	{
		model.theta[model.rotatingAxis] += 5;
	}

	// animate small cubes
	if (model.cubesSpeed > 0 && model.cubeTranslation < 2)
	{
		model.cubesSpeed = 0.1;
	}
	else if (model.cubesSpeed > 0 && model.cubeTranslation > 2)
	{
		model.cubesSpeed = -0.1;
	}
	else if (model.cubesSpeed < 0 && model.cubeTranslation > 0)
	{
		model.cubesSpeed = -0.1;
	}
	else
	{
		model.cubesSpeed = 0.1;
	}
	model.cubeTranslation += model.cubesSpeed;

	// redraws the screen
	glutPostRedisplay();
}


/*******************************
***    Keyboard Callbacks    ***
*******************************/

void prints_help(void)
{
	printf("\n\nHELP MENU\n");
	printf("h,H - Help \n");
	printf("F1  - Restart \n");
	printf("F2  - Polygon Fill \n");
	printf("F3  - Polygon Line \n");
	printf("F4  - Polygon Point \n");
	printf("+   - Increase cubes size\n");
	printf("-   - Decrease cubes size\n");
	printf("i,I - Reset variables\n");
	printf("p,p - Initiates/Stops small cubes movement\n");
	printf("D,d - Debug ON/OFF\n");
	printf("ESC - Exit\n");
	printf("Mouse keys to start/stop the cube rotation and change direction\n");

}


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
	case '+':
		if (model.cubeSideWidth<SIDE_MAX)
		{
			model.cubeSideWidth += SIDE_DELTA;
			glutPostRedisplay();
		}
		break;

	case '-':
		if (model.cubeSideWidth>SIDE_MIN)
		{
			model.cubeSideWidth -= SIDE_DELTA;
			glutPostRedisplay();
		}
		break;

	case 'i':
	case 'I':
		initiate_model();
		glutPostRedisplay();
		break;
	case 'p':
	case 'P':
		state.translationMovement = !state.translationMovement;
		break;
	case 'D':
	case 'd': 
		state.debug = !state.debug;
		printf("DEBUG is %s\n", (state.debug) ? "ON" : "OFF");
		break;

	}

	if (DEBUG)
		printf("You pressed the key %c\n", key);

}

// Callback to interact via keyboard (release a key)
void KeyUp(unsigned char key, int x, int y)
{

	if (DEBUG)
		printf("You released the key %c\n", key);
}

// Callback to interact via specials keys (press a key)
void SpecialKey(int key, int x, int y)
{
	/* ... actions about other special keys ...
	GLUT_KEY_F1 ... GLUT_KEY_F12
	GLUT_KEY_UP
	GLUT_KEY_DOWN
	GLUT_KEY_LEFT
	GLUT_KEY_RIGHT
	GLUT_KEY_PAGE_UP
	GLUT_KEY_PAGE_DOWN
	GLUT_KEY_HOME
	GLUT_KEY_END
	GLUT_KEY_INSERT
	*/

	switch (key) {

		/* redraw the scren */
		//glutPostRedisplay();
	case GLUT_KEY_F1:
		initiate_model();
		glutPostRedisplay();
		break;

	}


	if (DEBUG)
		printf("You pressed the special key %d\n", key);
}

// Callback to interact via special keys (release a key)
void SpecialKeyUp(int key, int x, int y)
{

	if (DEBUG)
		printf("You released the special key %d\n", key);

}

/*******************************
***     Mouse Callbacks      ***
*******************************/

void MouseMotion(int x, int y)
{
	// x,y => coordinates of the mouse pointer when clicking on a key

	if (DEBUG)
		printf("Mouse Motion %d %d\n", x, y);

}

void MousePassiveMotion(int x, int y)
{
	// x,y => coordinates of the mouse pointer without clicking on a key

	if (DEBUG)
		printf("Mouse Passive Motion %d %d\n", x, y);

}

void Mouse(int button, int mouse_state, int x, int y)
{
	// button      => GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
	// mouse_state => GLUT_UP, GLUT_DOWN
	// x, y        => coordinates of the mouse pointer when clicking on a key

	// change the axis to rotate (variable model.rotatingAxis)
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (mouse_state == GLUT_DOWN)
		{
			if (model.rotatingAxis == 0)
			{
				state.rotationMovement = !state.rotationMovement;
			}
			else
			{
				model.rotatingAxis = 0;
			}
		}
		break;
	case GLUT_MIDDLE_BUTTON:
		if (mouse_state == GLUT_DOWN)
		{
			if (model.rotatingAxis == 1)
			{
				state.rotationMovement = !state.rotationMovement;
			}
			else
			{
				model.rotatingAxis = 1;
			}
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (mouse_state == GLUT_DOWN)
		{
			if (model.rotatingAxis == 2)
			{
				state.rotationMovement = !state.rotationMovement;
			}
			else
			{
				model.rotatingAxis = 2;
			}
		}
		break;
	}
	if (DEBUG)
		printf("Mouse button:%d state:%d coord:%d %d\n", button, state, x, y);
}

int main(int argc, char **argv)
{
	char str[] = " makefile MAKEFILE Makefile ";

	state.doubleBuffer = 1;
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(400, 400);
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
	//glutKeyboardUpFunc(KeyUp);
	glutSpecialFunc(SpecialKey);
	//glutSpecialUpFunc(SpecialKeyUp);

	/* mouse callbacks */
	//glutPassiveMotionFunc(MousePassiveMotion);
	//glutMotionFunc(MouseMotion);
	glutMouseFunc(Mouse);

	/* callbacks timer/idle */
	glutTimerFunc(state.movementDelay, Timer, 0);
	//glutIdleFunc(Idle);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/* COMECAR... */
	glutMainLoop();
	return 0;
}


void axes() {

	GLfloat origin[3] = { 0,0,0 };

	GLfloat x[3] = { 2,0,0 };
	GLfloat y[3] = { 0,2,0 };
	GLfloat z[3] = { 0,0,2 };

	GLfloat colors[][3] = { 
		{ 0.0,0.5,1.0 },
		{ 0.5,0.0,0.0 },
		{ 0.5,1.0,0.0 } };

	glBegin(GL_LINES);
		glColor3fv(colors[0]);
		glVertex3fv(origin);
		glVertex3fv(x);

		glColor3fv(colors[1]);
		glVertex3fv(origin);
		glVertex3fv(y);

		glColor3fv(colors[2]);
		glVertex3fv(origin);
		glVertex3fv(z);
	glEnd();
}