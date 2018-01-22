#include <cmath>
#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <sstream>
#include <tuple>
#include <deque>
#include "Distance_Matrix.h"
#include "Output_Manager.h"
#include "Solution_vector.h"
#include "timsort.hpp"

#define NUM_ITERACIONES 2000
#define TAM_POBLACION 50

void generar_solucion_inicial(std::vector<int> &solucion);
std::tuple<int, int> generar_intercambio(std::vector<std::tuple<int, int>>* intercambiosGenerados);
int total_vecinos () ;
std::tuple<int, int> generar_intercambio_ordinal(const std::deque<std::tuple<int,int>>& listaTabu) ;
void generar_solucion_inicial_greedy(std::vector<int> &solucion, Distance_Matrix &distance_matrix) ;
std::vector<int> generar_solucion_por_insercion(int solucion, std::vector<int> vector, Distance_Matrix *pMatrix, int* bestInsertIndex);
void generar_poblacion(std::vector<Solution_vector> &poblacion, Distance_Matrix &matriz_distancias);
std::tuple<int, int, int> realizar_torneo(std::vector<Solution_vector> &poblacion) ;
std::vector<Solution_vector> reproducirse(std::vector<Solution_vector> &poblacion, Distance_Matrix &distance_matrix);
std::pair<Solution_vector, Solution_vector>
crossover(Solution_vector parent1, Solution_vector parent2, std::pair<int, int> border_index);
void mutar_poblacion(std::vector<Solution_vector> &poblacion, Distance_Matrix &distance_matrix);


const double euler_constant = std::exp(1.0);
static std::ifstream randomsFile;
Output_Manager output_manager;

int main(int argc, char* argv[]) {

    std::vector<int> solucion;
    std::vector<Solution_vector> poblacion;

    if (argc > 4 || argc == 1) {
        std::cerr << "Uso: " << argv[0]
                  << " fichero_distancias [fichero_aleatorios] [fichero_salida]\n";
        return 1;
    }

    Distance_Matrix matriz_distancias = Distance_Matrix(argv[1]);

    if (argc >= 3) {
        randomsFile.open(argv[2]);
        if (!randomsFile.is_open()) {
            std::cerr << "Error abriendo archivo: "
                      << argv[2];
            return 1;
        }
    }

    if (argc == 4) {
        output_manager.setOutputFile(argv[3]);
    }

    generar_poblacion(poblacion, matriz_distancias);
    output_manager.print("POBLACION INICIAL");
    output_manager.print_population(poblacion);
    Solution_vector mejorGlobal = Solution_vector(std::vector<int>(), INT32_MAX);
    int mejorIteracion = 0;

    for (int iteraciones = 1; iteraciones <= NUM_ITERACIONES; iteraciones++) {
        output_manager.print_progress((float) iteraciones / (float) NUM_ITERACIONES);

        output_manager.print_title_selection(iteraciones);
        //Realiza todos los torneos menos dos
        std::vector<Solution_vector> nuevaPoblacion;
        for (int i = 0; i < TAM_POBLACION - 2; i++) {
            auto resultadosTorneo = realizar_torneo(poblacion);
            output_manager.print_tournament(i, std::get<0>(resultadosTorneo), std::get<1>(resultadosTorneo),
                                            std::get<2>(resultadosTorneo));
            nuevaPoblacion.push_back((poblacion.at(std::get<2>(resultadosTorneo))));
        }

        output_manager.print_title_crossover(iteraciones);
        auto poblacionCruzada = reproducirse(nuevaPoblacion,
                                             matriz_distancias); //Crossover (si hay fecundacion se meten los hijos, si no se meten ellos) -> genera una nueva ]oblacion, actualiza esta coleccion

        output_manager.print_title_mutation(iteraciones);
        mutar_poblacion(poblacionCruzada, matriz_distancias);

        gfx::timsort(poblacionCruzada.begin(), poblacionCruzada.end());
        gfx::timsort(poblacion.begin(), poblacion.end());

        poblacionCruzada.insert(poblacionCruzada.begin(), std::move(poblacion.at(0)));
        poblacionCruzada.emplace(poblacionCruzada.begin(), std::move(poblacion.at(1)));

        // Se reemplaza la poblacion
        poblacion = poblacionCruzada;
        output_manager.print_title_replacement(iteraciones);
        output_manager.print_population(poblacion);

        //Buscamos la mejor solucion
        std::sort(poblacionCruzada.begin(), poblacionCruzada.end());
        auto mejorLocal = poblacionCruzada.front();

        if (mejorLocal < mejorGlobal) {
            mejorGlobal = mejorLocal;
            mejorIteracion = iteraciones;
        }
    }

    std::cout << std::endl;
    output_manager.print_bestSolution(mejorGlobal, mejorIteracion);
    std::cout << std::endl;

    return 0;
}

