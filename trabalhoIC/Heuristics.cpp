

#include "Heuristics.h"

struct Cand
{
    int id;
    float beneficio;
    float custo;
    int groupInsertionId;
    bool operator==(const int &rhs) const
    {
        return id == rhs;
    }
};

struct Conj
{
    Vertice vert;
    float beneficio;
    int groupId;
    public:
    static bool compareConjByBene(Conj &a, Conj &b) {
        return a.beneficio < b.beneficio;
    }

}typedef Conj;

struct
{
    bool operator()(Vertice &cand1, Vertice &cand2) const
    {
        return cand1.beneficio > cand2.beneficio;
    }

} sortCand;

struct {
    bool operator()(Vertice &c1, Vertice &c2) const {
        return c1.peso > c2.peso;
    }

    bool operator()(Cand &c1, Cand &c2) const {
        return c1.custo > c2.custo;
    }
} sortPesoCand;

struct {
    bool operator()(Grupo &g1, Grupo &g2) const {
        return g1.getCusto() < g2.getCusto();
    }
} sortGruposByCusto;

struct {
    bool operator()(Grupo &g1, Grupo &g2) const {
        return g1.getBeneficio() < g2.getBeneficio();
    }
} sortGruposByBeneficio;

Solution Heuristics::constructive(Grafo &graph, clock_t seed) {
    cout << "-------------------------------" << endl;
    
    ListaCandidatos listaCandidatos(graph.vertices);

    
    unsigned int qtdGrupos = graph.getNumGroups();
    
    // Criação dos grupos
    Solution solution(graph.getNumGroups());
    for(unsigned int i = 0; i < qtdGrupos; i++) {
        solution.grupos[i].setId(i);
        solution.grupos[i].setLowerLimit(graph.getLowerLimit());
        solution.grupos[i].setUpperLimit(graph.getUpperLimit());
    }

    int i = -1;
    srand(seed);
    // Inserção de 1 vertice aleatorio em cada grupo
    for(Grupo& it: solution.grupos) {
        while(i < 0) {
            i = (int) (listaCandidatos.qtdCandidatos * (rand() / (RAND_MAX + 1.0)));
            if(!listaCandidatos.isDisponivel(i) || listaCandidatos.candidatos[i].v.getGrau() < 1) {
                i = -1;
            }
        }
        it.addVertice(graph.matrizDistancia, listaCandidatos.candidatos[i].v);
        listaCandidatos.removeCandidato(i);
        i = -1;
    }

    unsigned int idVerticeOrigem = -1;
    float maiorBeneficio = -1;
    unsigned int idMelhorVertice = -1;
    int iterCont = 0;
    while(listaCandidatos.hasCandidatos()) {
        if(iterCont > solution.grupos.size()) {
            cout << "Sobraram " << listaCandidatos.qtdCandidatos << " candidatos na lista... corrigindo" << endl;
            break;
        }
        for(Grupo& g: solution.grupos) {
            if(!listaCandidatos.hasCandidatos()) {
                break;
            }
            for(Vertice& v: g.getVertices()) {
                for(Aresta& ar: v.getAdjacencia()) {
                    if(ar.getPeso() > maiorBeneficio && listaCandidatos.isDisponivel(ar.getIdAdj())) {
                        if(isViavel(g, listaCandidatos.candidatos[ar.getIdAdj()].v)) {
                            idVerticeOrigem = v.getIdVertice();
                            maiorBeneficio = ar.getPeso();
                            idMelhorVertice = ar.getIdAdj();
                        }
                    }
                }
            }
            if(idVerticeOrigem == -1 || maiorBeneficio == 0 || idMelhorVertice == -1) {
                iterCont++;
            } else {
                g.addVertice(graph.matrizDistancia, listaCandidatos.candidatos[idMelhorVertice].v);
                listaCandidatos.candidatos[idMelhorVertice].idGrupo = g.getId();
                listaCandidatos.removeCandidato(idMelhorVertice);
                idVerticeOrigem = -1;
                maiorBeneficio = 0;
                idMelhorVertice = -1;
                iterCont = 0;
            }
        }
    }
    if(listaCandidatos.qtdCandidatos > 0) {
        iterCont = 0;
        int contCand = 0;
        sort(solution.grupos.begin(), solution.grupos.end(), sortGruposByCusto);
        vector<Cand> restantes;
        for(Candidato c: listaCandidatos.candidatos) {
            if(c.isDisponivel) {
                restantes.emplace_back();
                restantes[contCand].id = c.v.getIdVertice();
                restantes[contCand].custo = c.v.getPeso();
                contCand++;
            }
        }
        sort(restantes.begin(), restantes.end(), sortPesoCand);
        for(Cand& ca: restantes) {
            for(Grupo& g: solution.grupos) {
                if(isViavel(g, listaCandidatos.candidatos[ca.id].v)) {
                    g.addVertice(graph.matrizDistancia, listaCandidatos.candidatos[ca.id].v);
                    listaCandidatos.candidatos[ca.id].idGrupo = g.getId();
                    listaCandidatos.removeCandidato(ca.id);
                    sort(solution.grupos.begin(), solution.grupos.end(), sortGruposByCusto);
                    break;
                } else {
                    cout << "Nao encontrou grupo para o vertice... adicionando no ultimo grupo visitado" << endl;
                    g.addVertice(graph.matrizDistancia, listaCandidatos.candidatos[ca.id].v);
                    listaCandidatos.removeCandidato(ca.id);
                }
            }
        }
    }
    cout << endl << "Solucao construtivo: " << endl;
    float somaBeneficio = 0;
    bool isviavel = true;
    if(listaCandidatos.qtdCandidatos > 0) isviavel = false;
    for(Grupo& it: solution.grupos) {
        if(!it.isViavel()) {
            isviavel = false;
            cout << "Grupo nao viavel - ";
            it.printGrupo(false);
        }
        somaBeneficio+= it.getBeneficio();
    }
    cout << "Status da solucao: ";
    if(isviavel) cout << "Viavel";
    else cout << "Inviavel";
    cout << " - - - Beneficio total: " << somaBeneficio << " " << endl;
    buscaLocal(graph, solution, 10000);
    float oldBeneficio = somaBeneficio;
    cout << "Solucao construtivo + busca local" << endl;
    isviavel = true;
    somaBeneficio = 0;
    for(Grupo& it: solution.grupos) {
        if(!it.isViavel()) {
            isviavel = false;
            cout << "Grupo nao viavel - ";
            it.printGrupo(false);
        }
        somaBeneficio+= it.getBeneficio();
    }
    cout << "Status da solucao: ";
    if(isviavel) cout << "Viavel";
    else cout << "Inviavel";
    float melhoria = ((somaBeneficio-oldBeneficio)/oldBeneficio)*100;
    cout << " - - - Beneficio total: " << somaBeneficio << " --- Melhoria: "; cout.precision(2); cout << melhoria << "%"<< endl; cout.precision(6);
    return solution;
}

