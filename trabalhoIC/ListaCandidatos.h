#ifndef LISTACANDIDATOS_H
#define LISTACANDIDATOS_H

#include "Candidato.h"
#include "Vertice.h"
#include <vector>

class ListaCandidatos
{
private:
    
    
public:
    ListaCandidatos(vector<Vertice> &vertices);
    ~ListaCandidatos();
    vector<Candidato> candidatos;
    vector<int> foreverAloneIds;
    int qtdCandidatos;
    int qtdForeverAlone;
    void removeCandidato(int pos);
    void imprimir();
    bool hasCandidatos();
    bool isDisponivel(int id);
};

#endif // LISTACANDIDATOS_H
