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
#include <string.h>

#define MAX_MODELS 10

// std::string CONFIGS_DIR = "C:\\Users\\gimez\\Desktop\\CG2324\\Projeto\\test_files\\test_files_phase_2";
// std::string CONFIGS_DIR = "C:\\Users\\gimez\\Desktop\\CG2324\\Projeto\\test_files";
std::string CONFIGS_DIR = "/mnt/additional/repos/CG2324/Projeto/test_files"; // alex
float camX, camY, camZ;
float lookAtX, lookAtY, lookAtZ;
float upX, upY, upZ;
int WINDOW_WIDTH, WINDOW_HEIGHT;

int num_models = 0;

double fov, nearPlane, farPlane;

float frames;

float mode = GL_LINE;
int timebase = glutGet(GLUT_ELAPSED_TIME);
int frame = 0;

class Point
{
private:
	double x, y, z;

public:
	Point(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {}

	double getX() const { return x; }
	double getY() const { return y; }
	double getZ() const { return z; }

	void setX(double _x) { x = _x; }
	void setY(double _y) { y = _y; }
	void setZ(double _z) { z = _z; }
};

class Transformation
{
private:
	std::string type;
	double x, y, z;
	float angle;
	double time;
	bool align;
	std::vector<Point> points;

public:
	Transformation()
		: type(""), x(0.0), y(0.0), z(0.0), angle(0.0f), time(0.0), align(false) {}

	Transformation(std::string type, double x, double y, double z, float angle = 0.0f, double time = 0.0, bool align = false)
		: type(type), x(x), y(y), z(z), angle(angle), time(time), align(align) {}

	// Getters
	std::string getType() const { return type; }
	double getX() const { return x; }
	double getY() const { return y; }
	double getZ() const { return z; }
	float getAngle() const { return angle; }
	double getTime() const { return time; } // Getter for time
	bool getAlign() const { return align; } // Getter for align

	// Setters
	void setType(const std::string &newType) { type = newType; }
	void setX(double newX) { x = newX; }
	void setY(double newY) { y = newY; }
	void setZ(double newZ) { z = newZ; }
	void setAngle(float newAngle) { angle = newAngle; }
	void setTime(double newTime) { time = newTime; }   // Setter for time
	void setAlign(bool newAlign) { align = newAlign; } // Setter for align

	void addPoint(const Point &point)
	{
		points.push_back(point);
	}

	void apply() const
	{
		if (type == "translate")
		{
			glTranslatef(x, y, z);
		}
		else if (type == "rotate")
		{
			glRotatef(angle, x, y, z); // Use angle in glRotatef
		}
		else if (type == "scale")
		{
			glScalef(x, y, z);
		}
	}
};

class Model
{
private:
	GLuint bufferID;
	std::string filePath;
	int numVertices;
	std::vector<Transformation> transformationList;

public:
	Model() : bufferID(0), numVertices(0) {}

	// Set vertices data
	void setVertices(int vertices)
	{
		numVertices = vertices;
	}
	void setFilePath(std::string fp)
	{
		filePath = fp;
	}
	GLuint getIndex()
	{
		return bufferID;
	}

	GLuint getNumVertices()
	{
		return numVertices;
	}

	std::vector<Transformation> getTransf()
	{
		return transformationList;
	}

	void setTransf(std::vector<Transformation> tList)
	{
		transformationList = tList;
	}

	void importModel(int bufferIndex)
	{
		GLuint Itemp;
		glGenBuffers(bufferIndex, &Itemp);
		bufferID = Itemp;
		std::ifstream arquivo(filePath);

		if (!arquivo.is_open())
		{
			std::cerr << "Erro ao abrir o arquivo. " << filePath << std::endl;
			return;
		}
		else
		{
			std::cout << "Abriu! " << filePath << std::endl;
		}

		std::string linha;
		std::vector<float> vertexB;
		while (std::getline(arquivo, linha))
		{

			std::istringstream iss(linha);
			char descartavel;
			float num1, num2, num3;
			iss >> descartavel; // Descarta o primeiro caractere
			iss.ignore();		// Ignora o ponto e v�rgula
			iss >> num1;		// L� o primeiro n�mero
			iss.ignore();		// Ignora o ponto e v�rgula
			iss >> num2;		// L� o segundo n�mero
			iss.ignore();		// Ignora o ponto e v�rgula
			iss >> num3;		// L� o terceiro n�mero
			vertexB.push_back(num1);
			vertexB.push_back(num2);
			vertexB.push_back(num3);
		}

		numVertices = vertexB.size() / 3;

		glBindBuffer(GL_ARRAY_BUFFER, bufferID);

		glBufferData(GL_ARRAY_BUFFER,				 // tipo do buffer, s� � relevante na altura do desenho
					 sizeof(float) * vertexB.size(), // tamanho do vector em bytes
					 vertexB.data(),				 // os dados do array associado ao vector
					 GL_STATIC_DRAW);				 // indicativo da utiliza��o (est�tico e para desenho)
	}