Solution& Heuristics::buscaLocal(Grafo &graph, Solution &solution, int numIter) {
    vector<int> idGruposInviaveis;
    for(Grupo& g: solution.grupos) {
        if(!g.isViavel()) {
            idGruposInviaveis.push_back(g.getId());
        }
    }
    int limiteIterComInviaveis = idGruposInviaveis.size()*30;
    int contIterComInviaveis = 0;
    float novoPesoG1;
    float novoPesoG2;
    float adicaoBeneficioG1;
    float adicaoBeneficioG2;
    float reducaoBeneficioG1;
    float reducaoBeneficioG2;
    // Tentativa de normalizar o numero de iterações baseado na ordem do grafo
    numIter *= (5-((480-graph.getOrdem())/480)*5);

    for(int i = 0; i < numIter; i++) {
        int g1 = 0;
        int g2 = 0;

        while(g1 == g2) {
            if(contIterComInviaveis>=limiteIterComInviaveis) {
                g1 = (int) (solution.grupos.size() * (rand() / (RAND_MAX + 1.0)));
                g2 = (int) (solution.grupos.size() * (rand() / (RAND_MAX + 1.0)));
            } else {
                contIterComInviaveis++;
                g1 = (int) idGruposInviaveis[(idGruposInviaveis.size() * (rand() / (RAND_MAX + 1.0)))];
                g2 = (int) (solution.grupos.size() * (rand() / (RAND_MAX + 1.0)));
            }
        }

        Vertice v1 = solution.grupos[g1].getVertices()[(int) (solution.grupos[g1].getVertices().size() * (rand() / (RAND_MAX + 1.0)))];
        Vertice v2 = solution.grupos[g2].getVertices()[(int) (solution.grupos[g2].getVertices().size() * (rand() / (RAND_MAX + 1.0)))];

        bool inviabInicial = !solution.grupos[g1].isViavel() || !solution.grupos[g2].isViavel();

        novoPesoG1 = solution.grupos[g1].custo-v1.getPeso()+v2.getPeso();
        novoPesoG2 = solution.grupos[g2].custo-v2.getPeso()+v1.getPeso();
        adicaoBeneficioG1 = 0;
        adicaoBeneficioG2 = 0;
        reducaoBeneficioG1 = 0;
        reducaoBeneficioG2 = 0;
        for(Vertice& v: solution.grupos[g1].vertices) {
            if(v.getIdVertice() != v1.getIdVertice()) {
                reducaoBeneficioG1-=graph.matrizDistancia[v.getIdVertice()][v1.getIdVertice()];
                adicaoBeneficioG1+=graph.matrizDistancia[v.getIdVertice()][v2.getIdVertice()];
            }
        }
        for(Vertice& v: solution.grupos[g2].vertices) {
            if(v.getIdVertice() != v2.getIdVertice()) {
                reducaoBeneficioG2-=graph.matrizDistancia[v.getIdVertice()][v2.getIdVertice()];
                adicaoBeneficioG2+=graph.matrizDistancia[v.getIdVertice()][v1.getIdVertice()];
            }
        }
        if((solution.grupos[g1].lowerLimit <= novoPesoG1 && novoPesoG1 <= solution.grupos[g1].upperLimit && solution.grupos[g2].lowerLimit <= novoPesoG2 && novoPesoG2 <= solution.grupos[g2].upperLimit) || inviabInicial) {
            if(((adicaoBeneficioG1+reducaoBeneficioG1)+(adicaoBeneficioG2+reducaoBeneficioG2)) > 0) {
                solution.grupos[g1].removeVertice(reducaoBeneficioG1, v1);
                solution.grupos[g2].removeVertice(reducaoBeneficioG2, v2);
                solution.grupos[g1].addVertice(adicaoBeneficioG1, v2);
                solution.grupos[g2].addVertice(adicaoBeneficioG2, v1);
            }
        }
    }

    viabilizaSolucao(solution, graph);
    return solution;
}

