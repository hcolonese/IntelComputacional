

#ifndef IC_DEFINICAO_GRUPOS_ARESTA_H
#define IC_DEFINICAO_GRUPOS_ARESTA_H


class Aresta
{
private:
    float peso;
    int idVerticeAdj;

public:
    Aresta();
    Aresta(int idAdj, float peso);
    int getIdAdj() const;
    float getPeso() const;

};


#endif //IC_DEFINICAO_GRUPOS_ARESTA_H
