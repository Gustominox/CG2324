#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "pugixml.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

float alfa = 0.0f, beta = 0.0f, radius = 5.0f;
float camX, camY, camZ;

GLuint verticeCount;
GLuint vertices = 0;
float frames;


void spherical2Cartesian() {

	camX = radius * cos(beta) * sin(alfa);
	camY = radius * sin(beta);
	camZ = radius * cos(beta) * cos(alfa);
}


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
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
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


/*----------------------------------------------------------------------------------- 
	Draw Cylinder with strips and fans

	  parameters: radius, height, sides

-----------------------------------------------------------------------------------*/


void cylinder0(float radius, float height, int sides) {

	int i;
	float step;

	step = 360.0/sides;

	glColor3f(1,0,0);
	glBegin(GL_TRIANGLE_FAN);

		glVertex3f(0,height*0.5,0);
		for (i=0; i <= sides; i++) {
			glVertex3f(cos(i * step * M_PI/180.0)*radius,height*0.5,-sin(i * step *M_PI/180.0)*radius);
		}
	glEnd();

	glColor3f(0,1,0);
	glBegin(GL_TRIANGLE_FAN);

		glVertex3f(0,-height*0.5,0);
		for (i=0; i <= sides; i++) {
			glVertex3f(cos(i * step * M_PI/180.0)*radius,-height*0.5,sin(i * step *M_PI/180.0)*radius);
		}
	glEnd();

	glColor3f(0,0,1);
	glBegin(GL_TRIANGLE_STRIP);

		for (i=0; i <= sides; i++) {
			glVertex3f(cos(i * step * M_PI/180.0)*radius, height*0.5,-sin(i * step *M_PI/180.0)*radius);
			glVertex3f(cos(i * step * M_PI/180.0)*radius,-height*0.5,-sin(i * step *M_PI/180.0)*radius);
		}
	glEnd();
}


/*-----------------------------------------------------------------------------------
	Draw Cylinder

		parameters: radius, height, sides

-----------------------------------------------------------------------------------*/


void cylinder(float radius, float height, int sides) {

	int i;
	float step;

	step = 360.0 / sides;

	glBegin(GL_TRIANGLES);

	// top
	for (i = 0; i < sides; i++) {
		glVertex3f(0, height*0.5, 0);
		glVertex3f(cos(i * step * M_PI / 180.0)*radius, height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i+1) * step * M_PI / 180.0)*radius, height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
	}

	// bottom
	for (i = 0; i < sides; i++) {
		glVertex3f(0, -height*0.5, 0);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, -height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
		glVertex3f(cos(i * step * M_PI / 180.0)*radius, -height*0.5, -sin(i * step *M_PI / 180.0)*radius);
	}

	// body
	for (i = 0; i <= sides; i++) {

		glVertex3f(cos(i * step * M_PI / 180.0)*radius, height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos(i * step * M_PI / 180.0)*radius, -height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);

		glVertex3f(cos(i * step * M_PI / 180.0)*radius, -height*0.5, -sin(i * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, -height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
		glVertex3f(cos((i + 1) * step * M_PI / 180.0)*radius, height*0.5, -sin((i + 1) * step *M_PI / 180.0)*radius);
	}
	glEnd();
}

float mode = GL_LINE;
int timebase = glutGet(GLUT_ELAPSED_TIME);
int frame=0;

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();

	gluLookAt(camX, camY, camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	// desenhar cilindro VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, verticeCount);

	// calcular frames 
	frame++;
	float fps=0.0f;
	int time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fps = frame * 1000.0 / (time - timebase);
		timebase = time;
		frame = 0;

		// nome da window == FPS's
		char title[50];
		sprintf(title, "%.2f FPS", fps);
		glutSetWindowTitle(title);
	}

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char key, int xx, int yy) {

	// Mudar modos de visualiza��o
	if (key == '1') {
		mode = GL_FILL;
	}
	else if (key == '2') {
		mode = GL_LINE;
	}
	else if (key == '3') {
		mode = GL_POINT;
	}
	
	glutPostRedisplay();

}



void processSpecialKeys(int key, int xx, int yy) {

	switch (key) {

	case GLUT_KEY_RIGHT:
		alfa -= 0.1; break;

	case GLUT_KEY_LEFT:
		alfa += 0.1; break;

	case GLUT_KEY_UP:
		beta += 0.1f;
		if (beta > 1.5f)
			beta = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		beta -= 0.1f;
		if (beta < -1.5f)
			beta = -1.5f;
		break;

	case GLUT_KEY_PAGE_DOWN: radius -= 0.1f;
		if (radius < 0.1f)
			radius = 0.1f;
		break;

	case GLUT_KEY_PAGE_UP: radius += 0.1f; break;
	}
	spherical2Cartesian();
	glutPostRedisplay();

}


void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	printf("\nUse Arrows to move the camera up/down and left/right\n");
	printf("Page Up and Page Down control the distance from the camera to the origin");
}