void Heuristics::calcularBeneficio(float** matriz, Grupo* g, int id) {
    float beneficioExtra = 0;
    for(Vertice& v: g->getVertices()) {
        beneficioExtra+= matriz[v.getIdVertice()][id];
    }
    g->setBeneficio(g->getBeneficio() + beneficioExtra);
}

int Heuristics::getVerticeById(vector<Vertice> &tempVertices, int &idVertice) {
    for(int i = 0; i < tempVertices.size(); i++) {
        if(tempVertices[i].getIdVertice() == idVertice) {
            return i;
        }
    }
    return -1;
}

bool Heuristics::isDisponivel(vector<Vertice> &tempVertices, int &idVertice) {
    for(Vertice& v: tempVertices) {
        if(v.getIdVertice() == idVertice) {
            return true;
        }
    }
    return false;
}

bool Heuristics::isViavel(Grupo &g, Vertice &vAdd) {
    return ((g.getCusto()+vAdd.getPeso()) <= g.getUpperLimit());
}

Solution Heuristics::greedyRandAlgorithm(Grafo &graph, float alfa, int semente)
{
    // cria vetor de tamanho ordem do grafo
    vector<Vertice> candidates(graph.getOrdem());
    Solution solution(graph.getNumGroups());
    srand(semente);
    float bestBenefitOutro=0;
    // seta as arestas e pesos dos vertices
    for (int i = 0; i < graph.getOrdem(); ++i)
    {
        for(const auto& itAdj : graph.vertices[i].getAdjacencia())
        {
            bestBenefitOutro = itAdj.getPeso();
            candidates[i].beneficio+=itAdj.getPeso();
        }
        candidates[i].id = i;
        candidates[i].peso = graph.vertices[i].getPeso();
    }
    // ordena por beneficio
    sort(candidates.begin(), candidates.end(), sortCand);

    // adiciona um primeiro vertice em cada grupo
    for (unsigned int i = 0; i < graph.getNumGroups(); ++i)
    {
        solution.grupos.at(i).vertices.push_back(candidates[0]);
        solution.grupos.at(i).custo = candidates[0].peso;
        candidates.erase(candidates.begin());
    }
    Vertice bestVerticeId;
    // flag para mostrar que ele n pode ser adicionado em nenhum grupo
    bestVerticeId.setId(-1);
    int bestGroup = 0;
    float bestBenefit = 0;
    vector<Conj> total;
    while (!candidates.empty())
    {
        for(const Vertice& itCand : candidates)
        {
            for (const auto& itGroup : solution.grupos)
            {

                for (const auto& itVert : itGroup.vertices)
                {
                    if(/*graph.matrizDistancia[itCand.getIdVertice()][itVert.getIdVertice()] > bestBenefit &&*/ canAdd(itCand.peso, graph.getUpperLimit(), itGroup.custo))
                    {
                        // coloca todos em um vetor
                        Conj aux;
                        aux.vert=itCand;
                        aux.beneficio=graph.matrizDistancia[itCand.getIdVertice()][itVert.getIdVertice()];;
                        aux.groupId=itGroup.getId();
                        total.push_back(aux);
                        // bestVerticeId = itCand;
                        // bestBenefit = graph.matrizDistancia[itCand.getIdVertice()][itVert.getIdVertice()];
                        // bestGroup = itGroup.getId();
                    }
                }

            }
        }
        // falta apagar o vetor total todo para resetar
        // ordenando vetor geral
        sort(total.begin(), total.end(), Conj::compareConjByBene);
        // pega o sorteio de alfa
        int itAlfa =  alfa*total.size();
        int indice;
        if(itAlfa==0){
            indice=0;
        }
        else
            indice = rand() %itAlfa;
        // salva em bestvertice
        bestVerticeId = total[indice].vert;
        bestBenefit = total[indice].beneficio;
        bestGroup = total[indice].groupId;
        total.erase(total.begin()+indice);
        if(bestBenefit == 0 || bestVerticeId.getIdVertice() == -1)
        {
            unsigned int indiceInsercao = 0;
            float custo = solution.grupos[0].custo;
            // busca pelo grupo de menor custo
            for (unsigned int i = 1; i < solution.grupos.size(); ++i)
            {
                if(solution.grupos[i].custo < custo)
                {
                    custo = solution.grupos[i].custo;
                    indiceInsercao = i;
                }
            }
            bestGroup = indiceInsercao;
            // se n puder seradicionado em nenhum lugar
            if(bestVerticeId.getIdVertice() == -1)
            {
                indiceInsercao = 0;
                custo = candidates[0].peso;
                for (unsigned int i = 1; i < candidates.size(); ++i)
                {
                    if(candidates[i].peso< custo)
                    {
                        indiceInsercao = i;
                        custo = candidates[i].peso;
                    }
                }
                bestVerticeId = candidates[indiceInsercao];
            }
        }
        solution.grupos[bestGroup].vertices.push_back(bestVerticeId);
        solution.grupos[bestGroup].custo+=graph.vertices[bestVerticeId.getIdVertice()].getPeso();//candidates[bestVerticeId].custo;
        for(auto& it : solution.grupos[bestGroup].vertices)
        {
            solution.grupos[bestGroup].beneficio+=graph.matrizDistancia[bestVerticeId.getIdVertice()][it.getIdVertice()];
        }
        candidates.erase(remove(candidates.begin(), candidates.end(), bestVerticeId.getIdVertice()), candidates.end());
        bestVerticeId.setId(-1);
        bestBenefit = 0;

    }

    return solution;
}