void mutar_poblacion(std::vector<Solution_vector> &poblacion, Distance_Matrix &distance_matrix) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0, 1);

    int individuo_idx = 0;
    for (Solution_vector &individuo : poblacion) {
        output_manager.print_individual_init_mutation(individuo.solution, individuo_idx);
        for (int i = 0; i < (NUM_CIUDADES-1); i++) {
            double mutationProbability;
            if (!randomsFile.is_open()) {
                mutationProbability = dist(mt);
            } else {
                randomsFile >> mutationProbability;
            }

            if (mutationProbability < 0.01) {
                double mutationDestinationRandom;
                if (!randomsFile.is_open())
                    mutationDestinationRandom = dist(mt);
                else
                    randomsFile >> mutationDestinationRandom;

                int cityToSwap = (int)std::floor(mutationDestinationRandom*(NUM_CIUDADES-1));

                output_manager.print_successful_mutation(i, mutationProbability, cityToSwap);

                int a,b;
                if (cityToSwap > i) {
                    b = cityToSwap;
                    a = i;
                } else {
                    b = i;
                    a = cityToSwap;
                }

                auto first = std::next(individuo.solution.begin(), a);
                auto last = std::next(individuo.solution.begin(), b);
                while ((first != last) && (first != --last)) {
                    std::iter_swap(first++, last);
                }

                individuo.cost = distance_matrix.coste(individuo.solution);
            } else {
                output_manager.print_unsuccessful_mutation(i, mutationProbability);
            }
        }
        individuo_idx++;
        output_manager.print_individual_result_mutation(individuo.solution);
    }
}

std::vector<Solution_vector> reproducirse(std::vector<Solution_vector> &poblacion, Distance_Matrix &distance_matrix) {
    std::vector<Solution_vector> nuevaPoblacion = std::vector<Solution_vector>();
    std::random_device rd;
    std::mt19937 mt(rd());

    for (int i = 0; i < TAM_POBLACION - 2; i+=2) {
        auto padre1 = poblacion.at(i);
        auto padre2 = poblacion.at(i+1);

        double canReproduce;
        if (randomsFile.is_open()) {
            randomsFile >> canReproduce;
        } else {
            std::uniform_real_distribution<double> dist(0,1);

            canReproduce = dist(mt);
        }

        output_manager.print_crossover_init(i, canReproduce, padre1, padre2);

        if (canReproduce < 0.95) {
            std::pair<int,int> corte;
            if (!randomsFile.is_open()) {
                std::uniform_int_distribution<int> dist(0, NUM_CIUDADES - 2);
                corte.first = dist(mt);
                corte.second = dist(mt);
                if (corte.second == corte.first)
                    corte.second = (corte.second+1)%(NUM_CIUDADES-1);

            } else {
                double randomA, randomB;
                randomsFile >> randomA;
                randomsFile >> randomB;

                corte.first = (int)std::floor(randomA*(NUM_CIUDADES-1));
                corte.second = (int)std::floor(randomB*(NUM_CIUDADES-1));
            }

            auto hijos = crossover(padre1, padre2, corte);

            hijos.first.cost = distance_matrix.coste(hijos.first.solution);
            hijos.second.cost = distance_matrix.coste(hijos.second.solution);

            nuevaPoblacion.push_back(hijos.first);
            nuevaPoblacion.push_back(hijos.second);

            output_manager.print_crossover_result(corte, hijos);
        } else {
            nuevaPoblacion.push_back(padre1);
            nuevaPoblacion.push_back(padre2);
            output_manager.print("\t\tNO SE CRUZA\n");
        }
    }

    return nuevaPoblacion;
}

