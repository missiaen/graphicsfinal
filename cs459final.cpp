// Final Project
// CS459
// By Matthew Buchanan, Philip Kocol, and Garrett Missiaen
// readOFF code taken from Lab 8

#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h> 
#include <string.h>
#include <cstdlib>//for random numbers
#include <string.h>
#include <memory.h>
#include <iostream>

#define PI 3.14159265 

static float alpha = 0.0;
static float beta = PI / 6.0;
static GLdouble cpos[3];
static GLint angleView = 30.0;
bool flag = true, pause = false;//used for color change

static GLfloat lpos[] = { -1.0, 2.0, 1.0, 1.0 };
static GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat blue[] = { 0.0, 0.0, 1.0, 1.0 };
static GLfloat transparent[] = { 1.0, 1.0, 1.0, 0.40 };

static GLint numVertices, numPolygons, numEdges;
static GLfloat **vdata;   //array for vertex data
static GLuint **pdata;    //array for polygon data (vertex indices)
static GLuint *psize;     //array for polygon size
static GLfloat **normals; //array for polygon normals
static GLfloat **cdata;//array for polygon color

bool fullscreen = false;
bool mouseDown = false;

float currentframe[3][3] = {
	{ 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0 },
	{ 1.0, 1.0, 1.0 }
};
int reflect = 1;

int refreshTimer = 15; //time between refreshes in milliseconds
int numIntPoints = 200; //number of points to interpolate
int currentIntPoint = 0;
int numFrames = 2;
int currIndex = 0;
int nextIndex = 1;

#define maxframes 20

//1st index frames number
//2nd index translation, rotation, scale
//3rd index x, y, z
float frames[maxframes][3][3]

=
{
	{ { -1,1,-1 },{ -100,-100,-100 },{ 0.9,0.9,0.9 } },
	{ { 1,2,0 },{ 300,200,100 },{ 1.0,2,2 } }
}

;
float zoom = 15.0;


bool both = false;

#define min(x,y) ((x)<(y) ? (x) : (y))
#define max(x,y) ((x)>(y) ? (x) : (y))


void writemessage()
{
	printf("\n\
	Final Project by Matthew Buchanan, Philip Kocol, and Garrett Missiaen\n\
	Allows defining keyframs and animates between them\n\
	a ------------------------- add random keyframe\n\
	b ------------------------- add keyframe\n\
	l ------------------------- change color\n\
	-, + ---------------------- change zoom\n\
	c ------------------------- change off files\n\
	x, X, y, Y, z, Z ---------- change light source position\n\
	spacebar ------------------ pause/unpause\n\
	F1 ------------------------ fullscreen\n\
	ESC ----------------------- exit\n\
	\n");
}


