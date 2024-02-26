#include <iostream>
#include <fstream>
#include <iomanip> 
#include <cmath>

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
                outfile << "v;" << x2 << ";0.000000;" << posZ << ";" << std::endl;;
                outfile << "v;" << x2 << ";0.000000;" << z2 << ";" << std::endl;;

                outfile << "v;" << posX << ";0.000000;" << posZ << ";" << std::endl;;
                outfile << "v;" << x2 << ";0.000000;" << z2 << ";" << std::endl;;
                outfile << "v;" << posX << ";0.000000;" << z2 << ";" << std::endl;;
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


        //face 1 y=-size/2 BASE
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
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << z2 << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << posZ << ";" << std::endl;;

                outfile << "v;" << posX << ";" <<posInicial<< ";"  << posZ << ";" << std::endl;;
                outfile << "v;" << posX << ";" <<posInicial<< ";"  << z2 << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << z2 << std::endl;
            }
        }

        //face 2 y = size/2 TOPO
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
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << posZ << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << z2 << ";" << std::endl;;

                outfile << "v;" << posX << ";" <<posInicial<< ";"  << posZ << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posInicial<< ";"  << z2 << ";" << std::endl;;
                outfile << "v;" << posX << ";" <<posInicial<< ";"  << z2 << ";" << std::endl;;
            }
        }

        //face 3 x= -size/2
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

        //face 4 x= size/2
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


                outfile << "v;" << posInicial*-1<< ";" <<posY<< ";"  << posZ << ";" << std::endl;;
                outfile << "v;" << posInicial*-1<< ";" <<y2<< ";"  << z2 << ";" << std::endl;;
                outfile << "v;" << posInicial*-1<< ";" <<posY<< ";"  << z2 << ";" << std::endl;;

            }
        }
        //face 5 z= -size/2
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

                outfile << "v;" << posX << ";" <<posY<< ";" << posInicial*-1 << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<y2<< ";"  << posInicial*-1 << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<posY<< ";"  << posInicial*-1 << ";" << std::endl;;

                outfile << "v;" << posX << ";" <<posY<< ";"  << posInicial*-1 << ";" << std::endl;;
                outfile << "v;" << posX << ";" <<y2<< ";"  << posInicial*-1 << ";" << std::endl;;
                outfile << "v;" << x2 << ";" <<y2<< ";"  << posInicial*-1 << ";" << std::endl;;
            }
        }


        
        outfile.close();
        std::cout << "Box model generated and saved to " << filename << std::endl;
    }
   
};

int main() {
    // Generate models
    Generator::generatePlane(2.0f, 10, "plane.3d");
    Generator::generateBox(2.0f, 2, "box.3d");

    return 0;
}
