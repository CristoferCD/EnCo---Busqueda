//
// Created by crist on 22/11/17.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include "Distance_Matrix.h"

Distance_Matrix::Distance_Matrix(const char *filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cout << "Error abriendo " << filePath;
        exit(1);
    }

    // Initialize matrix to vector of zeroes
    _matrix_values = new std::vector<int>((NUM_CIUDADES - 1) * (NUM_CIUDADES - 1));
    std::fill(_matrix_values->begin(), _matrix_values->end(), 0);

    std::string line;
    int column = 0;
    int row = 1;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string value;
        while (lineStream >> value) {
            _matrix_values->at((NUM_CIUDADES-1)*(row-1) + column) = std::stoi(value);
            column++;
        }
        column = 0;
        row++;
    }
    //TODO: if file bigger than NUM_CIUDADES
}

int Distance_Matrix::distancia(int ciudadA, int ciudadB) {
    int row, column;
    if (ciudadA > ciudadB) {
        row = ciudadA - 1;
        column = ciudadB;
    } else {
        row = ciudadB - 1;
        column = ciudadA;
    }
    return _matrix_values->at((NUM_CIUDADES-1)*row + column);
}

int Distance_Matrix::coste(const std::vector<int> &solucion) {
    int coste = distancia(0, solucion.at(0));

    for (int i = 1; i <= NUM_CIUDADES-2; i++) {
        coste += distancia(solucion.at(i-1), solucion.at(i));
    }

    coste += distancia(solucion.at(NUM_CIUDADES-2), 0);

    return coste;
}

Distance_Matrix::~Distance_Matrix() {
    delete _matrix_values;
}
