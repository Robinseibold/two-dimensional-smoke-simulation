#include "Fluid.h"

void Fluid::addSourceDensity(float particleAmount, int i, int j) {
    densityPrevious[i][j] = particleAmount;
}

void Fluid::addSourceVelocity(float force, float xDirection, float yDirection, int i, int j) {
    xPrevious[i][j] = force * xDirection;
    yPrevious[i][j] = force * yDirection;
}

matrix Fluid::getDensity() {
    return density;
}

void Fluid::clearPrevious() {
    for(int i = 0; i != fluidSize; ++i) {
        for(int j = 0; j != fluidSize; ++j) {
            densityPrevious[i][j] = 0.0;
            xPrevious[i][j] = 0.0;
            yPrevious[i][j] = 0.0;
        }
    }
}

void Fluid::clearCurrent() {
    for(int i = 0; i != fluidSize; ++i) {
        for(int j = 0; j != fluidSize; ++j) {
            density[i][j] = 0.0;
            x[i][j] = 0.0;
            y[i][j] = 0.0;
        }
    }
}

void Fluid::densityStep(float diff, float dt) {
    addSource(density, densityPrevious, dt);
    diffuse(densityPrevious, density, diff, dt, 20, 0);
    advect(density, densityPrevious, x, y, dt, 0);
}

void Fluid::velocityStep(float visc, float dt) {
    addSource(x, xPrevious, dt);
    addSource(y, yPrevious, dt);
    diffuse(xPrevious, x, visc, dt, 20, 1);
    diffuse(yPrevious, y, visc, dt, 20, 2);
    project(xPrevious, yPrevious, x, y, 20);
    advect(x, xPrevious, xPrevious, yPrevious, dt, 1);
    advect(y, yPrevious, xPrevious, yPrevious, dt, 2);
    project(x, y, xPrevious, yPrevious, 20);
}

void Fluid::addSource(matrix &destination, matrix &source, float dt) {
    for(int i = 0; i != (fluidSize + 1); ++i) {
        for(int j = 0; j != (fluidSize + 1); ++j) {
            destination[i][j] += source[i][j] * dt;
        }
    }
}

void Fluid::diffuse(matrix &current, matrix &previous, float diff, float dt, int iterations, int b) {
    float ratio = dt * diff * (fluidSize - 1) * (fluidSize - 1);
    for(int k = 0; k != iterations; ++k) {
        for(int i = 1; i != fluidSize; ++i) {
            for(int j = 1; j != fluidSize; ++j) {
                current[i][j] = (previous[i][j] + ratio * calculateNeighbourSum(current, i, j)) / (1 + (4 * ratio));
            }
        }
        setBounds(current, b);
    }
}

void Fluid::advect(matrix &current, matrix &previous, matrix &velocityX, matrix &velocityY, float dt, int b) {
    float dtRatio = dt * (fluidSize - 1);
    for(int i = 1; i != fluidSize; ++i) {
        for(int j = 1; j != fluidSize; ++j) {
            float xPosition = constrainValue(i - dtRatio * velocityX[i][j]);
            float yPosition = constrainValue(j - dtRatio * velocityY[i][j]);
            int fromX = int (xPosition);
            int fromY = int (yPosition);
            float dtX = xPosition - fromX;
            float dtY = yPosition - fromY;
            current[i][j] = (1 - dtX) * ((1 - dtY) * previous[fromX][fromY] + dtY * previous[fromX][fromY + 1]) +
                            dtX * ((1 - dtY) * previous[fromX + 1][fromY] + dtY * previous[fromX + 1][fromY + 1]);
        }
    }
    setBounds(current, b);
}

void Fluid::project(matrix &currentX, matrix &currentY, matrix &previousX, matrix &previousY, int iterations) {
    float unit = 1.0 / (fluidSize - 1);
    for(int i = 1; i != fluidSize; ++i) {
        for(int j = 1; j != fluidSize; ++j) {
            previousY[i][j] = -0.5 * unit * (verticalDifference(currentX, i, j) + horizontalDifference(currentY, i, j));
            previousX[i][j] = 0;
        }
    }
    setBounds(previousY, 0);
    setBounds(previousX, 0);
    for(int k = 0; k != iterations; ++k) {
        for(int i = 1; i != fluidSize; ++i) {
            for(int j = 1; j != fluidSize; ++j) {
                previousX[i][j] = (previousY[i][j] + calculateNeighbourSum(previousX, i, j)) / 4;
            }
        }
        setBounds(previousX, 0);
    }
    for(int i = 1; i != fluidSize; ++i) {
        for(int j = 1; j != fluidSize; ++j) {
            currentX[i][j] -= 0.5 * verticalDifference(previousX, i, j) / unit;
            currentY[i][j] -= 0.5 * horizontalDifference(previousX, i, j) / unit;
        }
    }
    setBounds(currentX, 1);
    setBounds(currentY, 2);
}

void Fluid::setBounds(matrix &source, int b) {
    for(int i = 1; i != fluidSize; ++i) {
        source[0][i] = b == 1 ? -source[1][i] : source[1][i];
        source[fluidSize][i] = b == 1 ? -source[fluidSize - 1][i] : source[fluidSize - 1][i];
        source[i][0] = b == 2 ? -source[i][1] : source[i][1];
        source[i][fluidSize] = b == 2 ? -source[i][fluidSize - 1] : source[i][fluidSize - 1];
    }
    source[0][0] = 0.5 * (source[1][0] + source[0][1]);
    source[0][fluidSize] = 0.5 * (source[1][fluidSize] + source[0][fluidSize - 1]);
    source[fluidSize][0] = 0.5 * (source[fluidSize -1][0] + source[fluidSize][1]);
    source[fluidSize][fluidSize] = 0.5 * (source[fluidSize - 1][fluidSize] + source[fluidSize][fluidSize - 1]);
}

float Fluid::constrainValue(float value) {
    if(value < 0.5) {
        value = 0.5;
    }else if(value > (fluidSize - 0.5)) {
        value = fluidSize - 0.5;
    }
    return value;
}

float Fluid::calculateNeighbourSum(matrix &source, int i, int j) {
    return source[i - 1][j] + source[i + 1][j] + source[i][j - 1] + source[i][j + 1];
}

float Fluid::horizontalDifference(matrix &source, int i, int j) {
    return source[i][j + 1] - source[i][j - 1];
}

float Fluid::verticalDifference(matrix &source, int i, int j) {
    return source[i + 1][j] - source[i - 1][j];
}

