Solution Heuristics::greedyReativoAlgoritm(Grafo &graph, clock_t timeStart)
{
    // Setando resultado em decimal
    cout.setf(ios::fixed);
    Solution bestSolution = constructive(graph, clock());
    Solution guloso = greedyRandAlgorithm(graph, 0, 0);
    // Tenta viabilizar, caso não seja possível, pega outra solução
    viabilizaSolucao(bestSolution, graph);
    if(!bestSolution.verificaViabilidade(graph))
    {
        bestSolution = greedyRandAlgorithm(graph, 0, 0);
    }
    else
        
        if(bestSolution.getValorSolucaoFinal()< guloso.getValorSolucaoFinal()){
            bestSolution = guloso;
        }
    buscaLocal(graph, bestSolution, 2000);
    cout << (double)(clock() - timeStart)/CLOCKS_PER_SEC << "s" << endl;
    Solution currentSolution;
    // Solution sol0,sol1,sol2,sol3,sol4,sol5,sol6,sol7,sol8,sol9;
    const int numAlfas = 10;
    int indiceAlfaAtual;
    const float vetAlfas[numAlfas] = {0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50};
    int ordem = graph.getOrdem();
    const int timeExec= ordem/4; 
    // int iteAlfas=0;

    while((double)(clock() - timeStart)/CLOCKS_PER_SEC <= timeExec)
    {
        // adaptar aqui para o randomizado reativo
        // sol0 = greedyAlgorithm(graph, vetAlfas[0], 0);
        // sol1 = greedyAlgorithm(graph, vetAlfas[1], 0);
        // sol2 = greedyAlgorithm(graph, vetAlfas[2], 0);
        // sol3 = greedyAlgorithm(graph, vetAlfas[3], 0);
        // sol4 = greedyAlgorithm(graph, vetAlfas[4], 0);
        // sol5 = greedyAlgorithm(graph, vetAlfas[5], 0);
        // sol6 = greedyAlgorithm(graph, vetAlfas[6], 0);
        // sol7 = greedyAlgorithm(graph, vetAlfas[7], 0);
        // sol8 = greedyAlgorithm(graph, vetAlfas[8], 0);
        // sol9 = greedyAlgorithm(graph, vetAlfas[9], 0);
        currentSolution = greedyRandAlgorithm(graph, vetAlfas[2], 0);
        buscaLocal(graph, currentSolution, 10000);
        if(currentSolution.verificaViabilidade(graph) && (currentSolution.getValorSolucaoFinal() > bestSolution.getValorSolucaoFinal()))
        {
            bestSolution = currentSolution;
        }
    }

    cout << endl << "Melhor solucao encontrada: " << (bestSolution.verificaViabilidade(graph) ? "" : "NAO ") << "VIAVEL!" << endl;
    cout << "Solucao final (beneficio): " << bestSolution.getValorSolucaoFinal() << endl;
    cout << "Solucao final calculada (beneficio): " << bestSolution.calculaBeneficio(graph.matrizDistancia)<< endl;
    return bestSolution;
}

