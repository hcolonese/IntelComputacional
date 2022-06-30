#include "Grupo.h"



Grupo::~Grupo()
{
}


void Grupo::setId(unsigned int id) {
    this->id = id;
}

unsigned int Grupo::getId() const
{
    return id;
}

void Grupo::printVertices() {
    cout << "Grupo [" << id << "]: ";
    for(Vertice& it: vertices) {
        cout << "(" << it.getIdVertice() << ") Grau: " << it.getGrau() << " - ";
    }
    cout << endl;
}

void Grupo::addVertice(float** matriz, Vertice v) {
    custo+=v.getPeso();
    for(Vertice& it: vertices) {
        beneficio+= matriz[it.getIdVertice()][v.getIdVertice()];
    }
    vertices.push_back(v);
}

void Grupo::addVertice(float adicaoBeneficio, Vertice v) {
    custo+=v.getPeso();
    beneficio+=adicaoBeneficio;
    vertices.push_back(v);
}


void Grupo::removeVertice(float** matriz, Vertice v) {
    custo-=v.getPeso();
    int posV = -1;
    for(int i = 0; i < vertices.size(); i++) {
        if(vertices[i].getIdVertice() == v.getIdVertice()) {
            posV = i;
        }
        else beneficio-= matriz[vertices[i].getIdVertice()][v.getIdVertice()];
    }
    if(posV >= 0) {
        swap(vertices[posV], vertices[vertices.size()-1]);
        vertices.pop_back();
    }
}


void Grupo::removeVertice(float reducaoBeneficio, Vertice v) {
    custo-=v.getPeso();
    for(int i = 0; i < vertices.size(); i++) {
        if(vertices[i].getIdVertice() == v.getIdVertice()) {
            swap(vertices[i], vertices[vertices.size()-1]);
            vertices.pop_back();
            break;
        }
    }
    beneficio+=reducaoBeneficio;
}
/*
void Grupo::addAresta(Aresta a) {
    arestas.push_back(a);
}
*/
vector<Vertice> Grupo::getVertices() {
    return vertices;
}

void Grupo::printGrupo(bool showVertices) {
    cout << "Grupo [" << id << "]: Tamanho do grupo: " << vertices.size() << " - Custo: " << custo  << " - Beneficio: " << beneficio << endl;
    if(showVertices) {
        for(auto it: vertices) {
            cout << it.getIdVertice() << "-";
        }
        cout << endl;
    }
}

bool Grupo::isViavel() {
    return (lowerLimit <= custo && custo <= upperLimit);
}
