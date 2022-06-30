
#include "Vertice.h"

Vertice::Vertice(){ grau = 0; }//costSumEdges=0;}

Vertice::Vertice(int id, float peso)
{
    //costSumEdges=0;
    grau = 0;
    this->id = id;
    this->peso = peso;
}


void Vertice::adicionaAresta(Vertice &vert, double peso)//, bool digrafo) //metodo para adicionar aresta em grafo nao orientado
{
    adj.emplace_back(vert.getIdVertice(), peso);
    this->grau++;
}

/*
bool Vertice::removeAresta(list<Vertice>::iterator vert, bool digrafo)
{
    int id = vert->getIdVertice();
    for(auto k = adj.begin(); k != adj.end(); k++) //auto k é a mesma coisa que list<Aresta>::iterator k
        if(k->getIdAdj() == id)
        {
            adj.erase(k);
            vert->diminuiGrauEntrada();
            if(digrafo)
                grauSaida--;
            return true;
        }
    return false;
}
*/
/*
unsigned int Vertice::getGrau()
{
    return grauEntrada+grauSaida;
}
*/

unsigned int Vertice::getGrau()
{
    return  grau;
}

bool Vertice::procuraAdjacencia(int idVert) //Retorna true caso ache uma adjacencia com o vertice de id "idVert"
{
    for(auto k = adj.begin(); k != adj.end(); k++)
        if(k->getIdAdj() == idVert)
            return true;
    return false;
}

unsigned int Vertice::contaAdjacencia(int vert)
{
    unsigned int cont = 0;
    for(auto k = adj.begin(); k != adj.end(); k++)
        if(k->getIdAdj() == vert)
            cont++;
    return cont;
}

void Vertice::imprimeListAdjacencia(bool ponderado)
{
    for(auto k = adj.begin(); k != adj.end(); k++)
    {
        ponderado ? cout << right << setw(2) << k->getIdAdj() << "|" << k->getPeso() << fixed : cout << setw(2) << k->getIdAdj();
        if(++k != adj.end())
        {
            cout << " ";
        }
        k--;
    }
}

string Vertice::retornaListAdjacencia(bool ponderado)
{
    string salvaDados;
    for(auto k = adj.begin(); k != adj.end(); k++)
    {
        if(ponderado)
            salvaDados += to_string(k->getIdAdj()) + "|" + to_string(k->getPeso());
        else
            salvaDados += to_string(k->getIdAdj());
        if(++k != adj.end())
            salvaDados += " ";
        k--;
    }
    return salvaDados;
}


list<Aresta>& Vertice::getAdjacencia()
{
    return adj;
}

int Vertice::getIdVertice() const
{
    return id;
}

bool Vertice::operator<(const Vertice &rhs) const {
    return id < rhs.id;
}

bool Vertice::operator>(const Vertice &rhs) const {
    return rhs < *this;
}

bool Vertice::operator<=(const Vertice &rhs) const {
    return !(rhs < *this);
}

bool Vertice::operator==(const int &rhs) const
{
    return id == rhs;
}

bool Vertice::operator>=(const Vertice &rhs) const {
    return !(*this < rhs);
}

float Vertice::getPeso() const
{
    return peso;
}

void Vertice::setId(int id)
{
    Vertice::id = id;
}

bool Vertice::operator==(const Vertice &rhs) const
{
    return id == rhs.id;
}

bool Vertice::operator!=(const Vertice &rhs) const
{
    return !(rhs == *this);
}
