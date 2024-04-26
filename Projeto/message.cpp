#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

float camX = 0, camY, camZ = 5;
int startX, startY, tracking = 0;
float pos[3];
float deriv[3];
int alpha = 0, beta = 0, r = 5;
float t = 0;





#define POINT_COUNT 5
// Points that make up the loop for catmull-rom interpolation


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
				//std:: cout <<"valores de p:"<<pos.getX()<<", "<< pos.getY()<<", " <<pos.getZ() <<std::endl;

				glVertex3f(pos.getX(), pos.getY(), pos.getZ());

				gt += step;
			}
			glEnd();
		}


	void apply() 
	{
		if (type == "translate")
		{	
			if(points.size() == 0){
				glTranslatef(x, y, z);
			}
			else{
				float static t = 0;
				Point pos1;
				Point deriv1;
				float elapse = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
				float gt = elapse/time;
				renderCatmullRomCurve(pos1, deriv1);

				// apply transformations here
				
				Point pos2;
				Point deriv2;
				getGlobalCatmullRomPoint(gt, &pos2, &deriv2);
				glTranslatef(pos2.getX(), pos2.getY(), pos2.getZ());

				if(align){

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


				//t =+ 0.001;

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


std::vector<std::vector<int>> vectors;
std::vector<Point> pointsp;

std::vector<std::vector<Point>> curves;
std::vector<std::vector<Point>> sup;

Point getBezierPoint(float t, Point p0, Point p1, Point p2, Point p3)
{
	// catmull-rom matrix
	float m[4][4] = {{-1.0f, 3.0f, -3.0f, 1.0f},
					 {3.0f, -6.0f, 3.0f, 0.0f},
					 {-3.0f, 3.0f, 0.0f, 0.0f},
					 {1.0f, 0.0f, 0.0f, 0.0f}};
	// Compute A = M * P
	float vecX[4] = {p0.getX(), p1.getX(), p2.getX(), p3.getX()}; // x
	float vecY[4] = {p0.getY(), p1.getY(), p2.getY(), p3.getY()}; // y
	float vecZ[4] = {p0.getZ(), p1.getZ(), p2.getZ(), p3.getZ()}; // z
	float A[3][4];

	multMatrixVector(*m, vecX, A[0]);
	multMatrixVector(*m, vecY, A[1]);
	multMatrixVector(*m, vecZ, A[2]);
	
	Point pos;
	// Compute pos = T * A
	float tv[4] = {t * t * t, t * t, t, 1};
	pos.setX(multVectors(A[0], tv));
	pos.setY(multVectors(A[1], tv));
	pos.setZ(multVectors(A[2], tv));
	//std:: cout <<"valores de p:"<<pos.getX()<<", "<< pos.getY()<<", " <<pos.getZ() <<std::endl;

	return pos;
	// ...
}



	// given  global t, returns the point in the curve
	Point getGlobalBezierPointCurve(float gt, int i)
	{
		int point_count = vectors[i].size();
		float t = gt * point_count; // this is the real global t
		int index = floor(t);		// which segment
		t = t - index;				// where within  the segment

		// indices store the points
		int indices[4];
		indices[0] = (index + point_count - 1) % point_count;
		indices[1] = (indices[0] + 1) % point_count;
		indices[2] = (indices[1] + 1) % point_count;
		indices[3] = (indices[2] + 1) % point_count;
		
		Point p0 = pointsp[vectors[i][indices[0]]];
		Point p1 = pointsp[vectors[i][indices[1]]];
		Point p2 = pointsp[vectors[i][indices[2]]];
		Point p3 = pointsp[vectors[i][indices[3]]];
		//std:: cout <<"indices:"<<indices[0]<<", "<< indices[1]<<", " <<indices[2]<<", " <<indices[3] <<std::endl;

		//std:: cout <<"valores de p0:"<<p0.getX()<<", "<< p0.getY()<<", " <<p0.getZ() <<std::endl;
		//std:: cout <<"valores de p1:"<<p1.getX()<<", "<< p1.getY()<<", " <<p1.getZ() <<std::endl;
		//std:: cout <<"valores de p2:"<<p2.getX()<<", "<< p2.getY()<<", " <<p2.getZ() <<std::endl;
		//std:: cout <<"valores de p3:"<<p3.getX()<<", "<< p3.getY()<<", " <<p3.getZ() <<std::endl;


		return (getBezierPoint(t, p0, p1, p2, p3));
	}

		// given  global t, returns the point in the curve
	Point getGlobalBezierPointSup(float gt, int i)
	{
		int point_count = curves.size();
		float t = gt * point_count; // this is the real global t
		int index = floor(t);		// which segment
		t = t - index;				// where within  the segment

		// indices store the points
		int indices[4];
		indices[0] = (index + point_count - 1) % point_count;
		indices[1] = (indices[0] + 1) % point_count;
		indices[2] = (indices[1] + 1) % point_count;
		indices[3] = (indices[2] + 1) % point_count;
		
		Point p0 = pointsp[vectors[indices[0]][i]];
		Point p1 = pointsp[vectors[indices[1]][i]];
		Point p2 = pointsp[vectors[indices[2]][i]];
		Point p3 = pointsp[vectors[indices[3]][i]];

		std:: cout <<"indices:"<<indices[0]<<", "<< indices[1]<<", " <<indices[2]<<", " <<indices[3] <<std::endl;
//
		//std:: cout <<"valores de p0:"<<p0.getX()<<", "<< p0.getY()<<", " <<p0.getZ() <<std::endl;
		//std:: cout <<"valores de p1:"<<p1.getX()<<", "<< p1.getY()<<", " <<p1.getZ() <<std::endl;
		//std:: cout <<"valores de p2:"<<p2.getX()<<", "<< p2.getY()<<", " <<p2.getZ() <<std::endl;
		//std:: cout <<"valores de p3:"<<p3.getX()<<", "<< p3.getY()<<", " <<p3.getZ() <<std::endl;


		return (getBezierPoint(t, p0, p1, p2, p3));
	}

	void renderBezierCurve(int tessellation)
		{
			// draw curve using line segments with GL_LINE_LOOP
			float gt = 0.0;
			int NUM_STEPS = tessellation;
			float step = 1.0 / (NUM_STEPS);
			curves.resize(vectors.size());

			for (int j = 0; j < vectors.size(); j++){

				for (int i = 0; i < NUM_STEPS; i++)
				{
					gt = i * step;

					Point pos = getGlobalBezierPointCurve(gt, j);
					//std:: cout <<"i:"<<i<<std::endl;
					//glBegin(GL_POINTS);
            		//glVertex3f(pos.getX(), pos.getY(), pos.getZ());
            		//glEnd();
					//std:: cout <<"valores de p:"<<pos.getX()<<", "<< pos.getY()<<", " <<pos.getZ() <<std::endl;

					//glVertex3f(pos.getX(), pos.getY(), pos.getZ());
					curves[j].push_back(pos);
					gt+=step;
				}
				//std:: cout <<"j:"<<j<<std::endl;
			}
		}

	void renderBezierSup(int tessellation){

		renderBezierCurve(tessellation);
		
		float gt = 0.0;
		int NUM_STEPS = tessellation;
		float step = 1.0 / (NUM_STEPS);
		sup.resize(NUM_STEPS);

			for (int j = 0; j < NUM_STEPS; j++){
				sup[j].resize(NUM_STEPS);
				std:: cout <<"j:"<<j<<std::endl;
				for (int i = 0; i < NUM_STEPS; i++)
				{	
					
					gt = i * step;
					Point pos = getGlobalBezierPointSup(gt, i);
					//std:: cout <<"i:"<<i<<std::endl;
            		glBegin(GL_POINTS);
            		glVertex3f(pos.getX(), pos.getY(), pos.getZ());
            		glEnd();
					//std:: cout <<"valores de p:"<<pos.getX()<<", "<< pos.getY()<<", " <<pos.getZ() <<std::endl;

					//glVertex3f(pos.getX(), pos.getY(), pos.getZ());
					sup[i].push_back(pos);
					gt+=step;
				}
				//std:: cout <<"j:"<<j<<std::endl;
			}
		

	}

	void drawSup(int tesselation){

		renderBezierCurve(tesselation);

		glBegin(GL_TRIANGLES);
		for (int i = 0; i < curves.size()-1; i++){
			for(int j = 0; j < curves[i].size()-1; j++){
				glVertex3f(curves[i][j].getX(),     curves[i][j].getY(),     curves[i][j].getZ());
				glVertex3f(curves[i][j+1].getX(),   curves[i][j+1].getY(),   curves[i][j+1].getZ());
				glVertex3f(curves[i+1][j].getX(),   curves[i+1][j].getY(),   curves[i+1][j].getZ());
				glVertex3f(curves[i][j+1].getX(),   curves[i][j+1].getY(),   curves[i][j+1].getZ());
				glVertex3f(curves[i+1][j].getX(),   curves[i+1][j].getY(),   curves[i+1][j].getZ());
				glVertex3f(curves[i+1][j+1].getX(), curves[i+1][j+1].getY(), curves[i+1][j+1].getZ());
				
			}
		}
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

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45, ratio, 1, 1000);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}



void renderScene(void)
{

	static float t = 0;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
			  0.0, 0.0, 0.0,
			  0.0f, 1.0f, 0.0f);

	//renderCatmullRomCurve();
	//// apply transformations here
	//getGlobalCatmullRomPoint(t, pos, deriv);
	//glTranslatef(pos[0], pos[1], pos[2]);
//
	//float aux[3] = {0, 1, 0};
	//float vx[3] = {deriv[0], deriv[1], deriv[2]};
	//normalize(vx);
//
	//float vz[3];
	//cross(vx, aux, vz);
	//normalize(vz);
//
	//float vy[3];
	//cross(vz, vx, vy);
	//normalize(vy);
//
	//float m[16];
	//buildRotMatrix(vx, vy, vz, m);
//
	//glMultMatrixf(m);

	// ...
	//Transformation trans;
//
	//trans.setType("translate");
	//trans.setTime(15.0);
	//trans.setAlign(true);
	//trans.addPoint(Point(0.0, 0.0, 4.0));
	//trans.addPoint(Point(4.0, 0.0, 0.0));
	//trans.addPoint(Point(0.0, 0.0, -4.0));
	//trans.addPoint(Point(-4.0, 10.0, 0.0));

	//trans.apply();
	//renderBezierCurve(20);
	renderBezierSup(20);

	//drawSup(20);

	//glutWireTeapot(0.1);

	glutSwapBuffers();

}

void processMouseButtons(int button, int state, int xx, int yy)
{
	if (state == GLUT_DOWN)
	{
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP)
	{
		if (tracking == 1)
		{
			alpha += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2)
		{

			r -= yy - startY;
			if (r < 3)
				r = 3.0;
		}
		tracking = 0;
	}
}

void processMouseMotion(int xx, int yy)
{
	int deltaX, deltaY;
	int alphaAux, betaAux;
	int rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1)
	{

		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = r;
	}
	else if (tracking == 2)
	{

		alphaAux = alpha;
		betaAux = beta;
		rAux = r - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camY = rAux * sin(betaAux * 3.14 / 180.0);
}


// Function to parse the file
bool parseFile(const std::string& filename, std::vector<std::vector<int>>& vectors, std::vector<Point>& points) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return false;
    }

    std::string line;
    // Parse vectors
    int numVectors;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        if (!(iss >> numVectors)) {
            std::cerr << "Error parsing number of vectors" << std::endl;
            return false;
        }
        vectors.resize(numVectors);
        for (int i = 0; i < numVectors; ++i) {
            if (!std::getline(file, line)) {
                std::cerr << "Error reading vector " << i + 1 << std::endl;
                return false;
            }
            std::istringstream issVec(line);
            float value;
            while (issVec >> value) {
                vectors[i].push_back(value);
                if (issVec.peek() == ',')
                    issVec.ignore();
            }
        }
    } else {
        std::cerr << "Error reading file" << std::endl;
        return false;
    }

    // Parse points
    int numPoints;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        if (!(iss >> numPoints)) {
            std::cerr << "Error parsing number of points" << std::endl;
            return false;
        }
        points.resize(numPoints);
        for (int i = 0; i < numPoints; ++i) {
            if (!std::getline(file, line)) {
                std::cerr << "Error reading point " << i + 1 << std::endl;
                return false;
            }
            std::istringstream issPoint(line);
            double x, y, z;
            char comma;
            if (!(issPoint >> x >> comma >> y >> comma >> z)) {
                std::cerr << "Error parsing point " << i + 1 << std::endl;
                return false;
            }
            points[i] = Point(x, y, z);
        }
    } else {
        std::cerr << "Error reading file" << std::endl;
        return false;
    }

    return true;
}



int main(int argc, char **argv)
{


	std::string filename = "teapot.patch"; // Change to your file name
	if (parseFile(filename, vectors, pointsp)) {
 	       std::cout << "Vectors:" << vectors.size() << std::endl;
 	       for (const auto& vec : vectors) {
 	           for (float val : vec) {
 	               std::cout << val << " ";
 	           }
 	           std::cout <<" Size:"<< vec.size() << std::endl;
 	       }
 	       std::cout << "Points:" << pointsp.size() << std::endl;
 	       for (const auto& point : pointsp) {
 	           std::cout << "(" << point.getX() << ", " << point.getY() << ", " << point.getZ() << ")" << std::endl;
 	       }
 	   } else {
 	       std::cerr << "Failed to parse file." << std::endl;
 	   }
	// inicialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("CG@DI-UM");

	// callback registration
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	// mouse callbacks
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	// OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// enter GLUT's main cycle
	glutMainLoop();

	return 1;
}
