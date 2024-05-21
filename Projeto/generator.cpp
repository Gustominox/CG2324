#include <iostream>
#include <fstream>
#include <iomanip> 
#include <cmath>
#include <cstring>
#include <vector>

void multMatrixVector(float m[4][4], float v[4], float res[4], bool isRowVector) {
    if (isRowVector) {
        // Vector * Matrix (1xN * NxM)
        for (int i = 0; i < 4; ++i) {
            res[i] = 0;
            for (int j = 0; j < 4; ++j) {
                res[i] += v[j] * m[j][i];
            }
        }
    } else {
        // Matrix * Vector (NxM * Mx1)
        for (int i = 0; i < 4; ++i) {
            res[i] = 0;
            for (int j = 0; j < 4; ++j) {
                res[i] += m[i][j] * v[j];
            }
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

void cross(float *a, float *b, float *res) {

	res[0] = a[1]*b[2] - a[2]*b[1];
	res[1] = a[2]*b[0] - a[0]*b[2];
	res[2] = a[0]*b[1] - a[1]*b[0];
}


class Point
{
private:
	float x, y, z;

public:
	Point(float _x = 0.0, float _y = 0.0, float _z = 0.0) : x(_x), y(_y), z(_z) {}

	float getX() const { return x; }
	float getY() const { return y; }
	float getZ() const { return z; }

	void setX(float _x) { x = _x; }
	void setY(float _y) { y = _y; }
	void setZ(float _z) { z = _z; }
};


void normalize(Point a) {

	float l = sqrt(a.getX()*a.getX() + a.getY() * a.getY() + a.getZ() * a.getZ());
	a.setX(a.getX()/l);
	a.setY(a.getY()/l);
	a.setY(a.getZ()/l);
}

std::vector<std::vector<int>> vectors;
std::vector<Point> controlPoints;

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
                std::cerr << "N_Points: " << numPoints<< std::endl;
                std::cerr << "N_vectors: " << numVectors<< std::endl;
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

void write3D(std::vector<Point>& points, std::vector<Point>& normals, std::vector<Point>& textures, const std::string& filename) {
        
        std::ofstream outfile(filename);
        
        if (!outfile.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }
        
        outfile << std::fixed << std::setprecision(6);

        //escrever todos os pontos no ficheiro (1º pontos, 2º normais, 3º texturas)
        //outfile << "v;" << a.getX() << ";" << a.getY() << ";"  << a.getZ() << ";" << std::endl;;
        //outfile << "n;" << na.getX() << ";" << na.getY() << ";"  << na.getZ() << ";" << std::endl;;
        //outfile << "t;" << ta.getX() << ";" << ta.getY() << ";"  << ta.getZ() << ";" << std::endl;;

        for(size_t i = 0; i < points.size(); i++){
            Point a = points[i];
            outfile << "v;" << a.getX() << ";" << a.getY() << ";"  << a.getZ() << ";" << std::endl;;
        }

        for(size_t i = 0; i < normals.size(); i++){
            Point a = normals[i];
            outfile << "n;" << a.getX() << ";" << a.getY() << ";"  << a.getZ() << ";" << std::endl;;
        }

        for(size_t i = 0; i < textures.size(); i++){
            Point a = textures[i];
            outfile << "t;" << a.getX() << ";" << a.getY() << ";"  << a.getZ() << ";" << std::endl;;
        }

        outfile.close();
}


class Generator {
public:
    // Generate a plane model
    static void generatePlane(float size, int divisions, const std::string& filename) {

        std::vector<Point> Points;
        std::vector<Point> Normals;
        std::vector<Point> Textures;

        
        //tamanho dos catetos do triangulo
        float step = size / divisions;


        // x e z sao o numero de divisoes
        for (int z = 0; z < divisions; ++z) {
            for (int x = 0; x < divisions; ++x) {
                // Define vertices for each quad in the plane
                //quando o x,z=0 posx,posz= -size/2
                //isto é, comeca no eixo -x/-z
                float posX = x * step - size / 2;
                float posZ = z * step - size / 2;
                float x2 = posX + step;
                float z2 = posZ + step;
                
                // Vertices
                Point a = Point(posX, 0.0, posZ);
                Point b = Point(x2, 0.0, z2);
                Point c = Point(x2, 0.0, posZ);
                Point d = Point(posX, 0.0, z2);
                //Parte superior
                Points.push_back(a);
                Points.push_back(b);
                Points.push_back(c);
                Points.push_back(a);
                Points.push_back(d);
                Points.push_back(b);
                //Parte inferior
                Points.push_back(a);
                Points.push_back(c);
                Points.push_back(b);
                Points.push_back(a);
                Points.push_back(b);
                Points.push_back(d);

                //Normals
                Point na = Point(0.0, 1.0, 0.0);

                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);

                //Textures


            }
        }

        //funçao que recebe os 3 vetores o nome do ficheiro e escreve o ficheiro
        write3D(Points, Normals, Textures, filename);
        std::cout << "Plane model generated and saved to " << filename << std::endl;
    }

    // Generate a box model
    static void generateBox(float size, int divisions, const std::string& filename) {

        std::vector<Point> Points;
        std::vector<Point> Normals;
        std::vector<Point> Textures;

        //tamanho dos catetos do triangulo
        float step = size / divisions;

        float posInicial = -(size/2);

        //FACES ESCONDIDAS
        //face 1 y=-size/2 BASE - Certo
        for (int z = 0; z < divisions; ++z) {
            for (int x = 0; x < divisions; ++x) {
                // Define vertices for each quad in the plane
                //quando o x,z=0 posx,posz= -size/2
                //isto é, comeca no eixo -x/-z
                float posX = x * step - size / 2;
                float posZ = z * step - size / 2;
                float x2 = posX + step;
                float z2 = posZ + step;

                //Vertices
                Point a = Point(posX, posInicial, posZ);                
                Point b = Point(x2, posInicial, posZ);
                Point c = Point(x2, posInicial, z2);
                Point d = Point(posX, posInicial, z2);      
                
                Points.push_back(a);
                Points.push_back(b);
                Points.push_back(c);
                Points.push_back(a);                
                Points.push_back(c);                          
                Points.push_back(d);

                //Normals
                Point na = Point(0.0, -1.0, 0.0);

                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);

                //Textures


            }
        }

        //face 2 x= -size/2 - Certo
        for (int z = 0; z < divisions; ++z) {
            for (int y = 0; y < divisions; ++y) {
                // Define vertices for each quad in the plane
                //quando o x,z=0 posx,posz= -size/2
                //isto é, comeca no eixo -x/-z
                float posY = y * step - size / 2;
                float posZ = z * step - size / 2;
                float y2 = posY + step;
                float z2 = posZ + step;

                //Vetices
                Point a = Point(posInicial, posY, posZ);
                Point b = Point(posInicial, y2, z2);
                Point c = Point(posInicial, y2, posZ);
                Point d = Point(posInicial, posY, z2);

                Points.push_back(a);
                Points.push_back(b);
                Points.push_back(c);
                Points.push_back(a);
                Points.push_back(d);
                Points.push_back(b);

                //Normals
                Point na = Point(-1.0, 0.0, 0.0);

                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);

                //Textures

            }
        }

        //face 3 z= -size/2
        for (int y = 0; y < divisions; ++y) {
            for (int x = 0; x < divisions; ++x) {
                // Define vertices for each quad in the plane
                //quando o x,z=0 posx,posz= -size/2
                //isto é, comeca no eixo -x/-z
                float posX = x * step - size / 2;
                float posY = y * step - size / 2;
                float x2 = posX + step;
                float y2 = posY + step;

                //Vertices
                Point a = Point(posX, posY, posInicial);
                Point b = Point(x2, y2, posInicial);
                Point c = Point(x2, posY, posInicial);
                Point d = Point(posX, y2, posInicial);

                Points.push_back(a);
                Points.push_back(b);
                Points.push_back(c);
                Points.push_back(a);
                Points.push_back(d);
                Points.push_back(b);

                //Normals
                Point na = Point(0.0, 0.0, -1.0);

                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);

                //Textures
            }
        }


        //FACES À MOSTRA
        //face 4 y = size/2 TOPO
        posInicial = posInicial*-1;
        for (int z = 0; z < divisions; ++z) {
            for (int x = 0; x < divisions; ++x) {
                // Define vertices for each quad in the plane
                //quando o x,z=0 posx,posz= -size/2
                //isto é, comeca no eixo -x/-z
                float posX = x * step - size / 2;
                float posZ = z * step - size / 2;
                float x2 = posX + step;
                float z2 = posZ + step;

                //Vertices
                Point a = Point(posX, posInicial, posZ);
                Point b = Point(x2, posInicial, z2);
                Point c = Point(x2, posInicial, posZ);
                Point d = Point(posX, posInicial, z2);

                Points.push_back(a);
                Points.push_back(b);
                Points.push_back(c);
                Points.push_back(a);
                Points.push_back(d);
                Points.push_back(b);

                //Normals
                Point na = Point(0.0, 1.0, 0.0);

                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);

                //Textures

            }
        }



        //face 5 x= size/2
        for (int z = 0; z < divisions; ++z) {
            for (int y = 0; y < divisions; ++y) {
                // Define vertices for each quad in the plane
                //quando o x,z=0 posx,posz= -size/2
                //isto é, comeca no eixo -x/-z
                float posY = y * step - size / 2;
                float posZ = z * step - size / 2;
                float y2 = posY + step;
                float z2 = posZ + step;

                //Vertices
                Point a = Point(posInicial, posY, posZ);
                Point b = Point(posInicial, y2, posZ);
                Point c = Point(posInicial, y2, z2);
                Point d = Point(posInicial, posY, z2);

                Points.push_back(a);
                Points.push_back(b);
                Points.push_back(c);
                Points.push_back(a);
                Points.push_back(c);
                Points.push_back(d);
                
                //Normals
                Point na = Point(1.0, 0.0, 0.0);

                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);

                //Textures
            }
        }


        //face 6 z= size/2
        for (int y = 0; y < divisions; ++y) {
            for (int x = 0; x < divisions; ++x) {
                // Define vertices for each quad in the plane
                //quando o x,z=0 posx,posz= -size/2
                //isto é, comeca no eixo -x/-z
                float posX = x * step - size / 2;
                float posY = y * step - size / 2;
                float x2 = posX + step;
                float y2 = posY + step;

                //Vertices
                Point a = Point(posX, posY, posInicial);
                Point b = Point(x2, posY, posInicial);
                Point c = Point(x2, y2, posInicial);
                Point d = Point(posX, y2, posInicial);        

                Points.push_back(a);
                Points.push_back(b);
                Points.push_back(c);
                Points.push_back(a);
                Points.push_back(c);
                Points.push_back(d);

                //Normals
                Point na = Point(0.0, 0.0, 1.0);

                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);
                Normals.push_back(na);   

                //Textures         
            }
        }


        
        write3D(Points, Normals, Textures, filename);
        std::cout << "Box model generated and saved to " << filename << std::endl;
    }

    static void generateCone(float radius, float height, int slices, int stacks, const std::string& filename){

        std::vector<Point> Points;
        std::vector<Point> Normals;
        std::vector<Point> Textures;

        //altura de cada stack
        float alt = height/stacks;

        //diferenca entre os raios de diferentes niveis
        float r_step = radius/stacks;

        //stacks
        for(int j = 0; j < stacks; j++){
            //calculo dos y's da stack
            float y0 = j * alt;
            float y1 = (j+1) * alt;

            //slices
            for(int i = 0; i < slices; i++){
                //angulo para "desenhar" a "base/topo" da stack
                float alpha = i * (2*M_PI / slices);
                float alpha2 = (i+1) * (2*M_PI / slices);

			    //pontos inferiores do trapezio
			    float radius1 = radius -(j*r_step);
			    float x1 = radius1*sin(alpha);
			    float z1 = radius1*cos(alpha);

			    float x2 = radius1*sin(alpha2);
			    float z2 = radius1*cos(alpha2);

			    //pontos superiores do trapezio
			    float radius2 = radius - ((j+1)*(r_step));
			    float x3 = radius2*sin(alpha);
			    float z3 = radius2*cos(alpha);

			    float x4 = radius2*sin(alpha2);
			    float z4 = radius2*cos(alpha2);

                //base
                if(j == 0){
                    //Vertices
                    Point b0 = Point(0.0, 0.0, 0.0);
                    Point b1 = Point(x2, y0, z2);
                    Point b2 = Point(x1, y0, z1);

                    Points.push_back(b0);
                    Points.push_back(b1);
                    Points.push_back(b2);

                    //Normals
                    Point nb0 = Point(0.0, -1.0, 0.0);

                    Normals.push_back(nb0);            
                    Normals.push_back(nb0);
                    Normals.push_back(nb0);

                    //Textures

                }

                //triangulo de baixo do trapezio
                //Vertice
                Point a = Point(x1, y0, z1);
                Point b = Point(x2, y0, z3);
                Point c = Point(x3, y1, z3);      

                Points.push_back(a);    
                Points.push_back(b);                    
                Points.push_back(c);   

                //Normals
                normalize(a);
                normalize(b);
                normalize(c);

                Normals.push_back(a);
                Normals.push_back(b);
                Normals.push_back(c);

                //Textures
                
                //triangulo de cima do trapezio
                if(j != stacks-1){
                    //Vertices
                    Point d = Point(x2, y0, z2);
                    Point e = Point(x4, y1, z4);
                    Point f = Point(x3, y1, z3);

                    Points.push_back(d);
                    Points.push_back(e);
                    Points.push_back(f);

                    //Normals
                    normalize(d);
                    normalize(e);
                    normalize(f);

                    Normals.push_back(d);
                    Normals.push_back(e);
                    Normals.push_back(f);

                    //Textures

                }
                
            }
        }
        write3D(Points, Normals, Textures, filename);
        std::cout << "Cone model generated and saved to " << filename << std::endl;
    }

    static void generateSphere(float radius, int slices, int stacks, const std::string& filename){
        
        std::vector<Point> Points;
        std::vector<Point> Normals;
        std::vector<Point> Textures;
          

        for (int i = 0; i < stacks; ++i) {
        float beta = i * (M_PI / stacks);
        float beta2 = (i + 1) * (M_PI / stacks);

            for (int j = 0; j < slices; ++j) {
                float alpha = j * (2*M_PI / stacks);
                float alpha2 = (j + 1) * (2*M_PI / stacks);

                // Vertices
		    	//comeca pelo ponto (0,r,0), cos(0)=1
                float x0 = radius * sin(alpha) * sin(beta);
                float y0 = radius * cos(beta);
                float z0 = radius * cos(alpha) * sin(beta);

                float x1 = radius * sin(alpha2) * sin(beta);
                float y1 = radius * cos(beta);
                float z1 = radius * cos(alpha2) * sin(beta);

                float x2 = radius * sin(alpha) * sin(beta2);
                float y2 = radius * cos(beta2);
                float z2 = radius * cos(alpha) * sin(beta2);

                float x3 = radius * sin(alpha2) * sin(beta2);
                float y3 = radius * cos(beta2);
                float z3 = radius * cos(alpha2) * sin(beta2);

		    	if(i !=  0){
                    //Vertices
                    Point a = Point(x0, y0, z0);
                    Point b = Point(x2, y2, z2);
                    Point c = Point(x1, y1, z1);

                    Points.push_back(a);
                    Points.push_back(b);
                    Points.push_back(c);

                    //Normals
                    normalize(a);
                    normalize(b);
                    normalize(c);
                    
                    Normals.push_back(a);
                    Normals.push_back(b);
                    Normals.push_back(c);                 

                    //Textures
		    	}

		    	if(i != stacks-1){
                    //Vertices
                    Point d = Point(x1, y1, z1);
                    Point e = Point(x1, y1, z1);
                    Point f = Point(x1, y1, z1);

                    Points.push_back(d);
                    Points.push_back(e);
                    Points.push_back(f);

                    //Normals
                    normalize(d);
                    normalize(e);
                    normalize(f);

                    Normals.push_back(d);
                    Normals.push_back(e);
                    Normals.push_back(f);                    
                    
                    //Textures
		    	}
            }
        }
        write3D(Points, Normals, Textures, filename);
        std::cout << "Sphere model generated and saved to " << filename << std::endl;
    
    }


    
    static void surfPoint(float u, float v, std::vector<int> i, Point a, Point normal){ //verificar se as contas se fazem bem
    	float m[4][4] = {{-1.0f,  3.0f, -3.0f, 1.0f},
    				 {3.0f, -6.0f,  3.0f, 0.0f},
    				 {-3.0f,  3.0f,  0.0f, 0.0f},
    				 {1.0f,  0.0f,  0.0f, 0.0f}};

    	float uv[4] = {u*u*u,u*u,u,1.0f}; //1*4 vetor u
    	float vv[4] = {v*v*v,v*v,v,1.0f}; //4*1 vetor v
        float der_uv[4] = {3.0f*u*u, 2.0f*u, 1.0f, 0.0f}; // 1*4 derivada do vetor u (u')
        float der_vv[4] = {3.0f*v*v, 2.0f*v, 1.0f, 0.0f}; // 4*1 derivada do vetor v (v')

    	float um[4];
    	float vm[4];
    	multMatrixVector(m, uv, um, true); 	// uv * M = 1 * 4
    	multMatrixVector(m, vv, vm, false); // M * vv = 4 * 1

        float dum[4];
        float dvm[4];

        multMatrixVector(m, der_uv, dum, true);  // u' * M = 1 * 4
        multMatrixVector(m, der_vv, dvm, false); // M * v' = 4 * 1 

    	//matrix dos X's
    	float mx[4][4] = {	{controlPoints[i[0]].getX(),  controlPoints[i[1]].getX(),  controlPoints[i[2]].getX(),  controlPoints[i[3]].getX()},
    				 		{controlPoints[i[4]].getX(),  controlPoints[i[5]].getX(),  controlPoints[i[6]].getX(),  controlPoints[i[7]].getX()},
    				 		{controlPoints[i[8]].getX(),  controlPoints[i[9]].getX(),  controlPoints[i[10]].getX(), controlPoints[i[11]].getX()},
    				 		{controlPoints[i[12]].getX(), controlPoints[i[13]].getX(), controlPoints[i[14]].getX(), controlPoints[i[15]].getX()}};

    	//matrix dos Y's
    	float my[4][4] = {	{controlPoints[i[0]].getY(),  controlPoints[i[1]].getY(),  controlPoints[i[2]].getY(),  controlPoints[i[3]].getY()},
    				 		{controlPoints[i[4]].getY(),  controlPoints[i[5]].getY(),  controlPoints[i[6]].getY(),  controlPoints[i[7]].getY()},
    				 		{controlPoints[i[8]].getY(),  controlPoints[i[9]].getY(),  controlPoints[i[10]].getY(), controlPoints[i[11]].getY()},
    				 		{controlPoints[i[12]].getY(), controlPoints[i[13]].getY(), controlPoints[i[14]].getY(), controlPoints[i[15]].getY()}};

    	//matrix dos Z's
    	float mz[4][4] = {	{controlPoints[i[0]].getZ(),  controlPoints[i[1]].getZ(),  controlPoints[i[2]].getZ(),  controlPoints[i[3]].getZ()},
    				 		{controlPoints[i[4]].getZ(),  controlPoints[i[5]].getZ(),  controlPoints[i[6]].getZ(),  controlPoints[i[7]].getZ()},
    				 		{controlPoints[i[8]].getZ(),  controlPoints[i[9]].getZ(),  controlPoints[i[10]].getZ(), controlPoints[i[11]].getZ()},
    				 		{controlPoints[i[12]].getZ(), controlPoints[i[13]].getZ(), controlPoints[i[14]].getZ(), controlPoints[i[15]].getZ()}};

    	
        //Vertice
        float umpx[4];
    	float umpy[4];
    	float umpz[4];

    	multMatrixVector(mx, um, umpx, true); // um * mx = 1*4 
    	multMatrixVector(my, um, umpy, true); // um * my = 1*4
    	multMatrixVector(mz, um, umpz, true); // um * mz = 1*4

    	a.setX(multVectors(umpx, vm));
    	a.setY(multVectors(umpy, vm));
    	a.setZ(multVectors(umpz, vm));

        //Normal
        float Du[3]; //derivada parcial u
        float Dv[3]; //derivada parcial v

        float Dumpx[4];
        float Dumpy[4];
        float Dumpz[4];
        //derivada parcial u
        // U' * M * P
        multMatrixVector(mx, dum, Dumpx, true);
        multMatrixVector(my, dum, Dumpy, true);
        multMatrixVector(mz, dum, Dumpz, true);
        Du[0] = multVectors(Dumpx,vm);
        Du[1] = multVectors(Dumpy,vm);
        Du[2] = multVectors(Dumpz,vm);
        
        //derivada parcial v
        multMatrixVector(mx, um, umpx, true);
        multMatrixVector(my, um, umpy, true);
        multMatrixVector(mz, um, umpz, true);
        Dv[0] = multVectors(umpx, dvm);
        Dv[1] = multVectors(umpy, dvm);
        Dv[2] = multVectors(umpz, dvm);
        
        float n[3];
        cross(Dv, Du, n);

        normal.setX(n[0]);
        normal.setY(n[1]);
        normal.setZ(n[2]);

        //Textures

    }


    static void generateBezier(const std::string& patchfile, int tessellation, const std::string& filename){
        //abre e le o patchfile
        parseFile(patchfile, vectors, controlPoints);

        std::vector<Point> Points;
        std::vector<Point> Normals;
        std::vector<Point> Textures;

        Point a, b, c, d;
        Point na, nb, nc, nd;
        Point ta, tb, tc, td;

	    float u = 0.0f, v = 0.0f, delta = 1.0f/tessellation;
	    for(int k = 0; k<vectors.size(); k++){ //percorrer cada vetor de indices
	    	for(int i = 0; i < tessellation; i++, u+=delta){
	    		for(int j = 0; j < tessellation; j++, v+=delta){
                
	    			//CALCULO DE PONTOS
	    			surfPoint(u		 , v	  , vectors[k], a, na);
	    			surfPoint(u		 , v+delta, vectors[k], b, nb);
	    			surfPoint(u+delta, v	  , vectors[k], c, nc);
	    			surfPoint(u+delta, v+delta, vectors[k], d, nd);
    
                    
                    //Triangulaçao
                    //Vertices
                    Points.push_back(c);
                    Points.push_back(a);
                    Points.push_back(b);
                    Points.push_back(b);
                    Points.push_back(d);
                    Points.push_back(c);
                    
                    //Normals
                    Normals.push_back(nc);
                    Normals.push_back(na);
                    Normals.push_back(nb);
                    Normals.push_back(nb);
                    Normals.push_back(nd);
                    Normals.push_back(nc);

    
	    		}
	    		v = 0.0f;
	    	}
	    	u = v = 0.0f;
	    } 
        write3D(Points, Normals, Textures, filename);
        std::cout << "Patch model generated and saved to " << filename << std::endl;
    }


};

