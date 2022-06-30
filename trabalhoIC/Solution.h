

#ifndef IC_DEFINICAO_GRUPOS_SOLUTION_H
#define IC_DEFINICAO_GRUPOS_SOLUTION_H

#include <vector>
#include "Grafo.h"
#include "Grupo.h"

using namespace std;

class Solution
{



public:
    float alfa;
    int semente;
    float cost;
    float benefit;
    vector<Grupo> grupos;
    Solution() {}
    Solution(unsigned int numGroups);
    void printSolution();
    float getValorSolucaoFinal();
    float calculaBeneficio(float **matriz);
    bool verificaViabilidade(Grafo& graph);
    bool verificaUpperLimit(Grafo &graph);
    bool verificaLowerLimit(Grafo &graph);
    string retornaSolucao();
};


#endif //IC_DEFINICAO_GRUPOS_SOLUTION_H
