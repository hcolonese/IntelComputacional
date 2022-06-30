

#include "Aresta.h"

Aresta::Aresta() {
    
}

Aresta::Aresta(int idAdj, float peso)
{
    this->idVerticeAdj = idAdj;
    this->peso = peso;
}

int Aresta::getIdAdj() const
{
    return idVerticeAdj;
}

float Aresta::getPeso() const
{
    return peso;
}
