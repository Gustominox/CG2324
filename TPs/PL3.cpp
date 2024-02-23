#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}
void drawAxis() {
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(
		-100.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f,
		-100.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f,
		-100.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();
}


void drawCylinder(float radius, float height, int slices) {
	float previousPx = 0;
	float previousPz = radius;
	float angle = 2 * M_PI / slices;

	float alpha = 2 * M_PI / slices;
	// put code to draw cylinder in here
	glBegin(GL_TRIANGLES);
	for (size_t i = 0; i < slices; i++)
	{
		
		
		float x = radius * sin(alpha);
		float z = radius * cos(alpha);


		// base
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(x, 0.0f, z);
		glVertex3f(previousPx, 0, previousPz);

		// LADO
		glColor3f(0.7f, 0.2f, 0.7f);

		glVertex3f(previousPx, 0, previousPz);
		glVertex3f(x, 0.0f, z);
		glVertex3f(x, height, z);

		glVertex3f(x, height, z);
		glVertex3f(previousPx, height, previousPz);
		glVertex3f(previousPx, 0, previousPz);


		// topo
		glColor3f(0.0f, 0.0f, 1.0f); // blue

		glVertex3f(0.0f, height, 0.0f);
		glVertex3f(previousPx, height, previousPz);
		glVertex3f(x, height, z);



		previousPx = x;
		previousPz = z;

		alpha += angle;


		
	}
	glEnd();
}
float mode = GL_LINE;

float alph = M_PI / 4;
float beta = M_PI / 4;



void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	float hAlpha = 10.0 * cos(beta);

	float cameraX = hAlpha * sin(alph);
	float cameraZ = hAlpha * cos(alph);
	float cameraY = 10.0 * sin(beta);
	
	
	gluLookAt(cameraX, cameraY, cameraZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	drawAxis();
	glPolygonMode(GL_FRONT, mode);
	drawCylinder(1, 2, 200);

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char key, int xx, int yy) {

	// put code to process regular keys in here

	
	if (key == '1') {
		mode = GL_FILL;
	}
	else if (key == '2') {
		mode = GL_LINE;
	}
	else if (key == '3') {
		mode = GL_POINT;
	}
	else if (key == 'w') {
		beta += 0.1;
	}
	else if (key == 'a') {
		alph += 0.1;

	}
	else if (key == 's') {
		beta -= 0.1;
	}
	else if (key == 'd') {
		alph -= 0.1;

	}

	if (beta > M_PI / 2) beta = M_PI / 2;
	if (beta <  -1*M_PI / 2) beta = -1*M_PI / 2;
	glutPostRedisplay();

}


void processSpecialKeys(int key, int xx, int yy) {

	// put code to process special keys in here

}


int main(int argc, char** argv) {

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI-UM");

	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}