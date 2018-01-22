//
// Created by crist on 22/11/17.
//

#include <sstream>
#include <iostream>
#include <iomanip>
#include <deque>
#include "Output_Manager.h"
#include "Solution_vector.h"

Output_Manager::Output_Manager() = default;

Output_Manager::Output_Manager(const char *filePath) {

}

void Output_Manager::print_solution(int index, const std::vector<int> &solution, int cost) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "\nSOLUCION S_" << index << " -> [";
        for (auto &item : solution)
            output << item << ", ";
        output.seekp(-2, output.end);
        output << "]; " << cost << "km";
        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_neighbour(int index, std::tuple<int, int> swap, const std::vector<int> &solution, int cost) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "\tVECINO V_" << index;
        output << " -> Intercambio: (" << std::get<0>(swap)
               << ", " << std::get<1>(swap) << "); [";
        for (auto &item : solution)
            output << item << ", ";
        output.seekp(-2, output.end);
        output << "]; ";
        output << cost << "km";
        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

Output_Manager::~Output_Manager() {
    if (outputHandler.valid())
        outputHandler.wait();
    if (outputFile.is_open())
        outputFile.close();
}

void Output_Manager::print_initialSolution(std::vector<int> solution, int cost, double init_temp) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "SOLUCION INICIAL:" << std::endl;
        output << "\tRECORRIDO:";
        for (auto &item : solution)
            output << " " << item;
        output << " " << std::endl << "\tFUNCION OBJETIVO (km): "
               << cost << std::endl;
        output << "\tTEMPERATURA INICIAL: " << std::fixed << std::setprecision(6) << init_temp;
        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_iteration(int iteration_index, std::tuple<int, int> swap, std::vector<int> solution, int cost,
                                int unsuccessful_iteration_index, std::deque<std::tuple<int, int>> *taboo) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << std::endl;
        output << "ITERACION: " << iteration_index << std::endl;
        if (std::get<0>(swap) == 0)
            std::cout << "break\n";
        output << "\tINTERCAMBIO: (" << std::get<0>(swap) << ", "
               << std::get<1>(swap) << ")" << std::endl;
        output << "\tRECORRIDO: ";
        for (auto &item : solution)
            output << item << " ";
        output << std::endl << "\tCOSTE (km): "
               << cost << std::endl;
        output << "\tITERACIONES SIN MEJORA: " << unsuccessful_iteration_index << std::endl;
        output << "\tLISTA TABU:" << std::endl;
        for (auto &item : *taboo) {
            output << "\t" << std::get<0>(item) << " "
                   << std::get<1>(item) << std::endl;
        }
        if (outputToFile)
            outputFile << output.str();
        else
            std::cout << output.str();
    });
}

void Output_Manager::setOutputFile(const char *filePath) {
    outputFile.open(filePath);
    if (!outputFile.is_open())
        throw std::exception();
    outputToFile = true;
}

void Output_Manager::print_progress(float progress) {
    if (!outputToFile) return; //Don't print progress if output is being shown in terminal
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        int barWidth = 70;

        std::cout << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout.flush();
    });
}

void Output_Manager::print_reboot(int index) {

    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << std::endl;
        output << "***************" << std::endl;
        output << "REINICIO: " << index << std::endl;
        output << "***************" << std::endl;
        if (outputToFile)
            outputFile << output.str();
        else
            std::cout << output.str();
    });
}

void Output_Manager::print_bestSolution(std::vector<int> solution, int cost, int index) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << std::endl;
        output << "MEJOR SOLUCION: " << std::endl;
        output << "\tRECORRIDO: ";
        for (auto &item : solution)
            output << item << " ";
        output << std::endl << "\tCOSTE (km): "
               << cost << std::endl;
        output << "\tITERACION: " << index << std::endl;
        if (outputToFile)
            outputFile << output.str();

        std::cout << output.str();
    });
}

void Output_Manager::print_bestSolution(std::vector<int> solution, int cost, int index, double mu, double phi) {

    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << std::endl << std::endl;
        output << "MEJOR SOLUCION: " << std::endl;
        output << "\tRECORRIDO: ";
        for (auto &item : solution)
            output << item << " ";
        output << std::endl << "\tFUNCION OBJETIVO (km): "
               << cost << std::endl;
        output << "\tITERACION: " << index << std::endl;
        output << "\tmu = " << mu << ", phi = " << phi << std::endl;

        if (outputToFile)
            outputFile << output.str();

        std::cout << output.str();
    });
}

void Output_Manager::print_iteration(int iterationIndex, int cityIndex, int citySelected, int insertIndex,
                                     std::vector<int> solution, int cost, int delta, double temp, double exponential,
                                     bool isAccepted, int numTested, int numAccepted) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << std::endl;
        output << "ITERACION: " << iterationIndex << std::endl;
        output << "\tINDICE CIUDAD: " << cityIndex << std::endl;
        output << "\tCIUDAD: " << citySelected << std::endl;
        output << "\tINDICE INSERCION: " << insertIndex << std::endl;
        output << "\tRECORRIDO: ";
        for (auto &item : solution)
            output << item << " ";
        output << std::endl << "\tFUNCION OBJETIVO (km): "
               << cost << std::endl;
        output << "\tDELTA: " << std::fixed << std::setprecision(6) << delta << std::endl;
        output << "\tTEMPERATURA: " << std::fixed << std::setprecision(6) << temp << std::endl;
        output << "\tVALOR DE LA EXPONENCIAL: " << std::fixed << std::setprecision(6) << exponential << std::endl;
        if (isAccepted)
            output << "\tSOLUCION CANDIDATA ACEPTADA" << std::endl;
        output << "\tCANDIDATAS PROBADAS: " << numTested;
        output << ", ACEPTADAS: " << numAccepted << std::endl;

        if (outputToFile)
            outputFile << output.str();
        else
            std::cout << output.str();
    });
}

