

#ifndef IC_DEFINICAO_GRUPOS_HEURISTICS_H
#define IC_DEFINICAO_GRUPOS_HEURISTICS_H


#include <algorithm>
#include "Grafo.h"
#include "Solution.h"
#include "Grupo.h"
#include "ListaCandidatos.h"


class Heuristics
{
private: 
    static bool isDisponivel(vector<Vertice> &tempVertices, int &idVertice);
    static bool isViavel(Grupo &g, Vertice &vAdd);
    static int getVerticeById(vector<Vertice> &tempVertices, int &idVertice);
    static void calcularBeneficio(float** matriz, Grupo* g, int id);

public:
    static Solution VND_Local(Grafo &graph,Solution& solution, int maxIter); 
    static Solution VND_LocalArtigo(Grafo &graph,Solution& solution);
    static Solution VND_CCP(Grafo &graph, int maxIter);
    static Solution greedyAlgorithm(Grafo &graph, float alfa);
    static Solution greedyReativoAlgoritm(Grafo &grafo, clock_t timeStart);
    static Solution constructive(Grafo &graph, clock_t seed);
    static Solution& buscaLocal(Grafo& graph, Solution& solution, int numIter);
    static Solution &localSearch(Solution &s, Grafo &g);
    static void viabilizaSolucao(Solution &s, Grafo &g);
    // static int retornaAlfa (int QntAlfa, float vetProb[], int rand);
    static void buscaLocal_1to0(Grafo &g, Solution &s, float &melhora);
    static void buscaLocal_1to1(Grafo &g, Solution &s, float &melhora);
    static void buscaLocal_2to1(Grafo &g, Solution &s, float &melhora);
    static bool canAdd(const float &currentCostCand, const float &upperLimit, const float &currentCostGroup);
};


#endif //IC_DEFINICAO_GRUPOS_HEURISTICS_H