std::pair<Solution_vector, Solution_vector> crossover(Solution_vector parent1, Solution_vector parent2, std::pair<int, int> border_index) {
    Solution_vector hijo1, hijo2;
    if (border_index.second < border_index.first) std::swap(border_index.first, border_index.second);

    hijo1.solution = std::vector<int>(parent1.solution.size());
    hijo2.solution = std::vector<int>(parent2.solution.size());

    // Copia fragmentos en los hijos
    for (int i = border_index.first; i <= border_index.second; i++) {
        hijo1.solution.at(i) = std::move(parent1.solution.at(i));
        hijo2.solution.at(i) = std::move(parent2.solution.at(i));
    }

    int parentIndex = (border_index.second + 1) % (NUM_CIUDADES - 1);
    //Rellenar hijos con el otro padre
    for (int i = (border_index.second + 1) % (NUM_CIUDADES - 1); i != border_index.first; i = (i + 1) % (NUM_CIUDADES - 1)) {
        bool found = false;
        do {
            auto parentValue = parent2.solution.at(parentIndex);
            found = std::find(hijo1.solution.begin(), hijo1.solution.end(), parentValue) != hijo1.solution.end();
            if (!found) { // Si no existe en el hijo, se inserta
                hijo1.solution.at(i) = parentValue;
            } // Se prueba el siguiente valor del padre
            parentIndex = (int)((parentIndex + 1) % parent2.solution.size());
        } while (found);
    }

    // Y se repite para el otro hijo
    parentIndex = (border_index.second + 1) % (NUM_CIUDADES - 1);
    //Rellenar hijos con el otro padre
    for (int i = (border_index.second + 1) % (NUM_CIUDADES - 1); i != border_index.first; i = (i + 1) % (NUM_CIUDADES - 1)) {
        bool found = false;
        do {
            auto parentValue = parent1.solution.at(parentIndex);
            found = std::find(hijo2.solution.begin(), hijo2.solution.end(), parentValue) != hijo2.solution.end();
            if (!found) // Si no existe en el hijo, se inserta
                hijo2.solution.at(i) = parentValue;
            parentIndex = (int)((parentIndex + 1) % parent1.solution.size());
        } while (found);
    }

    return std::make_pair(hijo1, hijo2);
}

void generar_poblacion(std::vector<Solution_vector> &poblacion, Distance_Matrix &matriz_distancias) {
    int i = 0;
    for (i = 0; i < TAM_POBLACION/2.5; i++) {
        poblacion.emplace_back();
        generar_solucion_inicial(poblacion.at(i).solution);
    }
    for (i; i < TAM_POBLACION; i++) {
        poblacion.emplace_back();
        generar_solucion_inicial_greedy(poblacion.at(i).solution, matriz_distancias);
    }

    for (auto &individual : poblacion) {
        individual.cost = matriz_distancias.coste(individual.solution);
    }
}

std::vector<int> generar_solucion_por_insercion(int cityIndex, std::vector<int> solution, Distance_Matrix *matriz_distancias, int* bestInsertIndex) {
    auto mejorSolucion = solution;
    auto mejorCoste = INT32_MAX;
    auto citySelected = solution.at(cityIndex);

    for (int insertIndex = 0; insertIndex < NUM_CIUDADES - 1; insertIndex++) {
        if (insertIndex == cityIndex) continue;
        auto solucionActual = solution;
        solucionActual.erase(solucionActual.begin() + cityIndex);

        solucionActual.insert(solucionActual.begin() + insertIndex, citySelected);
        auto costeActual = matriz_distancias->coste(solucionActual);
        if (costeActual < mejorCoste) {
            mejorCoste = costeActual;
            mejorSolucion = solucionActual;
            *bestInsertIndex = insertIndex;
        }
    }

    return mejorSolucion;
}

void generar_solucion_inicial(std::vector<int> &solucion) {
    using std::cout;
    using std::endl;

    if (!randomsFile.is_open()) {
        for (int i = 1; i < NUM_CIUDADES; i++)
            solucion.push_back(i);

        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(solucion.begin(), solucion.end(), g);
    } else {
        for (int i = 1; i < NUM_CIUDADES; i++) {
            double random;
            randomsFile >> random;
            int value = 1 + (int)std::floor(random * (NUM_CIUDADES - 1));

            if (std::find(solucion.begin(), solucion.end(), value) != solucion.end()) {
                do {
                    value = (value+1)%NUM_CIUDADES;
                    if (value == 0) value++;
                } while (std::find(solucion.begin(), solucion.end(), value) != solucion.end());
            }
            solucion.push_back(value);
        }
    }
}

void generar_solucion_inicial_greedy(std::vector<int> &solucion, Distance_Matrix &distance_matrix) {
    int anterior = 0, menorCoste = INT32_MAX;
    int mejorVecino = 0;
    std::vector<int> vecinosGenerados;

    //Primer vecino aleatorio

    int primerVecino;
    if (!randomsFile.is_open()) {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(1, 9);

        primerVecino = dist(mt);
    } else {
        double random;
        randomsFile >> random;

        primerVecino = 1 + (int)std::floor(random * (NUM_CIUDADES - 1));
    }
    anterior = primerVecino;
    vecinosGenerados.push_back(primerVecino);
    solucion.push_back(primerVecino);

    //El resto por voraz
    while (solucion.size() != (NUM_CIUDADES - 1)) {
        for (int i = 1; i < NUM_CIUDADES; i++) {
            if (std::find(vecinosGenerados.begin(), vecinosGenerados.end(), i) != vecinosGenerados.end()) continue;
            auto costeActual = distance_matrix.distancia(anterior, i);
            if (costeActual < menorCoste) {
                menorCoste = costeActual;
                mejorVecino = i;
            }
        }
        menorCoste = INT32_MAX;
        anterior = mejorVecino;
        vecinosGenerados.push_back(mejorVecino);
        solucion.push_back(mejorVecino);
    }
}