void Output_Manager::print_chill(int countEnfriamientos, double temperature) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << std::endl;
        output << "============================" << std::endl;
        output << "ENFRIAMIENTO: " << countEnfriamientos << std::endl;
        output << "============================" << std::endl;
        output << "TEMPERATURA: " << std::fixed << std::setprecision(6) << temperature << std::endl;

        if (outputToFile)
            outputFile << output.str();
        else
            std::cout << output.str();
    });
}

void Output_Manager::print(std::string message) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        if (outputToFile)
            outputFile << message << std::endl;
        else
            std::cout << message << std::endl;
    });
}

void Output_Manager::print_population(std::vector<Solution_vector> population) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        auto size = population.size();
        std::ostringstream output;
        for (int i = 0; i < size; i++) {
            output << "INDIVIDUO " << i << " = ";
            output << "{FUNCION OBJETIVO (km): " << population.at(i).cost <<", ";
            output << "RECORRIDO: ";
            for (auto city : population.at(i).solution)
                output << city << " ";
            output << "}" << std::endl;
        }

        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_tournament(int tournament_index, int &firstChosen, int &secondChosen, int &winner) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "\tTORNEO " << tournament_index << ": ";
        output << firstChosen << " " << secondChosen;
        output << " GANA " << winner;

        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_title_selection(int iteration) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "ITERACION: " << iteration << ", SELECCION";

        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_crossover_init(int cross_index, double reproduction_probability, Solution_vector parent1,
                                          Solution_vector parent2) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "\tCRUCE: (" << cross_index << ", " << cross_index + 1;
        output << ") (ALEATORIO: " << std::fixed << std::setprecision(6) << reproduction_probability;
        output << ")" << std::endl;
        output << "\t\tPADRE: = {FUNCION OBJETIVO (km): " << std::setprecision(0) << parent1.cost;
        output << ", RECORRIDO: ";
        for (auto city : parent1.solution)
            output << city << " ";
        output << "}" << std::endl;

        output << "\t\tPADRE: = {FUNCION OBJETIVO (km): " << parent2.cost;
        output << ", RECORRIDO: ";
        for (auto city : parent2.solution)
            output << city << " ";
        output << "}" << std::endl;

        if (outputToFile)
            outputFile << output.str();
        else
            std::cout << output.str();
    });
}

void Output_Manager::print_crossover_result(std::pair<int, int> cut_index,
                                            std::pair<Solution_vector, Solution_vector> children) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "\t\tCORTES: (" << cut_index.first << ", " << cut_index.second << ")" << std::endl;
        output << "\t\tHIJO: = {FUNCION OBJETIVO (km): " << children.first.cost;
        output << ", RECORRIDO: ";
        for (auto city : children.first.solution)
            output << city << " ";
        output << "}" << std::endl;

        output << "\t\tHIJO: = {FUNCION OBJETIVO (km): " << children.second.cost;
        output << ", RECORRIDO: ";
        for (auto city : children.second.solution)
            output << city << " ";
        output << "}" << std::endl;

        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_title_crossover(int iterations) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << std::endl << "ITERACION: " << iterations << ", CRUCE ";

        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_title_mutation(int iteration) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "ITERACION: " << iteration << ", MUTACION";

        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_individual_init_mutation(std::vector<int> path, int index) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "\tINDIVIDUO " << index << std::endl;
        output << "\tRECORRIDO ANTES: ";
        for (auto val : path)
            output << val << " ";

        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_successful_mutation(int index, double probability, int swapsWith) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "\t\tPOSICION: " << index;
        output << " (ALEATORIO " << std::fixed << std::setprecision(6) << probability;
        output << ") INTERCAMBIO CON: " << swapsWith;

        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_unsuccessful_mutation(int index, double probability) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "\t\tPOSICION: " << index;
        output << " (ALEATORIO " << std::fixed << std::setprecision(6) << probability;
        output << ") NO MUTA";

        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_individual_result_mutation(std::vector<int> path) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << "\tRECORRIDO DESPUES: ";
        for (auto val : path)
            output << val << " ";
        output << std::endl;

        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_title_replacement(int iteration) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << std::endl << "ITERACION: " << iteration << ", REEMPLAZO";

        if (outputToFile)
            outputFile << output.str() << std::endl;
        else
            std::cout << output.str() << std::endl;
    });
}

void Output_Manager::print_bestSolution(Solution_vector solution, int index) {
    if (outputHandler.valid())
        outputHandler.wait();
    outputHandler = std::async(std::launch::async, [=] {
        std::ostringstream output;
        output << std::endl;
        output << "MEJOR SOLUCION: " << std::endl;
        output << "RECORRIDO: ";
        for (auto &item : solution.solution)
            output << item << " ";
        output << std::endl << "FUNCION OBJETIVO (km): "
               << solution.cost << std::endl;
        output << "ITERACION: " << index << std::endl;
        if (outputToFile)
            outputFile << output.str();

        std::cout << output.str();
    });
}
