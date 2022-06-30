
#ifndef IC_DEFINICAO_GRUPOS_VERTICE_H
#define IC_DEFINICAO_GRUPOS_VERTICE_H

#include <iostream>
#include <iomanip>
#include <list>
#include <string>
#include <vector>
#include "Aresta.h"

using namespace std;

class Vertice
{
private:
    list<Aresta> adj;
    unsigned int grau;

public:
    float peso;
    int id;
    void setId(int id);

    float beneficio;
    Vertice();
    Vertice(int id, float peso);
    int getIdVertice() const;
    float getPeso() const;
    bool procuraAdjacencia(int vert);
    //unsigned int getGrau();
    void imprimeListAdjacencia(bool ponderado);
    unsigned int contaAdjacencia(int vert);
    list<Aresta>& getAdjacencia() ;
    void adicionaAresta(Vertice &vert, double peso);//, bool digrafo);
    unsigned int getGrau();
    string retornaListAdjacencia(bool ponderado);

    //Sobrecarga de operadores
    bool operator<(const Vertice &rhs) const;
    bool operator>(const Vertice &rhs) const;
    bool operator<=(const Vertice &rhs) const;
    bool operator>=(const Vertice &rhs) const;
    bool operator==(const int &rhs) const;
    bool operator==(const Vertice &rhs) const;
    bool operator!=(const Vertice &rhs) const;

};


#endif //IC_DEFINICAO_GRUPOS_VERTICE_H
