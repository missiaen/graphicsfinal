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

static GLint numVertices, numPolygons, numEdges;
static GLfloat **vdata;   //array for vertex data
static GLuint **pdata;    //array for polygon data (vertex indices)
static GLuint *psize;     //array for polygon size
static GLfloat **normals; //array for polygon normals
static GLfloat **cdata;//array for polygon color

bool fullscreen = false;
bool mouseDown = false;

float currentframe[3][3] = {
	{0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0},
	{1.0, 1.0, 1.0}
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
float frames[maxframes][3][3] = {
	{ {-1,1,-1}, {-100,-100,-100}, {0.9,0.9,0.9} },
	{ {1,-1,0}, {300,200,100}, {1.0,2,2} }
};
float zoom = 5.0;


bool both = false;

#define min(x,y) ((x)<(y) ? (x) : (y))
#define max(x,y) ((x)>(y) ? (x) : (y))


void writemessage()
{
	printf("\n\
	Final Project by Matthew Buchanan, Philip Kocol, and Garrett Missiaen\n\
	Allows defining keyframs and animates between them\n\
	a ------------------------- add keyframe\n\
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
	while (fin == NULL) {
		printf("\n\nEnter an .off file name including extension (or press Enter to abort): ");
		int result = scanf("%99[^\n]%*c", filename);
		if (result != 1) exit(0);
		fin = fopen(filename, "rb");
	};

	/* OFF format */
	while (fgets(line, 256, fin) != NULL) {
		if (line[0] == 'O' && line[1] == 'F' && line[2] == 'F')
			break;
	}
	fscanf(fin, "%d %d %d\n", &numVertices, &numPolygons, &numEdges);

	printf("Number of vertices  = %d\n", numVertices);
	printf("Number of polygons = %d\n", numPolygons);
	printf("Number of edges = %d\n", numEdges);
	printf("loading vedrtices and polygons... ");

	vdata = new GLfloat*[numVertices];
	for (int i = 0; i<numVertices; i++)
		vdata[i] = new GLfloat[3];

	pdata = new GLuint*[numPolygons]; //array for storing polygon data (vertex indices)
	psize = new GLuint[numPolygons];  //array for storing polygon size

	resize = 0.0001;
	for (n = 0; n < numVertices; n++) { //read vertex data
		fscanf(fin, "%f %f %f\n", &x, &y, &z);
		vdata[n][0] = x;
		resize = max(resize, fabs(x));
		vdata[n][1] = y;
		resize = max(resize, fabs(y));
		vdata[n][2] = z;
		resize = max(resize, fabs(z));
	}

	for (n = 0; n < numVertices; n++) { //adjust vertex data
		vdata[n][0] = vdata[n][0] / resize;
		vdata[n][1] = vdata[n][1] / resize;
		vdata[n][2] = vdata[n][2] / resize;
	}

	for (n = 0; n < numPolygons; n++) {
		fscanf(fin, "%d", &a);
		psize[n] = a;  //store n-th polygon size in psize[n]
		pdata[n] = new GLuint[a];
		for (j = 0; j < a; j++) { //read and save vertices of n-th polygon
			fscanf(fin, "%d", &b);
			pdata[n][j] = b;
		}
	}
	fclose(fin);
	printf("done.\n");

}

void setInterpolationPoints()
{
	int res = 0;
	bool goodNum = false;
	printf("\n\nEnter the number of interpolation points(must be greater than 0): ");
	while (!goodNum) {
		res = scanf("%d", &numIntPoints);
		if (res >= 0) {
			goodNum = true;
		}
	}
}

void calculateNormal()//calculates the normal vector for every polygon
					  //using the first three vertices, assuming they occur in ccw order
{
	normals = new GLfloat*[numPolygons];
	for (int i = 0; i<numPolygons; i++)
		normals[i] = new GLfloat[3];

	for (int i = 0; i<numPolygons; i++) {

		GLint t1 = pdata[i][0], t2 = pdata[i][1], t3 = pdata[i][2];
		GLfloat v1[3] = { vdata[t1][0],vdata[t1][1],vdata[t1][2] };
		GLfloat v2[3] = { vdata[t2][0],vdata[t2][1],vdata[t2][2] };
		GLfloat v3[3] = { vdata[t3][0],vdata[t3][1],vdata[t3][2] };

		GLfloat n1[3] = { v2[0] - v1[0],v2[1] - v1[1],v2[2] - v1[2] };
		GLfloat n2[3] = { v3[0] - v1[0],v3[1] - v1[1],v3[2] - v1[2] };

		float	normx = (n1[1] * n2[2]) - (n2[1] * n1[2]),
			normy = (n1[2] * n2[0]) - (n2[2] * n1[0]),
			normz = (n1[0] * n2[1]) - (n2[0] * n1[1]);

		float factor = sqrt(pow(normx, 2) + pow(normy, 2) + pow(normz, 2));
		normx /= factor;
		normy /= factor;
		normz /= factor;
		normals[i][0] = normx;
		normals[i][1] = normy;
		normals[i][2] = normz;
		//---------------------------------------------------------------------

	}
}

void setRandomColor() {
	bool flip = false;
	cdata = new GLfloat*[numPolygons];
	for (int i = 0; i < numPolygons; i++) {
		cdata[i] = new GLfloat[4];
		cdata[i][0] = ((double)i / (numPolygons));
		cdata[i][1] = 1 - cdata[i][0];
		cdata[i][2] = (double)(i % 1000) / 1000;
		cdata[i][3] = 1.0;

		if ((i % 1000) == 0) flip = !flip;
		if (flip) cdata[i][2] = 1 - cdata[i][2];
	}
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angleView, (GLfloat)w / (GLfloat)h, 1.5, 40.0);
	glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	glMaterialfv(GL_FRONT, GL_EMISSION, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);

	glPointSize(4.0);
	glLineWidth(2.0);

	glPushMatrix();

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

	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void addKeyframe()
{
	//TODO: Add a keyframe from user input right here

	//Translation
	frames[numFrames][0][0] = rand() / RAND_MAX;
	frames[numFrames][0][1] = rand() / RAND_MAX;
	frames[numFrames][0][2] = rand() / RAND_MAX;
	//Rotation
	frames[numFrames][1][0] = rand() / (RAND_MAX / 100);
	frames[numFrames][1][1] = rand() / (RAND_MAX / 100);
	frames[numFrames][1][2] = rand() / (RAND_MAX / 100);
	//Scaling
	frames[numFrames][2][0] = 1;
	frames[numFrames][2][1] = 1;
	frames[numFrames][2][2] = 1;
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
		addKeyframe();
	default:
		break;
	}
	glutPostRedisplay;
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

	readOFF();
	calculateNormal();
	setRandomColor();
	setInterpolationPoints();
	glutMainLoop();
	return 0;
}


