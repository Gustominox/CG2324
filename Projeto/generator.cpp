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

class Generator {
public:
    // Generate a plane model
    static void generatePlane(float size, int divisions, const std::string& filename) {
        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        // Set precision to 6 decimals
        outfile << std::fixed << std::setprecision(6);
        
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

                outfile << "v;" << posX << ";0.000000;" << posZ << ";" << std::endl;;
                outfile << "v;" << x2 << ";0.000000;" << z2 << ";" << std::endl;;
                outfile << "v;" << x2 << ";0.000000;" << posZ << ";" << std::endl;;

                outfile << "v;" << posX << ";0.000000;" << posZ << ";" << std::endl;;
                outfile << "v;" << posX << ";0.000000;" << z2 << ";" << std::endl;;
                outfile << "v;" << x2 << ";0.000000;" << z2 << ";" << std::endl;;
            }
        }
        outfile.close();
        std::cout << "Plane model generated and saved to " << filename << std::endl;
    }

    // Generate a box model
    static void generateBox(float size, int divisions, const std::string& filename) {

        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        // Set precision to 6 decimals
        outfile << std::fixed << std::setprecision(6);

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

                outfile << "v;" << posX << ";" <<posInicial<< ";" << posZ << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << posZ << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << z2 << ";" << std::endl;;

                outfile << "v;" << posX << ";" <<posInicial<< ";"  << posZ << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << z2 << std::endl;
                outfile << "v;" << posX << ";" <<posInicial<< ";"  << z2 << ";" << std::endl;;

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

                outfile << "v;" << posInicial << ";" <<posY<< ";" << posZ << ";" << std::endl;;
                outfile << "v;" << posInicial << ";" <<y2<< ";"  << z2 << ";" << std::endl;;
                outfile << "v;" << posInicial << ";" <<y2<< ";"  << posZ << ";" << std::endl;;


                outfile << "v;" << posInicial << ";" <<posY<< ";"  << posZ << ";" << std::endl;;
                outfile << "v;" << posInicial<< ";" <<posY<< ";"  << z2 << ";" << std::endl;;
                outfile << "v;" << posInicial<< ";" <<y2<< ";"  << z2 << ";" << std::endl;;

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

                outfile << "v;" << posX << ";" <<posY<< ";" << posInicial << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<y2<< ";"  << posInicial << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posY<< ";"  << posInicial << ";" << std::endl;;

                outfile << "v;" << posX << ";" <<posY<< ";"  << posInicial << ";" << std::endl;;
                outfile << "v;" << posX << ";" <<y2<< ";"  << posInicial << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<y2<< ";"  << posInicial << ";" << std::endl;;
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

                outfile << "v;" << posX << ";" <<posInicial<< ";"  << posZ << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << z2 << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << posZ << ";" << std::endl;;

                outfile << "v;" << posX << ";" <<posInicial<< ";"  << posZ << ";" << std::endl;;
                outfile << "v;" << posX << ";" <<posInicial<< ";"  << z2 << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << z2 << ";" << std::endl;;
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

                outfile << "v;" << posInicial << ";" <<posY<< ";" << posZ << ";" << std::endl;;
                outfile << "v;" << posInicial << ";" <<y2<< ";"  << posZ << ";" << std::endl;;
                outfile << "v;" << posInicial << ";" <<y2<< ";"  << z2 << ";" << std::endl;;


                outfile << "v;" << posInicial<< ";" <<posY<< ";"  << posZ << ";" << std::endl;;
                outfile << "v;" << posInicial<< ";" <<y2<< ";"  << z2 << ";" << std::endl;;
                outfile << "v;" << posInicial<< ";" <<posY<< ";"  << z2 << ";" << std::endl;;

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

                outfile << "v;" << posX << ";" <<posY<< ";" << posInicial << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posY<< ";"  << posInicial << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<y2<< ";"  << posInicial << ";" << std::endl;;

                outfile << "v;" << posX << ";" <<posY<< ";"  << posInicial << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<y2<< ";"  << posInicial << ";" << std::endl;;
                outfile << "v;" << posX << ";" <<y2<< ";"  << posInicial << ";" << std::endl;;
            }
        }


        
        outfile.close();
        std::cout << "Box model generated and saved to " << filename << std::endl;
    }

    static void generateCone(float radius, float height, int slices, int stacks, const std::string& filename){
        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        // Set precision to 6 decimals
        outfile << std::fixed << std::setprecision(6);  

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
                    outfile << "v;" << "0.000000" << ";0.000000;" << "0.000000" << ";" << std::endl;;
                    outfile << "v;" << x2 << ";" <<y0<< ";"  << z2 << ";" << std::endl;;
                    outfile << "v;" << x1 << ";" <<y0<< ";"  << z1 << ";" << std::endl;;
                }

                //triangulo de baixo do trapezio
                outfile << "v;" << x1 << ";" <<y0<<  ";" << z1 << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<y0<< ";"  << z2 << ";" << std::endl;;
                outfile << "v;" << x3 << ";" <<y1<< ";"  << z3 << ";" << std::endl;;
                
                //triangulo de cima do trapezio
                if(j != stacks-1){
                    outfile << "v;" << x2 << ";" <<y0<<  ";" << z2 << ";" << std::endl;;
                    outfile << "v;" << x4 << ";" <<y1<< ";"  << z4 << ";" << std::endl;;
                    outfile << "v;" << x3 << ";" <<y1<< ";"  << z3 << ";" << std::endl;;

                }
                
            }
        }
        outfile.close();
        std::cout << "Cone model generated and saved to " << filename << std::endl;
    }

    static void generateSphere(float radius, int slices, int stacks, const std::string& filename){
        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        // Set precision to 6 decimals
        outfile << std::fixed << std::setprecision(6);  

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
                    outfile << "v;" << x0 << ";" <<y0<<  ";" << z0 << ";" << std::endl;;
		    outfile << "v;" << x2 << ";" <<y2<< ";"  << z2 << ";" << std::endl;;
		    outfile << "v;" << x1 << ";" <<y1<< ";"  << z1 << ";" << std::endl;;
		    	}

		    	if(i != stacks-1){
                    outfile << "v;" << x1 << ";" <<y1<<  ";" << z1 << ";" << std::endl;;
		    outfile << "v;" << x2 << ";" <<y2<< ";"  << z2 << ";" << std::endl;;
		    outfile << "v;" << x3 << ";" <<y3<< ";"  << z3 << ";" << std::endl;;
		    	}
            }
        }
        outfile.close();
        std::cout << "Sphere model generated and saved to " << filename << std::endl;
    
    }


    
    static Point surfPoint(float u, float v, std::vector<int> i){ //verificar se as contas se fazem bem
    	float m[4][4] = {{-1.0f,  3.0f, -3.0f, 1.0f},
    				 {3.0f, -6.0f,  3.0f, 0.0f},
    				 {-3.0f,  3.0f,  0.0f, 0.0f},
    				 {1.0f,  0.0f,  0.0f, 0.0f}};

    	float uv[4] = {u*u*u,u*u,u,1.0f}; //1*4 vetor u
    	float vv[4] = {v*v*v,v*v,v,1.0f}; //4*1 vetor v
    	float um[4];
    	float vm[4];
    	multMatrixVector(m, uv, um, true); 	// uv * m = 1 * 4
    	multMatrixVector(m, vv, vm, false); // m * vv = 4 * 1
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

    	float umpx[4];
    	float umpy[4];
    	float umpz[4];

    	multMatrixVector(mx, um, umpx, true); // um * mx = 1*4 
    	multMatrixVector(my, um, umpy, true); // um * my = 1*4
    	multMatrixVector(mz, um, umpz, true); // um * mz = 1*4
    	Point a;
    	a.setX(multVectors(umpx, vm));
    	a.setY(multVectors(umpy, vm));
    	a.setZ(multVectors(umpz, vm));
    	return a;
    }


    static void generateBezier(const std::string& patchfile, int tessellation, const std::string& filename){
        //abre e le o patchfile
        parseFile(patchfile, vectors, controlPoints);
        
        
        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        // Set precision to 6 decimals
        outfile << std::fixed << std::setprecision(6);

        Point a, b, c, d;
	    float u = 0.0f, v = 0.0f, delta = 1.0f/tessellation;
	    for(int k = 0; k<vectors.size(); k++){ //percorrer cada vetor de indices
	    	for(int i = 0; i < tessellation; i++, u+=delta){
	    		for(int j = 0; j < tessellation; j++, v+=delta){
                
	    			//CALCULO DE PONTOS
	    			a = surfPoint(u		 , v	  , vectors[k]);
	    			b = surfPoint(u		 , v+delta, vectors[k]);
	    			c = surfPoint(u+delta, v	  , vectors[k]);
	    			d = surfPoint(u+delta, v+delta, vectors[k]);
    
                    
                    //Triangulaçao
                    outfile << "v;" << c.getX() << ";" << c.getY() <<  ";" << c.getZ() << ";" << std::endl;;
		            outfile << "v;" << a.getX() << ";" << a.getY() << ";"  << a.getZ() << ";" << std::endl;;
		            outfile << "v;" << b.getX() << ";" << b.getY() << ";"  << b.getZ() << ";" << std::endl;;
                    outfile << "v;" << b.getX() << ";" << b.getY() <<  ";" << b.getZ() << ";" << std::endl;;
		            outfile << "v;" << d.getX() << ";" << d.getY() << ";"  << d.getZ() << ";" << std::endl;;
		            outfile << "v;" << c.getX() << ";" << c.getY() << ";"  << c.getZ() << ";" << std::endl;;
    
	    		}
	    		v = 0.0f;
	    	}
	    	u = v = 0.0f;
	    } 
        outfile.close();
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
