#include <iostream>
#include <string>
#include <set>
#include "Grafo.h"
#include "Heuristics.h"

using namespace std;

/**----------------------------------------*
 ** Para fazer a execução e leitura da instancia
 ** Execute passando os seguintes paramentros
 ** ./executavel caminhaInstancia tipoInstancia
 ** Sendo que tem instancias do tipo 1 e 2
 ** Tipo 1: Grupos A, B e C
 ** Tipo 2: Grupo D
 **
 ** Compilar com: g++ *.cpp -o trabalho-ic
 **----------------------------------------*/


void writeToOutputFile(const string &outputFileName, const string &text, bool append);
void readSolutionAndQuantify(const string& instanceName, const string& fileOutName, Grafo& graph);


int main(int argc, char** argv)
{

    if(argc != 3)
    {
        cout << "Entrada errada." << endl;
        cout << "Modelo de entrada: ./executavel nomeInstancia tipoInstancia" << endl;
        return -1;
    }

    string instanceFileName = argv[1];

    // Starta clock
    clock_t tStart = clock();

    // Cria grafo
    Grafo grafo(instanceFileName, stoi(argv[2])); // argv[2] representa o tipo de instância de entrada


    cout << endl << "------------------------------------" << endl;
    Solution sol = Heuristics::greedyReativoAlgoritm(grafo, 0);
    // Solution sol2 = Heuristics::constructive(grafo, 0);
    cout << "solucao 1" << endl;
    cout << "-----------------" <<endl; 
    cout << sol.getValorSolucaoFinal() << endl;
    sol.printSolution();
    // cout << "solucao 2 " << endl;
    // cout << "-----------------" <<endl; 
    // cout << sol2.getValorSolucaoFinal() << endl;
    // sol2.printSolution();

    cout << "\tTempo gasto: " << fixed << (double) (clock() - tStart) / CLOCKS_PER_SEC << "s" << endl << endl;

    string saida = argv[1];
    saida += ".out";

    return 0;
}




void writeToOutputFile(const string &outputFileName, const string &text, bool append)
{
    ofstream writer;
    append ? writer.open(outputFileName, ios::app) : writer.open(outputFileName);
    if (writer.is_open())
    {
        writer << text << endl;
        writer.close();
    }
    else
        cout << "Falha ao escrever no arquivo \"" << outputFileName << "\"." << endl;
}

void readSolutionAndQuantify(const string& instanceName, const string& fileOutName, Grafo& graph)
{
    //Leitura do arquivo de entrada que foi salvo a solucao
    ifstream inFile;
    inFile.open(fileOutName);

    // pega tamanho do arquivo, para ler e salvar tudo no buffer
    inFile.seekg(0, std::ifstream::end);
    auto length = static_cast<unsigned int>(inFile.tellg());
    inFile.seekg(0, std::ifstream::beg);
    //criando e inicializando buffer
    //buffer contem o arquivo inteiro
    auto *buffer = new char[length];
    inFile.read(buffer, length);
    inFile.close();

    //Divide em linhas
    char *p = strtok(buffer, "\n");
    //Pega proxima linha
    strtok(nullptr, "\n");
    p = strtok(nullptr, "\n");
    //Pega valor da sulucao que foi salvo
    string valorSolucao = p;


    stringstream stream;
    string strAux;
    int vertAux;
    vector<vector<int>> grupos;

    // Pega linha da solução
    p = strtok(nullptr, "\n");
    // Reparte em grupos
    char *p2 = strtok(p, "}");
    while(p2)
    {
        vector<int> matLine;
        for (int i = 0; p2[i] != '\0' || !strAux.empty(); ++i)
        {
            if(p2[i] >= '0' && p2[i] <= '9')
                strAux.push_back(p2[i]);
            else if(p2[i] == ',' || p2[i] == '}' || p2[i] == '\0')
            {
                if(!strAux.empty())
                {
                    vertAux = stoi(strAux);
                    matLine.push_back(vertAux);

                }
                if(p2[i] == '\0')
                    --i;
                strAux.clear();
            }
        }
        if(!matLine.empty())
            grupos.push_back(matLine);
        p2 = strtok(nullptr, "}");
    }


    float soma = 0;
    vector<float> somaCustoGrupos(grupos.size(), 0);
    int contaVertices = 0;
    set<int> uniqueElements;
    for (int i = 0; i < grupos.size(); ++i)
    {
        for (int j = 0; j < grupos[i].size(); ++j)
        {
            contaVertices++;
            somaCustoGrupos[i]+=graph.vertices[grupos[i][j]].getPeso();
            uniqueElements.insert(grupos[i][j]);
            for (int k = j; k < grupos[i].size(); ++k)
            {
                int vert1 = grupos[i][j];
                int vert2 = grupos[i][k];
                soma += graph.matrizDistancia[vert1][vert2];
            }
        }
    }

    cout << "Instancia: " << instanceName << endl;
    cout << "Beneficio informado: " << valorSolucao << endl;
    cout << "Beneficio calculado: " << soma << endl;
    cout << "Numero de Vertices: " << contaVertices << endl;
    cout << "Numero de elementos unicos: " << uniqueElements.size() << endl;
    cout << "Numero de Grupos: " << grupos.size() << endl;
    cout << "Custo dos Grupos:" << endl;
    for (int i = 0; i < somaCustoGrupos.size(); ++i)
    {
        cout << "Grupo " << i << ": " << somaCustoGrupos[i] << endl;
    }
    bool inviavel = false;
    float upLimit = graph.getUpperLimit();
    float lowLimit = graph.getLowerLimit();
    for (int i = 0; i < somaCustoGrupos.size(); ++i)
    {
        if(somaCustoGrupos[i] < lowLimit || somaCustoGrupos[i] > upLimit)
        {
            cout << "Inviavel no grupo " << i << "." << endl;
            inviavel = true;
        }
    }
    if(inviavel)
        cout << "SOLUCAO INVIAVEL!!!!!!!!!!!" << endl;
    else
        cout << "SOLUCAO VIAVEL!!!!!!!!!!!" << endl;


}
