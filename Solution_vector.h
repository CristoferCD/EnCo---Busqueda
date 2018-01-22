//
// Created by crist on 13/01/18.
//

#ifndef ENCO_BUSQUEDA_SOLUTION_VECTOR_H
#define ENCO_BUSQUEDA_SOLUTION_VECTOR_H


#include <vector>

class Solution_vector {
    public:
        Solution_vector();
        Solution_vector(std::vector<int> solution, double cost);

    bool operator<(const Solution_vector &rhs) const {
        return cost < rhs.cost;
    }

    std::vector<int> solution;
        double cost;
};


#endif //ENCO_BUSQUEDA_SOLUTION_VECTOR_H
