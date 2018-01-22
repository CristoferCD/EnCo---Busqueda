//
// Created by crist on 22/11/17.
//

#ifndef ENCO_BUSQUEDA_OUTPUT_MANAGER_H
#define ENCO_BUSQUEDA_OUTPUT_MANAGER_H


#include <fstream>
#include <vector>
#include <tuple>
#include <future>
#include "Solution_vector.h"

class Output_Manager {
public:
    Output_Manager();
    Output_Manager(const char* filePath);

    void setOutputFile(const char *filePath);

    void print_solution(int index, const std::vector<int>& solution, int cost);
    void print_neighbour(int index, std::tuple<int,int> swap, const std::vector<int>& solution, int cost);

    virtual ~Output_Manager();

    void print_initialSolution(std::vector<int> solution, int cost, double init_temp);

    void print_iteration(int iteration_index, std::tuple<int, int> swap, std::vector<int> solution, int cost,
                         int unsuccessful_iteration_index, std::deque<std::tuple<int, int>> *taboo);

    void print_progress(float progress);

    void print_reboot(int index);

    void print_bestSolution(std::vector<int> solution, int cost, int index);
    void print_bestSolution(std::vector<int> solution, int cost, int index, double mu, double phi);
    void print_bestSolution(Solution_vector solution, int index);

    void print_iteration(int iterationIndex, int cityIndex, int citySelected, int insertIndex, std::vector<int> solution,
                         int cost, int delta, double temp, double exponential, bool isAccepted, int numTested,
                         int numAccepted);

    void print_chill(int countEnfriamientos, double temperature);

    void print(std::string message);

    void print_population(std::vector<Solution_vector> population);

    void print_tournament(int tournament_index, int &firstChosen, int &secondChosen, int &winner);

    void print_title_selection(int iteration);

    void print_crossover_init(int cross_index, double reproduction_probability, Solution_vector parent1,
                              Solution_vector parent2);

    void print_crossover_result(std::pair<int, int> cut_index, std::pair<Solution_vector, Solution_vector> children);

    void print_title_crossover(int iterations);

    void print_title_mutation(int iteration);

    void print_individual_init_mutation(std::vector<int> path, int index);

    void print_successful_mutation(int index, double probability, int swapsWith);

    void print_unsuccessful_mutation(int index, double probability);

    void print_individual_result_mutation(std::vector<int> path);

    void print_title_replacement(int iteration);

private:
    bool outputToFile = false;
    std::ofstream outputFile;
    std::future<void> outputHandler;
};


#endif //ENCO_BUSQUEDA_OUTPUT_MANAGER_H
