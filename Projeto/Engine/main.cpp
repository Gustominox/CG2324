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

void buildRotMatrix(float *x, float *y, float *z, float *m)
{

	m[0] = x[0];
	m[1] = x[1];
	m[2] = x[2];
	m[3] = 0;
	m[4] = y[0];
	m[5] = y[1];
	m[6] = y[2];
	m[7] = 0;
	m[8] = z[0];
	m[9] = z[1];
	m[10] = z[2];
	m[11] = 0;
	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}

void cross(float *a, float *b, float *res)
{

	res[0] = a[1] * b[2] - a[2] * b[1];
	res[1] = a[2] * b[0] - a[0] * b[2];
	res[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(float *a)
{

	float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0] / l;
	a[1] = a[1] / l;
	a[2] = a[2] / l;
}

float length(float *v)
{

	float res = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	return res;
}

void multMatrixVector(float *m, float *v, float *res)
{

	for (int j = 0; j < 4; ++j)
	{
		res[j] = 0;
		for (int k = 0; k < 4; ++k)
		{
			res[j] += v[k] * m[j * 4 + k];
		}
	}
}

float multVectors(float *m, float *v)
{
	float res = 0;
	for (int i = 0; i < 4; ++i)
	{
		res = res + m[i] * v[i];
	}
	return res;
}

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

	void getCatmullRomPoint(float t, Point p0, Point p1, Point p2, Point p3, Point *pos, Point *deriv)
	{

		// catmull-rom matrix
		float m[4][4] = {{-0.5f, 1.5f, -1.5f, 0.5f},
						 {1.0f, -2.5f, 2.0f, -0.5f},
						 {-0.5f, 0.0f, 0.5f, 0.0f},
						 {0.0f, 1.0f, 0.0f, 0.0f}};

		// Compute A = M * P
		float vecX[4] = {p0.getX(), p1.getX(), p2.getX(), p3.getX()}; // x
		float vecY[4] = {p0.getY(), p1.getY(), p2.getY(), p3.getY()}; // y
		float vecZ[4] = {p0.getZ(), p1.getZ(), p2.getZ(), p3.getZ()}; // z

		float A[3][4];
		multMatrixVector(*m, vecX, A[0]);
		multMatrixVector(*m, vecY, A[1]);
		multMatrixVector(*m, vecZ, A[2]);

		// Compute pos = T * A

		float tv[4] = {t * t * t, t * t, t, 1};

		(*pos).setX(multVectors(A[0], tv));
		(*pos).setY(multVectors(A[1], tv));
		(*pos).setZ(multVectors(A[2], tv));

		//		std:: cout <<"valores de p:"<<pos.getX()<<", "<< pos.getY()<<", " <<pos.getZ() <<std::endl;

		// compute deriv = T' * A

		float tvd[4] = {3 * t * t, 2 * t, 1, 0};
		Point derivV;
		(*deriv).setX(multVectors(A[0], tvd));
		(*deriv).setY(multVectors(A[1], tvd));
		(*deriv).setZ(multVectors(A[2], tvd));

		// ...
	}

	// given  global t, returns the point in the curve
	void getGlobalCatmullRomPoint(float gt, Point *pos, Point *deriv)
	{
		int point_count = points.size();
		float t = gt * point_count; // this is the real global t
		int index = floor(t);		// which segment
		t = t - index;				// where within  the segment

		// indices store the points
		int indices[4];
		indices[0] = (index + point_count - 1) % point_count;
		indices[1] = (indices[0] + 1) % point_count;
		indices[2] = (indices[1] + 1) % point_count;
		indices[3] = (indices[2] + 1) % point_count;

		getCatmullRomPoint(t, points[indices[0]], points[indices[1]],
						   points[indices[2]], points[indices[3]], pos, deriv);
	}

	void renderCatmullRomCurve(Point pos, Point deriv)
	{

		// draw curve using line segments with GL_LINE_LOOP
		float gt = 0.0;
		int NUM_STEPS = 1000;
		float step = 1.0 / NUM_STEPS;
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < NUM_STEPS; i++)
		{
			getGlobalCatmullRomPoint(gt, &pos, &deriv);
			std::cout << "valores de p:" << pos.getX() << ", " << pos.getY() << ", " << pos.getZ() << std::endl;

			glVertex3f(pos.getX(), pos.getY(), pos.getZ());

			gt += step;
		}
		glEnd();
	}

	void apply()
	{
		if (type == "translate")
		{
			if (points.size() == 0)
			{
				glTranslatef(x, y, z);
			}
			else
			{
				float static t = 0;
				Point pos1;
				Point deriv1;
				float elapse = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
				float gt = elapse / time;
				renderCatmullRomCurve(pos1, deriv1);

				// apply transformations here

				Point pos2;
				Point deriv2;
				getGlobalCatmullRomPoint(gt, &pos2, &deriv2);
				glTranslatef(pos2.getX(), pos2.getY(), pos2.getZ());

				if (align)
				{

					float vecAux[3] = {0, 1, 0};
					float vecX[3] = {deriv2.getX(), deriv2.getY(), deriv2.getZ()};
					normalize(vecX);

					float vecZ[3];
					cross(vecX, vecAux, vecZ);
					normalize(vecZ);

					float vecY[3];
					cross(vecZ, vecX, vecY);
					normalize(vecY);

					float m[16];
					buildRotMatrix(vecX, vecY, vecZ, m);

					glMultMatrixf(m);
				}

				// t =+ 0.001;
			}
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
			iss >> descartavel; // Descarta o primeiro caracter
			iss.ignore();		// Ignora o ponto e virgula
			iss >> num1;		// Le o primeiro numero
			iss.ignore();		// Ignora o ponto e virgula
			iss >> num2;		// Le o segundo numero
			iss.ignore();		// Ignora o ponto e virgula
			iss >> num3;		// Le o terceiro numero
			vertexB.push_back(num1);
			vertexB.push_back(num2);
			vertexB.push_back(num3);
		}

		numVertices = vertexB.size() / 3;

		glBindBuffer(GL_ARRAY_BUFFER, bufferID);

		glBufferData(GL_ARRAY_BUFFER,				 // tipo do buffer, so e relevante na altura do desenho
					 sizeof(float) * vertexB.size(), // tamanho do vector em bytes
					 vertexB.data(),				 // os dados do array associado ao vector
					 GL_STATIC_DRAW);				 // indicativo da utilizacao (estatico e para desenho)
	}

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