void createBuffer(float radius, float height, int sides) {
		
	std::vector<float> vertexB;

	
	int i;
	float step;

	step = 360.0 / sides;


	// top
	for (i = 0; i < sides; i++) {

		vertexB.push_back(0);
		vertexB.push_back(height * 0.5);
		vertexB.push_back( 0);

		vertexB.push_back(cos(i * step * M_PI / 180.0) * radius);
		vertexB.push_back(height * 0.5);
		vertexB.push_back(-sin(i * step * M_PI / 180.0) * radius);

		vertexB.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
		vertexB.push_back(height * 0.5);
		vertexB.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);
	}
	
	// bottom
	for (i = 0; i < sides; i++) {
		vertexB.push_back(0);
		vertexB.push_back(-height * 0.5);
		vertexB.push_back(0);

		vertexB.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
		vertexB.push_back(-height * 0.5);
		vertexB.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);

		vertexB.push_back(cos(i * step * M_PI / 180.0) * radius);
		vertexB.push_back(-height * 0.5);
		vertexB.push_back(-sin(i * step * M_PI / 180.0) * radius);
	}

	// body
	for (i = 0; i <= sides; i++) {

		vertexB.push_back(cos(i * step * M_PI / 180.0) * radius);
		vertexB.push_back(height * 0.5);
		vertexB.push_back(-sin(i * step * M_PI / 180.0) * radius);
		
		vertexB.push_back(cos(i * step * M_PI / 180.0) * radius);
		vertexB.push_back(-height * 0.5);
		vertexB.push_back(-sin(i * step * M_PI / 180.0) * radius);
		
		vertexB.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
		vertexB.push_back(height * 0.5);
		vertexB.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);

		vertexB.push_back(cos(i * step * M_PI / 180.0) * radius);
		vertexB.push_back(-height * 0.5);
		vertexB.push_back(-sin(i * step * M_PI / 180.0) * radius);
		vertexB.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
		vertexB.push_back(-height * 0.5);
		vertexB.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);

		vertexB.push_back(cos((i + 1) * step * M_PI / 180.0) * radius);
		vertexB.push_back(height * 0.5); 
		vertexB.push_back(-sin((i + 1) * step * M_PI / 180.0) * radius);
	}
	
	verticeCount = vertexB.size() / 3;
	
	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	
	glBufferData(GL_ARRAY_BUFFER, // tipo do buffer, s� � relevante na altura do desenho
				 sizeof(float) * vertexB.size(), // tamanho do vector em bytes
				 vertexB.data(), // os dados do array associado ao vector
				 GL_STATIC_DRAW); // indicativo da utiliza��o (est�tico e para desenho)




}

void lerFicheiro(const std::string& path) {
	std::ifstream arquivo(path);
	if (!arquivo.is_open()) {
		std::cerr << "Erro ao abrir o arquivo." << std::endl;
		return;
	}

	std::string linha;
	std::vector<float> vertexB;
	while (std::getline(arquivo, linha)) {
		//std::cout << "Linha lida: " << linha << std::endl; // Mensagem de depura��o
		std::istringstream iss(linha);
		char descartavel;
		float num1, num2, num3;
		iss >> descartavel; // Descarta o primeiro caractere
		iss.ignore(); // Ignora o ponto e v�rgula
		iss >> num1; // L� o primeiro n�mero
		iss.ignore(); // Ignora o ponto e v�rgula
		iss >> num2; // L� o segundo n�mero
		iss.ignore(); // Ignora o ponto e v�rgula
		iss >> num3; // L� o terceiro n�mero
		vertexB.push_back(num1);
		vertexB.push_back(num2);
		vertexB.push_back(num3);
	}
}
void printNode(const pugi::xml_node& node, int depth = 0) {
	// Output node name with indentation based on depth
	std::cout << std::string(depth * 2, ' ') << "Node: " << node.name() << std::endl;

	// Print attributes of the node
	for (pugi::xml_attribute attr : node.attributes()) {
		std::cout << std::string(depth * 2 + 2, ' ') << "Attribute: " << attr.name() << " = " << attr.value() << std::endl;
	}

	// Output node value if any
	const char* value = node.child_value();
	if (value && std::strlen(value) > 0) {
		std::cout << std::string(depth * 2 + 2, ' ') << "Value: " << value << std::endl;
	}

	// Recursively print children
	for (pugi::xml_node child : node.children()) {
		printNode(child, depth + 1);
	}
}

int main(int argc, char **argv) {

	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file("C:\\Users\\gimez\\Desktop\\CG2324\\Projeto\\config_example.xml");

	// Start printing from the root node
	printNode(doc.root());

	



// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene); // PARA CALCULAR OS FRAMES
	glutReshapeFunc(changeSize);
	
// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	// init GLEW
#ifndef __APPLE__
	glewInit();
#endif


//  OpenGL settings
	glEnableClientState(GL_VERTEX_ARRAY); // MODO VBO ATIVO

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_LINE);

	spherical2Cartesian();

	printInfo();

	glGenBuffers(1, &vertices);
	createBuffer(1,2,10);

// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
