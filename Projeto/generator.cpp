#include <iostream>
#include <fstream>
#include <cmath>

class Generator {
public:
    // Generate a plane model
    static void generatePlane(float sizeX, float sizeZ, int divisionsX, int divisionsZ, const std::string& filename) {
        std::ofstream outfile(filename);
        if (!outfile.is_open()) {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        float stepX = sizeX / divisionsX;
        float stepZ = sizeZ / divisionsZ;

        for (int z = 0; z < divisionsZ; ++z) {
            for (int x = 0; x < divisionsX; ++x) {
                // Define vertices for each quad in the plane
                float x1 = x * stepX - sizeX / 2;
                float z1 = z * stepZ - sizeZ / 2;
                float x2 = x1 + stepX;
                float z2 = z1 + stepZ;

                outfile << "v " << x1 << " 0 " << z1 << std::endl;
                outfile << "v " << x2 << " 0 " << z1 << std::endl;
                outfile << "v " << x2 << " 0 " << z2 << std::endl;

                outfile << "v " << x1 << " 0 " << z1 << std::endl;
                outfile << "v " << x2 << " 0 " << z2 << std::endl;
                outfile << "v " << x1 << " 0 " << z2 << std::endl;
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
    Generator::generatePlane(2.0f, 2.0f, 10, 10, "plane.3d");
    Generator::generateBox(2.0f, 10, "box.3d");

    return 0;
}