void calculateFPS()
{

	frame++;
	float fps = 0.0f;
	int time = glutGet(GLUT_ELAPSED_TIME);
	std::cout << "Calculating fps!" << time - timebase << "\n";

	if (time - timebase > 1000)
	{

		fps = frame * 1000.0 / (time - timebase);
		timebase = time;
		frame = 0;

		// Update window title with FPS
		char title[50];
		sprintf(title, "%.2f FPS", fps);
		glutSetWindowTitle(title);
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
	glPolygonMode(GL_FRONT, mode);

	// desenhar models VBO's
	for (int i = 0; i < num_models; i++)
	{
		glPushMatrix();
		modelsArray[i].draw();
		glPopMatrix();
	}

	calculateFPS();
	// glutSetWindowTitle("frame");

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

	for (pugi::xml_node transformation : group.child("transform").children())
	{
		transVector.push_back(parseSingleTransformationNode(transformation));
	}

	for (pugi::xml_node model : group.child("models").children("model"))
	{
		std::string modelFile = model.attribute("file").as_string();

		modelsArray[num_models].setFilePath("models/" + modelFile); // linux

		modelsArray[num_models].setTransf(transVector);
		num_models++;
	}

	for (pugi::xml_node childGroup : group.children("group"))
	{
		std::vector<Transformation> newVec = transVector;
		readGroup(childGroup, newVec);
	}
}

void readConfig(const pugi::xml_node &world)
{

	pugi::xml_node window = world.child("window");
	WINDOW_WIDTH = window.attribute("width").as_int();
	WINDOW_HEIGHT = window.attribute("height").as_int();

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

	for (pugi::xml_node group : world.children("group"))
	{
		std::vector<Transformation> transVector;
		readGroup(group, transVector);
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

	const char *value = node.child_value();

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

	std::string xmlFilePath = CONFIGS_DIR + "/test_files_phase_3/" + "test_3_1.xml"; // linux com rotacoes

	// Load the XML file
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(xmlFilePath.c_str());

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
	glutIdleFunc(renderScene);
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
		modelsArray[i - 1].importModel(i);
	}
	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