std::tuple<int, int> generar_intercambio_ordinal(const std::deque<std::tuple<int,int>>& listaTabu) {
    static int i = 1, j = 0;
    static int vecinosPosibles = total_vecinos();


    if (vecinosPosibles == 0) {
        vecinosPosibles = total_vecinos();
        i = 1; j = 0;
        throw std::exception();
    }

    vecinosPosibles--;

    if (i == j) {
        i = ((i + 1) % (NUM_CIUDADES - 1));
        if (i == 0) i++;
        j = 0;
    }

    auto intercambio = std::make_tuple(i, j++);

    auto it = std::find_if(listaTabu.begin(), listaTabu.end(),
                           [intercambio](const std::tuple<int, int> &tup) {
                               return std::get<0>(tup) == std::get<0>(intercambio) &&
                                      std::get<1>(tup) == std::get<1>(intercambio);
                           });
    if (it != listaTabu.end()) {
        return generar_intercambio_ordinal(listaTabu);
    }
    return intercambio;
};

std::tuple<int, int> generar_intercambio(std::vector<std::tuple<int, int>>* intercambiosGenerados) {

    int randomA, randomB;
    if (!randomsFile.is_open()) {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(1, NUM_CIUDADES - 2);

        randomA = dist(mt);
        randomB = dist(mt);

        /*std::cout << "Randoms: " << randomA
                  << " : " << randomB << std::endl;*/
    } else {
        double A, B;
        randomsFile >> A;
        randomsFile >> B;

        randomA = (int)std::floor(A*9);
        randomB = (int)std::floor(B*9);
    }

    std::tuple<int, int> intercambio;
    if (randomA > randomB)
        intercambio = std::make_tuple(randomA, randomB);
    else if (randomA < randomB)
        intercambio = std::make_tuple(randomB, randomA);
    else {
        randomA = ((randomA + 1) % (NUM_CIUDADES - 1));
        if (randomA == 0) randomA++;
        randomB = 0;
        intercambio = std::make_tuple(randomA, randomB);
    }


    int vecinosPosibles = total_vecinos();
    std::vector<std::tuple<int, int>>::iterator it;
    do {
        vecinosPosibles--;
        it = std::find_if(intercambiosGenerados->begin(), intercambiosGenerados->end(),
                               [intercambio](const std::tuple<int, int> &tup) {
                                   return std::get<0>(tup) == std::get<0>(intercambio) &&
                                          std::get<1>(tup) == std::get<1>(intercambio);
                               });
        if (it != intercambiosGenerados->end()) { // Intercambio ya generado
            std::get<1>(intercambio) += 1;
            if (std::get<1>(intercambio) >= std::get<0>(intercambio)) {
                std::get<0>(intercambio) = ((std::get<0>(intercambio) + 1) % (NUM_CIUDADES-1));
                if (std::get<0>(intercambio) == 0) std::get<0>(intercambio)++;
                std::get<1>(intercambio) = 0;
            }
        }
        if (vecinosPosibles == 0) throw std::exception();
    } while (it != intercambiosGenerados->end());

    intercambiosGenerados->push_back(intercambio);
    return intercambio;
}

int total_vecinos () {
    return ((NUM_CIUDADES-1)*(NUM_CIUDADES-2))/2;
}

std::tuple<int, int, int> realizar_torneo(std::vector<Solution_vector> &poblacion) {
    int randomA, randomB;
    if (!randomsFile.is_open()) {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(1, TAM_POBLACION - 1);

        randomA = dist(mt);
        randomB = dist(mt);

    } else {
        double A, B;
        randomsFile >> A;
        randomsFile >> B;

        randomA = (int)std::floor(A * (TAM_POBLACION));
        randomB = (int)std::floor(B * (TAM_POBLACION));
    }

    auto contendiente1 = poblacion.at(randomA);
    auto contendiente2 = poblacion.at(randomB);

    auto ganador = randomA;
    if (contendiente2.cost < contendiente1.cost)
        ganador = randomB;

    return std::make_tuple(randomA, randomB, ganador);
}