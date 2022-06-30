

#ifndef IC_DEFINICAO_GRUPOS_GRAFO_H
#define IC_DEFINICAO_GRUPOS_GRAFO_H


#include "Vertice.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <climits>
#include <cmath>
#include <iomanip>
#include <random>
#include <cstring>



class Grafo
{
private:
    unsigned int ordem;
    unsigned int groups;
    float upperLimit;
    float lowerLimit;
    void criaMatrizPeso();
    void criaTodasArestas();

public:
    explicit Grafo(string path, int instanceType);
    void instanceTypeOneRead(char *buffer);
    void instanceTypeTwoRead(char *buffer);
    unsigned int getOrdem();
    unsigned int getNumGroups() const;
    void imprimeMatrizDistancia();
    void printAdjacenceList();
    vector<Vertice> vertices;
    float **matrizDistancia;
    float &getLowerLimit()  {return lowerLimit;}
    float &getUpperLimit() {return upperLimit;}

};


#endif //IC_DEFINICAO_GRUPOS_GRAFO_H
