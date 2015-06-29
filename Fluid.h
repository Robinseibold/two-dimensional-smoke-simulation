#include <vector>

using namespace std;                                    /* Namespace std and typedef for prettier code      */
typedef vector<vector<float>> matrix;

class Fluid {
public:
    Fluid(int size) : fluidSize(size-1), x(size, vector<float>(size)){
        y = yPrevious = density = densityPrevious = xPrevious = x;
    }
    void addSourceDensity(float particleAmount, int i, int j);
    void addSourceVelocity(float force, float xDirection, float yDirection, int i, int j);
    void densityStep(float diff, float dt);
    void velocityStep(float visc, float dt);
    matrix getDensity();
    void clearPrevious();
    void clearCurrent();
    
private:
    void addSource(matrix &destination, matrix &source, float dt);
    void diffuse(matrix &current, matrix &previous, float diff, float dt, int iterations, int b);
    void advect(matrix &current, matrix &previous, matrix &velocityX, matrix &velocityY, float dt, int b);
    void project(matrix &currentX, matrix &currentY, matrix &previousX, matrix &previousY, int iterations);
    void setBounds(matrix &source, int b);
    
    float calculateNeighbourSum(matrix &current, int i, int j);
    float constrainValue(float value);
    float horizontalDifference(matrix &source, int i, int j);
    float verticalDifference(matrix &source, int i, int j);
    
	int fluidSize;
    matrix x, xPrevious;                                /* Current and previous velocity in the x direction  */
    matrix y, yPrevious;                                /* Current and previous velocity in the y direction  */
    matrix density, densityPrevious;                    /* Current and previous density                      */
};
