//
// Created by crist on 22/11/17.
//

#ifndef ENCO_BUSQUEDA_DISTANCE_MATRIX_H
#define ENCO_BUSQUEDA_DISTANCE_MATRIX_H

#define NUM_CIUDADES 100

#include <vector>


class Distance_Matrix {
public:
    explicit Distance_Matrix(const char* filePath);

    int distancia(int ciudadA, int ciudadB);
    int coste(const std::vector<int>& solucion);

    virtual ~Distance_Matrix();

private:
    std::vector<int>* _matrix_values;
};


#endif //ENCO_BUSQUEDA_DISTANCE_MATRIX_H
