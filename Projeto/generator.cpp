#include <iostream>
#include <fstream>
#include <iomanip> 
#include <cmath>
#include <cstring>

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