bool Heuristics::canAdd(const float &currentCostCand, const float &upperLimit, const float &currentCostGroup)
{
    return (currentCostGroup+currentCostCand)<=upperLimit;
}

void Heuristics::viabilizaSolucao(Solution &s, Grafo &g)
{
    // Função de viabilização de solução, onde podem ser tentadas 2 movimentos
    // Se a invialibidade for por custo min, pega o vertice de maior custo do cluster de maior custo e insere no cluster
    // Se a invialibidade for por custo max, pega o vertice de menor custo do cluster de menor custo e insere no cluster
    unsigned int vib = 0;
    while (!s.verificaViabilidade(g))
    {
        if (!s.verificaLowerLimit(g))
        {
            for (auto &itGroup :s.grupos)
            {
                if (itGroup.custo < g.getLowerLimit())
                {
                    // Cluster de maior custo
                    float maiorCost = s.grupos[0].custo;
                    int posGrupo = 0;
                    for (int i = 1; i < s.grupos.size(); ++i)
                    {
                        if (s.grupos[i].custo > maiorCost)
                        {
                            posGrupo = i;
                            maiorCost = s.grupos[i].custo;
                        }
                    }

                    // Vertice de maior custo para alocar alocar no cluster de menor custo
                    maiorCost = s.grupos[posGrupo].vertices[0].getPeso();
                    int posVert = 0;
                    for (int i = 1; i < s.grupos[posGrupo].vertices.size(); ++i)
                    {
                        if (s.grupos[posGrupo].vertices[i].getPeso() > maiorCost)
                        {
                            posVert = i;
                            maiorCost = s.grupos[posGrupo].vertices[i].getPeso();
                        }
                    }
                    for (auto &it : itGroup.vertices)
                        itGroup.beneficio += g.matrizDistancia[s.grupos[posGrupo].vertices[posVert].getIdVertice()][it.getIdVertice()];
                    itGroup.custo += maiorCost;

                    for (auto &it : s.grupos[posGrupo].vertices)
                        s.grupos[posGrupo].beneficio -= g.matrizDistancia[s.grupos[posGrupo].vertices[posVert].getIdVertice()][it.getIdVertice()];
                    s.grupos[posGrupo].custo -= maiorCost;

                    Vertice vert = s.grupos[posGrupo].vertices[posVert];
                    itGroup.vertices.push_back(s.grupos[posGrupo].vertices[posVert]);

                    s.grupos[posGrupo].vertices.erase(
                            remove(s.grupos[posGrupo].vertices.begin(), s.grupos[posGrupo].vertices.end(),
                                   s.grupos[posGrupo].vertices[posVert].id), s.grupos[posGrupo].vertices.end());

                }
            }
        }

        if (!s.verificaUpperLimit(g))
        {
            for (auto &itGroup : s.grupos)
            {
                if (itGroup.custo > g.getUpperLimit())
                {
                    // Busca vertice de menor custo
                    auto vert = itGroup.vertices.begin();
                    for (auto itVert = itGroup.vertices.begin(); itVert != itGroup.vertices.end(); ++itVert)
                    {
                        if (itVert->getPeso() > vert->getPeso())
                        {
                            vert = itVert;
                        }
                    }
                    for (auto &it : itGroup.vertices)
                        itGroup.beneficio -= g.matrizDistancia[vert->getIdVertice()][it.getIdVertice()];
                    itGroup.custo -= vert->getPeso();

                    // Busca grupo com menor custo, para alocar o vértice
                    auto group = s.grupos.begin();
                    for(auto it = s.grupos.begin(); it != s.grupos.end(); ++it)
                    {
                        if (it->custo < group->custo)
                        {
                            group = it;
                        }
                    }

                    Vertice copyVert = vert.operator*();
                    itGroup.vertices.erase(remove(itGroup.vertices.begin(), itGroup.vertices.end(), copyVert.id), itGroup.vertices.end());
                    for (auto &it : group->vertices)
                        group->beneficio += g.matrizDistancia[copyVert.getIdVertice()][it.getIdVertice()];

                    group->custo += copyVert.getPeso();
                    group->vertices.push_back(copyVert);

                }
            }
        }

        vib++;
        if (vib > 100)
            break;
    }
}