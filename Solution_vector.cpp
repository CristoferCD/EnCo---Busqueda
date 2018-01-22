//
// Created by crist on 13/01/18.
//

#include "Solution_vector.h"

Solution_vector::Solution_vector(std::vector<int> solution, double cost) {
    this->solution = std::move(solution);
    this->cost = cost;
}

Solution_vector::Solution_vector() {
    this->solution = std::vector<int>();
    this->cost = 0;
}