int main(int argc, char* argv[]) {
    // Generate models
    /*
    Generator::generatePlane(2.0f, 10, "plane.3d");
    Generator::generateBox(2.0f, 2, "box.3d");
    Generator::generateCone(1.0f, 2.0f, 4, 3, "cone.3d");
    Generator::generateSphere(1.0f, 10, 10, "sphere.3d"); */
    if (argc == 5) {
        if (strcmp(argv[1], "plane") == 0) {
            Generator::generatePlane(std::stof(argv[2]), std::stoi(argv[3]), argv[4]);
        }
        if (strcmp(argv[1], "box") == 0) {
            Generator::generateBox(std::stof(argv[2]), std::stoi(argv[3]), argv[4]);
        }
        if (strcmp(argv[1], "patch") == 0) {
            Generator::generateBezier(argv[2], std::stoi(argv[3]), argv[4]);
        }
        
    }
    if (argc == 6) {
        if (strcmp(argv[1], "sphere") == 0) {
            Generator::generateSphere(std::stof(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]), argv[5]);
        }
    }
    if (argc == 7) {
        if (strcmp(argv[1], "cone") == 0) {
            Generator::generateCone(std::stof(argv[2]), std::stof(argv[3]), std::stoi(argv[4]),
                                      std::stoi(argv[5]), argv[6]);
        }
    }

    return 0;
}
