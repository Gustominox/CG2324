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

                outfile << "v;" << posX << " 0.000000;" << posZ << std::endl;
                outfile << "v;" << x2 << " 0.000000;" << posZ << std::endl;
                outfile << "v;" << x2 << " 0.000000;" << z2 << std::endl;

                outfile << "v;" << posX << ";0.000000;" << posZ << std::endl;
                outfile << "v;" << x2 << ";0.000000;" << z2 << std::endl;
                outfile << "v;" << posX << ";0.000000;" << z2 << std::endl;
            }
        }
        outfile.close();
        std::cout << "Plane model generated and saved to " << filename << std::endl;
    }

    // Generate a box model
    static void generateBox(float dimension, int divisions, const std::string& filename) {
        float step = dimension / divisions;
        float halfDimension = dimension / 2;

        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        for (int i = 0; i < divisions; ++i) {
            for (int j = 0; j < divisions; ++j) {
                for (int k = 0; k < divisions; ++k) {
                    float x = i * step - halfDimension;
                    float y = j * step - halfDimension;
                    float z = k * step - halfDimension;

                    // Define vertices for each corner of the cube
                    outfile << "v " << x << " " << y << " " << z << std::endl;
                }
            }
        }
        outfile.close();
        std::cout << "Box model generated and saved to " << filename << std::endl;
    }
   
};

int main() {
    // Generate models
    Generator::generatePlane(2.0f, 10, "plane.3d");
    Generator::generateBox(2.0f, 10, "box.3d");

    return 0;
}