void readOFF()//method to read .off format data. Borrowed partially form Dr. Zeyun Yu homework
			  //modified further to read polygons of any size
			  //nonconvex and nonflat polygons may not be rendered correctly
{
	int n, j;
	int a, b;
	float x, y, z;
	float resize;
	char line[256];

	FILE* fin;
	char filename[FILENAME_MAX];

	fin = NULL;
	printf("\n\nEnter an .off file name including extension: ");
	std::cin >> filename;
	fin = fopen(filename, "rb");

void setInterpolationPoints()
{
	int res = 0;
	bool goodNum = false;
	printf("\n\nEnter the number of interpolation points(must be greater than 0): ");
	while (!goodNum) {
		res = scanf("%d", &numIntPoints);
		if (numIntPoints >= 0) {
			goodNum = true;
		}
		else {
			printf("\n\nThe number was less than or equal to zero, please use a higher number:\n");
	gluPerspective(angleView, (GLfloat)w / (GLfloat)h, 1.5, 40.0);
	glMatrixMode(GL_MODELVIEW);
}

void drawPolygon() {
	//Transform based on mouse input
	glTranslatef(currentframe[0][0], currentframe[0][1], currentframe[0][2]);
	glRotatef(currentframe[1][0], 1.0f, 0.0f, 0.0f);
	glRotatef(currentframe[1][1], 0.0f, 1.0f, 0.0f);
	glRotatef(currentframe[1][2], 0.0f, 0.0f, 1.0f);
	glScalef(currentframe[2][0], currentframe[2][1], currentframe[2][2]);


	//Loop through each face we are drawing
	for (int n = 0; n < numPolygons; n++) {
		//Draw each face
		glBegin(GL_POLYGON);
		//Set the color
		if (flag)
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cdata[n]);

		glNormal3fv(normals[n]);
		//Draw each vertex for the face from the pdata array
		for (int c = 0; c < psize[n]; c++) {
			glVertex3fv(vdata[pdata[n][c]]);
		}
		glEnd();
	}

	if (both) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
		//Loop through each face we are drawing
		for (int n = 0; n < numPolygons; n++) {
			//Draw each face
			glBegin(GL_POLYGON);
			glNormal3fv(normals[n]);
			for (int c = 0; c < psize[n]; c++) glVertex3fv(vdata[pdata[n][c]]);
			glEnd();
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void drawMirror() {

	float size = 4.0;

	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-size, 0.0, -size);
	glVertex3f(-size, 0.0, size);
	glVertex3f(size, 0.0, size);
	glVertex3f(size, 0.0, -size);
	glEnd();
	glEnable(GL_LIGHTING);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	cpos[0] = 5.0 * cos(beta) * sin(alpha);
	cpos[1] = 5.0 * sin(beta);
	cpos[2] = zoom * cos(beta) * cos(alpha);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cpos[0], cpos[1], cpos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glPushMatrix();
	glTranslatef(lpos[0], lpos[1], lpos[2]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_EMISSION, white);
	glutSolidSphere(0.05, 10, 8);
	glPopMatrix();

	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xffffff);

	drawMirror();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glStencilFunc(GL_EQUAL, 1, 0xffffff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glMaterialfv(GL_FRONT, GL_EMISSION, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);

	glPointSize(4.0);
	glLineWidth(2.0);

	glPushMatrix();
	glScalef(1.0, -1.0, 1.0);
	glColor4fv(transparent);
	drawPolygon();
	glPopMatrix();

	glDisable(GL_STENCIL_TEST);

	glPushMatrix();
	//draw the mirror
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawMirror();
	glPopMatrix();

	glPushMatrix();
	drawPolygon();
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void addKeyframe(char user)
{
	if (user == 'u') {

		printf("\n\nEnter the X Y Z translation coordinates(in a list seperated by spaces): ");
		std::cin >> frames[numFrames][0][0] >> frames[numFrames][0][1]>> frames[numFrames][0][2];

		if (frames[numFrames][0][1] <= 0) {
			printf("\n\nThe Y coordinate was less than or equal to zero, and is now being assigned to 1\n");
			frames[numFrames][0][1] = 1;
		}

		printf("\n\nEnter the rotations of X Y Z in degrees (in a list seperated by spaces): ");
		std::cin >> frames[numFrames][1][0] >> frames[numFrames][1][1] >> frames[numFrames][1][2];

		printf("\n\nEnter the scaler values of X Y Z (in a list seperated by spaces): ");
		std::cin >> frames[numFrames][2][0] >> frames[numFrames][2][1] >> frames[numFrames][2][2];

	}
	else {
		//Translation
		frames[numFrames][0][0] = rand() / RAND_MAX;
		frames[numFrames][0][1] = rand() / RAND_MAX;

		// This is for the mirror
		if (frames[numFrames][0][1] < 0) {
			frames[numFrames][0][1] = 3;
		}

		frames[numFrames][0][2] = rand() / RAND_MAX;
		//Rotation
		frames[numFrames][1][0] = rand() / (RAND_MAX / 100);
		frames[numFrames][1][1] = rand() / (RAND_MAX / 100);
		frames[numFrames][1][2] = rand() / (RAND_MAX / 100);
		//Scaling
		frames[numFrames][2][0] = 1;
		frames[numFrames][2][1] = 1;
		frames[numFrames][2][2] = 1;
	}

	numFrames++; if (numFrames >= maxframes) numFrames = maxframes - 1;

}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'c':   // change off file        
		printf("change off file...\n");
		readOFF();
		setRandomColor();
		calculateNormal();
		break;
	case 'x':
		lpos[0] = lpos[0] + 0.2;
		break;
	case 'X':
		lpos[0] = lpos[0] - 0.2;
		break;
	case 'y':
		lpos[1] = lpos[1] + 0.2;
		break;
	case 'Y':
		lpos[1] = lpos[1] - 0.2;
		break;
	case 'z':
		lpos[2] = lpos[2] + 0.2;
		break;
	case 'Z':
		lpos[2] = lpos[2] - 0.2;
		break;
	case 'l':
		flag = !flag;
		break;
	case ' ':
		pause = !pause;
		break;
	case '+':
		zoom -= 1.0;
		if (zoom < 1) zoom = 1;
		break;
	case '-':
		zoom += 1.0;
		if (zoom > 30) zoom = 30;
		break;
	case 'a':
		addKeyframe('r');
		break;
	case 'b':
		addKeyframe('u');
	default:
		break;
	}
	glutPostRedisplay();
}

void specialkey(GLint key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_F1:
		fullscreen = !fullscreen;
		if (fullscreen)
			glutFullScreen();
		else
		{
			glutReshapeWindow(500, 500);
			glutPositionWindow(50, 50);
		}
		break;
	default:
		break;
	}
}

void menu(int value) {
	switch (value) {
	case 0:
		exit(0);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		both = false;
		break;
	case 2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		both = false;
		break;
	case 3:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		both = false;
		break;
	case 4:
		glPolygonMode(GL_BACK, GL_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		both = true;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void createmenu(void) {

	glutCreateMenu(menu);

	// Create an entry
	glutAddMenuEntry("Point Mode", 1);
	glutAddMenuEntry("Line Mode", 2);
	glutAddMenuEntry("Fill Mode", 3);
	glutAddMenuEntry("Both Mode", 4);
	glutAddMenuEntry("Exit", 0);

	// Let the menu respond on the right mouse button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void setTranslation() {

	float xtransFactor = (frames[nextIndex][0][0] - frames[currIndex][0][0]) / numIntPoints;
	float ytransFactor = (frames[nextIndex][0][1] - frames[currIndex][0][1]) / numIntPoints;
	float ztransFactor = (frames[nextIndex][0][2] - frames[currIndex][0][2]) / numIntPoints;

	currentframe[0][0] = frames[currIndex][0][0] + currentIntPoint * xtransFactor;
	currentframe[0][1] = frames[currIndex][0][1] + currentIntPoint * ytransFactor;
	currentframe[0][2] = frames[currIndex][0][2] + currentIntPoint * ztransFactor;
}

void setRotation() {

	float xrotFactor = (frames[nextIndex][1][0] - frames[currIndex][1][0]) / numIntPoints;
	float yrotFactor = (frames[nextIndex][1][1] - frames[currIndex][1][1]) / numIntPoints;
	float zrotFactor = (frames[nextIndex][1][2] - frames[currIndex][1][2]) / numIntPoints;

	currentframe[1][0] = frames[currIndex][1][0] + currentIntPoint * xrotFactor;
	currentframe[1][1] = frames[currIndex][1][1] + currentIntPoint * yrotFactor;
	currentframe[1][2] = frames[currIndex][1][2] + currentIntPoint * zrotFactor;
}

void setScale() {

	float xscaleFactor = (frames[nextIndex][2][0] - frames[currIndex][2][0]) / numIntPoints;
	float yscaleFactor = (frames[nextIndex][2][1] - frames[currIndex][2][1]) / numIntPoints;
	float zscaleFactor = (frames[nextIndex][2][2] - frames[currIndex][2][2]) / numIntPoints;

	currentframe[2][0] = frames[currIndex][2][0] + currentIntPoint * xscaleFactor;
	currentframe[2][1] = frames[currIndex][2][1] + currentIntPoint * yscaleFactor;
	currentframe[2][2] = frames[currIndex][2][2] + currentIntPoint * zscaleFactor;
}

void timer(int value) {

	if (!pause) {
		setTranslation();
		setRotation();
		setScale();

		currentIntPoint = (currentIntPoint + 1) % numIntPoints;
		if (currentIntPoint >= numIntPoints || currentIntPoint <= 0) {
			currIndex = (currIndex + 1) % numFrames;
			nextIndex = (currIndex + 1) % numFrames;
		}

		glutPostRedisplay();      // Post re-paint request to activate display()
	}

	glutTimerFunc(refreshTimer, timer, 0);
}

int main(int argc, char** argv)
{
	writemessage();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(argv[0]);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);


	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkey);
	glutTimerFunc(refreshTimer, timer, 0); // next timer call milliseconds later
	createmenu();

	/*
	int i = 0;
	while (i < 2) {
		i++;
		addKeyframe('u');
	}
	*/

	readOFF();
	calculateNormal();
	setRandomColor();
	setInterpolationPoints();
	glutMainLoop();
	return 0;
}
