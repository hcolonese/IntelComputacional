#ifndef IC_DEFINICAO_GRUPOS_GRUPO_H
#define IC_DEFINICAO_GRUPOS_GRUPO_H

#include <vector>
#include "Vertice.h"
#include "Aresta.h"
#include "Grafo.h"



class Grupo
{
private:
    unsigned int id;
public:
    vector<Vertice> vertices;
    float beneficio;
    float custo;
    float lowerLimit;
    float upperLimit;


    Grupo() : custo(0), beneficio(0){}
    ~Grupo();
    void setId(unsigned int id);
    unsigned int getId() const;
    void printVertices();
    void addVertice(float** matriz, Vertice v);
    void addVertice(float adicaoBeneficio, Vertice v); // usado na busca local
    vector<Vertice> getVertices();
    void printGrupo(bool showVertices);
    bool isViavel();
    void removeVertice(float** matriz, Vertice v);
    void removeVertice(float reducaoBeneficio, Vertice v); // usado na busca local
    
    Grupo& setLowerLimit(float lowerLimit) {this->lowerLimit = lowerLimit; return *this;}
    Grupo& setUpperLimit(float upperLimit) {this->upperLimit = upperLimit; return *this;}
    float getLowerLimit() const {return lowerLimit;}
    float getUpperLimit() const {return upperLimit;}

    Grupo& setBeneficio(float beneficio) {this->beneficio = beneficio; return *this;}
    Grupo& setCusto(float custo) {this->custo = custo; return *this;}
    float getBeneficio() const {return beneficio;}
    float getCusto() const {return custo;}




};

#endif // IC_DEFINICAO_GRUPOS_GRUPO_H
