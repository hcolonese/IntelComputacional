#include "ListaCandidatos.h"

ListaCandidatos::ListaCandidatos(vector<Vertice> &vertices)
{
    for(Vertice& vert: vertices) {
        if(vert.getGrau() < 1) {
            foreverAloneIds.push_back(vert.getIdVertice());
        }
        candidatos.emplace_back();
        candidatos[vert.getIdVertice()].v = vert;
        candidatos[vert.getIdVertice()].isDisponivel = true;
    }
    qtdCandidatos = candidatos.size();
    qtdForeverAlone = foreverAloneIds.size();
}

ListaCandidatos::~ListaCandidatos()
{
}


void ListaCandidatos::imprimir() {
    for(int i = 0; i < candidatos.size(); i++) {
        cout << "pos: " << i << " idVert: " << candidatos[i].v.getIdVertice() << " isDisponivel: ";
        if(candidatos[i].isDisponivel) cout << "true" << endl;
        else cout << "false" << endl;
    }
}

void ListaCandidatos::removeCandidato(int pos) {
    candidatos[pos].isDisponivel = false;
    qtdCandidatos--;
}

bool ListaCandidatos::hasCandidatos() {
    return (qtdCandidatos > 0)? true: false;
}

bool ListaCandidatos::isDisponivel(int id) {
    return candidatos[id].isDisponivel;
}