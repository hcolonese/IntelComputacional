

#include <iostream>
#include "Solution.h"


Solution::Solution(unsigned int numgrupos)
{
    grupos.reserve(numgrupos);
    for(int i = 0; i < numgrupos; ++i)
    {
        grupos.emplace_back();
        grupos[i].setId(i);
    }
}

void Solution::printSolution()
{
    for(const auto& it : grupos)
    {
        cout << "Grupo "<< it.getId() << " [" << it.beneficio << "|" << it.custo <<"]: ";
        for(const auto& itVert : it.vertices)
            cout << itVert.getIdVertice() << " ";
        cout << endl;
    }
}

float Solution::getValorSolucaoFinal()
{
    float soma = 0;
    for(const auto& it : grupos)
    {
        soma += it.beneficio;
    }
    return soma;
}

float Solution::calculaBeneficio(float **matriz)
{
    float somaBeneficio = 0;
    for (auto &group : grupos)
    {
        for (int i = 0; i < group.vertices.size()-1; ++i)
        {
            for (int j = i+1; j < group.vertices.size(); ++j)
            {
                somaBeneficio += matriz[group.vertices[i].getIdVertice()][group.vertices[j].getIdVertice()];
            }
        }
    }

    return somaBeneficio;
}

bool Solution::verificaViabilidade(Grafo &graph)
{
    for (auto &group : grupos)
    {
        if(group.getCusto() < graph.getLowerLimit() || group.getCusto() > graph.getUpperLimit())
        {
            return false;
        }
    }
    return true;
}


bool Solution::verificaLowerLimit(Grafo &graph)
{
    for (auto &group : grupos)
    {
        if(group.custo < graph.getLowerLimit())
        {
            //cout <<" INVIAVEL";
            return false;
        }
    }
    return true;
}

bool Solution::verificaUpperLimit(Grafo &graph)
{
    for (auto &group : grupos)
    {
        if(group.custo > graph.getUpperLimit())
        {
            //cout <<" INVIAVEL";
            return false;
        }
    }
    return true;
}

string Solution::retornaSolucao()
{
    string s = "S = { ";
    for(const auto& g : grupos)
    {
        s += "{";
        for(const auto& v : g.vertices)
        {
            s += std::to_string(v.getIdVertice());
            s += ", ";
        }
        s.pop_back();
        s.pop_back();
        s += "}, ";
    }
    s.pop_back();
    s.pop_back();
    s += " }";

    return s;
}