	// Draw model
	void draw()
	{
		if (bufferID != 0)
		{

			for (Transformation t : transformationList)
				t.apply();

			glBindBuffer(GL_ARRAY_BUFFER, bufferID);	// ESCOLHE O BUFFER NA POS bufferID
			glVertexPointer(3, GL_FLOAT, 0, 0);			// dizer qual é a config do buffer
			glDrawArrays(GL_TRIANGLES, 0, numVertices); // como se desenha o buffer
		}
	}
};

Model modelsArray[MAX_MODELS];

void drawAxis()
{
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(
		-200.0f, 0.0f, 0.0f);
	glVertex3f(200.0f, 0.0f, 0.0f);
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

	// Color white
	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();
}

void changeSize(int w, int h)
{

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
	gluPerspective(fov, ratio, nearPlane, farPlane);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void printTransformations(const std::vector<Transformation> &transformations)
{
	for (const auto &transformation : transformations)
	{
		std::cout << "Type: " << transformation.getType() << ", X: " << transformation.getX() << ", Y: " << transformation.getY() << ", Z: " << transformation.getZ() << std::endl;
	}
}

void renderScene(void)
{

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();

	gluLookAt(camX, camY, camZ,
			  lookAtX, lookAtY, lookAtZ,
			  upX, upY, upZ);

	// desenhar axis
	drawAxis();

	// desenhar models VBO's
	for (int i = 0; i < num_models; i++)
	{

		std::cout << "Modelo " << i << "\n";
		printTransformations(modelsArray[i].getTransf());

		glPushMatrix();
		modelsArray[i].draw();
		glPopMatrix();
	}

	// calcular frames
	frame++;
	float fps = 0.0f;
	int time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000)
	{
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

void processKeys(unsigned char key, int xx, int yy)
{

	// Mudar modos de visualiza��o
	if (key == '1')
	{
		mode = GL_FILL;
	}
	else if (key == '2')
	{
		mode = GL_LINE;
	}
	else if (key == '3')
	{
		mode = GL_POINT;
	}

	glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy)
{
}

void printInfo()
{

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	printf("\nUse Arrows to move the camera up/down and left/right\n");
	printf("Page Up and Page Down control the distance from the camera to the origin\n");
}

Transformation parseSingleTransformation(const std::string &token)
{
	std::istringstream tokenStream(token);
	std::string type;
	double x, y, z;
	float angle = 0.0f; // Default angle

	tokenStream >> type;
	if (type == "rotate")
	{
		tokenStream >> angle;
	}
	else if (type == "translate")
	{
	}

	tokenStream >> std::skipws >> x >> std::skipws >> y >> std::skipws >> z;

	return Transformation(type, x, y, z, angle);
}

Transformation parseSingleTransformationNode(pugi::xml_node transformation)
{
	std::string type;
	double x, y, z;
	float angle = 0.0f; // Default angle
	double time = 0.0;
	bool align = false;

	type = std::string(transformation.name());
	Transformation t;
	t.setType(type);
	if (type == "translate")
	{
		// Check if there are multiple <point> elements
		if (transformation.child("point"))
		{

			time = transformation.attribute("time").as_double();
			align = transformation.attribute("align").as_bool();
			t.setTime(time);
			t.setAlign(align);

			for (pugi::xml_node point : transformation.children("point"))
			{

				double x = point.attribute("x").as_double();
				double y = point.attribute("y").as_double();
				double z = point.attribute("z").as_double();

				t.addPoint(Point(x, y, z));
			}
		}
		else
		{
			// Directly read x, y, z attributes
			x = transformation.attribute("x").as_double();
			y = transformation.attribute("y").as_double();
			z = transformation.attribute("z").as_double();
			t.setX(x);
			t.setY(y);
			t.setZ(z);
		}
	}
	else if (type == "rotate")
	{
		angle = transformation.attribute("angle").as_float();
		x = transformation.attribute("x").as_double();
		y = transformation.attribute("y").as_double();
		z = transformation.attribute("z").as_double();
		t.setX(x);
		t.setY(y);
		t.setZ(z);
		t.setAngle(angle);
	}
	else if (type == "scale")
	{
		x = transformation.attribute("x").as_double();
		y = transformation.attribute("y").as_double();
		z = transformation.attribute("z").as_double();
		t.setX(x);
		t.setY(y);
		t.setZ(z);
		t.setAngle(angle);
	}

	return t;
}

void readGroup(pugi::xml_node group, std::vector<Transformation> transVector)
{

	// translate 1 2 3;rotate 90 1 2 3;scales 0.75 0.75 0.75;
	for (pugi::xml_node transformation : group.child("transform").children())
	{
		std::string transformationString;

		transformationString += std::string(transformation.name()) + " ";

		for (pugi::xml_attribute attr : transformation.attributes())
		{
			transformationString += std::string(attr.value()) + " ";
		}

		transformationString += ";";

		transVector.push_back(parseSingleTransformation(transformationString));
	}

	for (pugi::xml_node model : group.child("models").children("model"))
	{
		std::string modelFile = model.attribute("file").as_string();
		// modelsArray[num_models].setFilePath("models\\" + modelFile);	//windows
		modelsArray[num_models].setFilePath("models/" + modelFile); // linux

		modelsArray[num_models].setTransf(transVector);
		num_models++;
	}
	int j = 0;
	for (pugi::xml_node childGroup : group.children("group"))
	{
		std::cout << "Group " << j;
		j++;
		std::vector<Transformation> newVec = transVector;
		readGroup(childGroup, newVec);
	}
}

void readConfig(const pugi::xml_node &world)
{

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

	// Parse model paths
	for (pugi::xml_node group : world.children("group"))
	{
		std::vector<Transformation> transVector;
		readGroup(group, transVector);
		// std::cout << "Group: " << transVector. << "\n";
	}
}

void printConfig(const pugi::xml_node &node, int depth = 0)
{
	// Output node name with indentation based on depth
	std::cout << std::string(depth * 2, ' ') << "Node: " << node.name() << std::endl;

	// Print attributes of the node
	for (pugi::xml_attribute attr : node.attributes())
	{
		std::cout << std::string(depth * 2 + 2, ' ') << "Attribute: " << attr.name() << " = " << attr.value() << std::endl;
	}

	// Output node value if any
	const char *value = node.child_value();

	// linux
	if (value && strlen(value) > 0)
	{
		std::cout << std::string(depth * 2 + 2, ' ') << "Value: " << value << std::endl;
	}

	// Recursively print children
	for (pugi::xml_node child : node.children())
	{
		printConfig(child, depth + 1);
	}
}

int main(int argc, char **argv)
{

	// std::string xmlFilePath = CONFIGS_DIR + "\\" + "solar.xml"; //windows
	std::string xmlFilePath = CONFIGS_DIR + "/test_files_phase_3/" + "test_3_2.xml"; // linux com rotacoes
	// std::string xmlFilePath = CONFIGS_DIR + "/" + "solar_estatic.xml"; //linux sem rotacoes

	// Load the XML file
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(xmlFilePath.c_str());
	// Start printing from the root node
	// printConfig(doc.root());

	pugi::xml_node world = doc.child("world");
	readConfig(world);

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CG@DI-UM");

	// Required callback registry
	glutDisplayFunc(renderScene);
	// glutIdleFunc(renderScene); // PARA CALCULAR OS FRAMES
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

	for (size_t i = 1; i < num_models + 1; i++)
	{
		// std::cout << "MODELO " << i << ": " << modelsArray[i - 1].getTransf() << "\n";

		modelsArray[i - 1].importModel(i);
	}

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
