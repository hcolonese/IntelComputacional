
#include "Grafo.h"



//Já cria o grafo lendo do arquivo de entrada

Grafo::Grafo(string path, int instanceType)
{
    ofstream outFile;
    ifstream inFile;
    string inFileName;
    string outFileName;


    inFile.open(path);

    if (inFile)
    {
        cout << "\tLendo arquivo instancia" << endl;
        cout << "\tPath (caminho): " << path << endl;

        // get length of file:
        inFile.seekg(0, std::ifstream::end);
        auto length = static_cast<unsigned int>(inFile.tellg());
        inFile.seekg(0, std::ifstream::beg);

        //criando e inicializando buffer
        //buffer contem o arquivo inteiro
        auto *buffer = new char[length];
        inFile.read(buffer, length);
        inFile.close();



        if (instanceType == 1)
            instanceTypeOneRead(buffer);//instanceTypeOneRead(inFile);
        else if (instanceType == 2)
            instanceTypeTwoRead(buffer);
        else
        {
            cout << "Tipo de instancia informado: " << instanceType << endl;
            cout << "Digite o tipo de instancia correntamente: 1 ou 2" << endl;
            exit(1);
        }

        criaTodasArestas();
        delete[] buffer;
        return;
    }
    cout << "\tFalha na leitura do arquivo!" << endl;
    cout << "\tO caminho tentado foi: \"" << path << "\"." << endl;
    cout << "\tVERIFIQUE se digitou o nome do diretorio path corretamente." << endl;
    exit(0);
}

void Grafo::instanceTypeOneRead(char *buffer)
{
    stringstream stream;

    cout << "\tLendo Instancia\n";
    char *p = strtok(buffer, "\n");

    stream << p;

    string temp;

    stream >> this->ordem >> this->groups >> temp >> this->lowerLimit >> this->upperLimit;

    criaMatrizPeso();

    for (int i = 0; i < (groups * 2) - 1; ++i) stream >> temp;

    int contVertices = 0;
    while (stream >> temp && temp != "\n")
    {
        this->vertices.emplace_back(contVertices, stof(temp));
        contVertices++;
    }

    int vert1, vert2;
    float peso;
    while (p != nullptr)
    {
        p = strtok(nullptr, "\n");
        stream.clear();
        stream << p;
        stream >> vert1 >> vert2 >> peso;
        matrizDistancia[vert1][vert2] = peso;
        matrizDistancia[vert2][vert1] = peso;
    }
}

void Grafo::instanceTypeTwoRead(char *buffer)
{
    stringstream stream;

    cout << "\tLendo Instancia\n";

    char *p = strtok(buffer, "\n");
    this->ordem = static_cast<unsigned int>(stoi(p));

    p = strtok(nullptr, "\n");
    this->groups = static_cast<unsigned int>(stoi(p));

    p = strtok(nullptr, "\n");
    this->upperLimit = stof(p);
    this->lowerLimit = 0;

    criaMatrizPeso();

    for (int i = 0; i < this->ordem; ++i)
    {
        p = strtok(nullptr, "\n");
        this->vertices.emplace_back(i, stof(p));
    }

    p = strtok(nullptr, "\n");
    stream << p;

    for (int i = 0; i < this->ordem; ++i)
    {
        for (int j = 0; j < this->ordem; ++j)
        {
            stream >> matrizDistancia[i][j];
        }
    }
}

void Grafo::criaMatrizPeso()
{
    matrizDistancia = new float *[this->ordem];

    for (int i = 0; i < this->ordem; i++)
    {
        matrizDistancia[i] = new float[this->ordem];
        for (int j = 0; j < this->ordem; j++)
        {
            matrizDistancia[i][j] = 0;
        }
    }
}

void Grafo::criaTodasArestas()
{
    for (int i = 0; i < ordem; ++i)
    {
        for (int j = 0; j < ordem; ++j)
        {
            if (matrizDistancia[i][j] != 0)
            {
                vertices[i].adicionaAresta(vertices[j], matrizDistancia[i][j]);
            }
        }
    }
}



/*
list<Vertice>::iterator Grafo::adicionaVertice(int id, int x, int y, int corPB)
{
    auto itVertice = getVertice(id);
    if(itVertice->getIdVertice() != id || vertices.empty()) //Se nao encontrar tem que adicionar
    {
        vertices.emplace_back(id, x, y, corPB);//push_back(v);
        ordem++;
        return --vertices.end(); //retorna o ultimo vertice, o que foi adicionado
    }
    else
    {
        return itVertice; //retorna o vertice encontrado, que ja foi adicionado

    }
}

 */



void Grafo::printAdjacenceList()
{
    cout.precision(2);
    cout.setf(ios::fixed);
    cout << endl << "-----------------------------------" << endl;
    cout << "Imprimindo o grafo:" << endl;
    cout << "Estilo [NodeId|NodeCost] -> ..Adjacencia|Peso.." << endl;
    cout << "Vertices - Adjacencias\n";
    for(auto it : vertices)
    {
        cout << "[" << it.getIdVertice() << "|" << it.getPeso() <<"]  ->   ";
        it.imprimeListAdjacencia(true);
        cout << endl;
    }
    cout << "-----------------------------------" << endl << endl;
    cout.precision(8);
}



unsigned int Grafo::getOrdem()
{
    return ordem;
}


/*

list<Vertice>::iterator Grafo::getVertice(int idVert)
{
    list<Vertice>::iterator it;
    for(it = vertices.begin(); it != vertices.end(); it++)
        if(it->getIdVertice() == idVert)
            break;
    if(it == vertices.end())
        return --it;
    return it;
}
*/


void Grafo::imprimeMatrizDistancia()
{
    cout << "Vert|";
    cout << fixed;
    cout.precision(2);
    cout.setf(ios::fixed);
    for (int i = -1; i < this->ordem; i++)
    {
        for (int j = -1; j < this->ordem; j++)
        {
            if (i == -1)
                cout << setw(10) << j + 1;
            else
            {
                if (j == -1)
                    cout << i << "   |";
                else
                    cout << setw(10) << matrizDistancia[i][j];
            }
        }
        cout << endl;
    }
    cout.precision(8);
}

unsigned int Grafo::getNumGroups() const
{
    return groups;
}
