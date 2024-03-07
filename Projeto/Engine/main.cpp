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
 
#define MAX_MODELS 10

float camX, camY, camZ;
float lookAtX, lookAtY, lookAtZ;
float upX, upY, upZ;
int WINDOW_WIDTH ,WINDOW_HEIGHT;

int iModel [MAX_MODELS];
int modelVert[MAX_MODELS];

std::vector<float> models[MAX_MODELS];
int num_models = 0;

double fov, nearPlane, farPlane;

float frames;

float mode = GL_LINE;
int timebase = glutGet(GLUT_ELAPSED_TIME);
int frame = 0;


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
	gluPerspective(fov ,ratio, nearPlane,farPlane);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}



void renderScene(void) {

	

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();

	gluLookAt(	camX, camY, camZ,
				lookAtX, lookAtY, lookAtZ,
				upX, upY, upZ
			);

	// desenhar axis
	drawAxis();

	// desenhar cilindro VBO
	for (int i = 0; i < num_models; i++)
	{
		glBindBuffer(GL_ARRAY_BUFFER, iModel[i]); // ESCOLHE O BUFFER NA POS VERTICES
		glVertexPointer(3, GL_FLOAT, 0, 0);		 // dizer qual é a config do buffer
		glDrawArrays(GL_TRIANGLES, 0, modelVert[i]); // como se desenha o buffer
	}

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


}


void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	printf("\nUse Arrows to move the camera up/down and left/right\n");
	printf("Page Up and Page Down control the distance from the camera to the origin");
}

void importModel(const std::string& path) {
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

	modelVert[num_models] = vertexB.size() / 3;

	glBindBuffer(GL_ARRAY_BUFFER, iModel[num_models]);

	glBufferData(GL_ARRAY_BUFFER, // tipo do buffer, s� � relevante na altura do desenho
		sizeof(float) * vertexB.size(), // tamanho do vector em bytes
		vertexB.data(), // os dados do array associado ao vector
		GL_STATIC_DRAW); // indicativo da utiliza��o (est�tico e para desenho)

	num_models++;
}

void readConfig(const pugi::xml_node& world) {
	// Load the XML file
	

	// Parse window attributes
	pugi::xml_node window = world.child("window");
	WINDOW_WIDTH = window.attribute("width").as_int();
	WINDOW_HEIGHT = window.attribute("height").as_int();

	// Parse camera attributes
	pugi::xml_node camera = world.child("camera");

	camX = camera.child("position").attribute("x").as_double();
	camY = camera.child("position").attribute("y").as_double();
	camZ = camera.child("position").attribute("z").as_double();
	
	lookAtX = camera.child("lookAt").attribute("x").as_double();
	lookAtY = camera.child("lookAt").attribute("y").as_double();
	lookAtZ = camera.child("lookAt").attribute("z").as_double();
	
	upX = camera.child("up").attribute("x").as_double();
	upY = camera.child("up").attribute("y").as_double();
	upZ = camera.child("up").attribute("z").as_double();
	
	fov = camera.child("projection").attribute("fov").as_double();
	nearPlane = camera.child("projection").attribute("near").as_double();
	farPlane = camera.child("projection").attribute("far").as_double();

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

std::string CONFIGS_DIR = "C:\\Users\\gimez\\Desktop\\CG2324\\Projeto\\test_files\\test_files_phase_1";

int main(int argc, char **argv) {

	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file("C:\\Users\\gimez\\Desktop\\CG2324\\Projeto\\test_files\\test_files_phase_1\\test_1_1.xml");

	// Start printing from the root node
	printNode(doc.root());


	// Access the root node
	pugi::xml_node world = doc.child("world");

	readConfig(world);

	importModel("plane.3d");

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
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


	printInfo();

	//glGenBuffers(1, &vertices);

// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
