#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

void changeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you can t make a window with zero width).
	if (h == 0)
		h = 1;
	// compute window's aspect ratio
	float ratio = w * 1.0f / h;
	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load the identity matrix
	glLoadIdentity();
	// Set the perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);
	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);

	// finally set the viewport to be the entire window
	glViewport(0, 0, w, h);
}

float size = 3;
float scale = 0;	
float step = 0.001;
float sinValue = 0;
float rotationAngle = 0.0f;
// converting degrees to radians
// xDegrees * 3.14159 / 180;
void renderScene(void)
{
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// set camera
	glLoadIdentity();
	gluLookAt(5.0f, 5.0f, 10.0f, // onde a camara esta
		0.0f, 0.0f, -1.0f, // para onde esta a olhar
		0.0f, 1.0f, 0.0f);

	sinValue += step;
	scale = fabsf(sin(sinValue));

		
	// put drawing instructions here
	glColor3f(0.0f, 0.0f, 1.0f); // Blue color
	// Apply rotation to the solid cube
	glPushMatrix();
	glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Rotate around arbitrary axis

	// Draw solid cube
	glutSolidCube(size * scale);


	// Increment rotation angle for next frame
	rotationAngle += 0.02; // Adjust the rotation speed as desired

	glColor3f(1.0f, 0.0f, 0.0f); // Red color
	glutWireCube(size);
	glPopMatrix(); // Restore original matrix


	// End of frame
	glutSwapBuffers();
}


void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
}


int main(int argc, char** argv)
{
	// put GLUT init here
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI");
	// put callback registry here
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);


	
	// some OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	// enter GLUT s main cycle
	glutMainLoop();
	
	return 1;
}