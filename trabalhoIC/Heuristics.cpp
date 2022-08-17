

#include "Heuristics.h"
#include "Utils.h"

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
    buscaLocal(graph, solution, 100);
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

void retiraBeneficio(vector<Vertice> &candidates, int idVertOut, float **matrizBeneficio)
{
    for(auto& cand : candidates)
    {
        cand.beneficio -= matrizBeneficio[cand.id][idVertOut];
    }
}

Solution  Heuristics::greedyAlgorithm(Grafo &graph, float alfa)
{

    vector<Vertice> candidates(graph.getOrdem());
    Solution *solution = new Solution(graph.getNumGroups());
    solution->semente = Utils::seed;
    solution->alfa = alfa;

    for(unsigned int i = 0; i < graph.getNumGroups(); i++) {
        solution->grupos[i].setId(i);
        solution->grupos[i].setLowerLimit(graph.getLowerLimit());
        solution->grupos[i].setUpperLimit(graph.getUpperLimit());
    }



    //float bestBenefitOutro=0;
    for (int i = 0; i < graph.getOrdem(); ++i)
    {
        candidates[i].beneficio = 0;
        for(const auto& itAdj : graph.vertices[i].getAdjacencia())
        {
            //bestBenefitOutro = itAdj.getPeso();
            candidates[i].beneficio+=itAdj.getPeso();
        }
        candidates[i].id = i;
        candidates[i].peso = graph.vertices[i].getPeso();
    }



    // escolhendo o primeiro vertice de cada grupo
    for (unsigned int i = 0; i < graph.getNumGroups(); ++i)
    {
        sort(candidates.begin(), candidates.end(), sortCand);

        auto escopoInsercaoMax = static_cast<int>(floor(candidates.size() * alfa));
        std::uniform_int_distribution<> generator(0, escopoInsercaoMax);
        unsigned int indiceVert = generator(Utils::rand);

        solution->grupos.at(i).vertices.push_back(candidates[indiceVert]);
        solution->grupos.at(i).custo = candidates[indiceVert].peso;
        retiraBeneficio(candidates, (candidates.begin()+indiceVert)->id, graph.matrizDistancia);
        candidates.erase(candidates.begin()+indiceVert);

    }


    Vertice bestVerticeId;
    bestVerticeId.setId(-1);
    int bestGroup = 0;
    float sumBenefitAuxiliar = 0;

    float beneficioMelhor;
    while (!candidates.empty())
    {
        for(Vertice& itCand : candidates)
        {
            itCand.beneficio = 0;

            for (const auto& itGroup : solution->grupos)
            {
                beneficioMelhor = 0;

                for (const auto& itVert : itGroup.vertices)
                {

                    beneficioMelhor += graph.matrizDistancia[itCand.getIdVertice()][itVert.getIdVertice()];
                }

                if(beneficioMelhor > itCand.beneficio && canAdd(itCand.peso, graph.getUpperLimit(), itGroup.custo))
                {
                    itCand.beneficio = beneficioMelhor;
                    itCand.idBestGroupInsert = itGroup.getId();
                }

            }

        }

        sort(candidates.begin(), candidates.end(), sortCand);
        auto escopoInsercaoMax = static_cast<int>(floor(candidates.size() * alfa));
        int indiceInsercao = 0;
        if(escopoInsercaoMax>1){
            indiceInsercao = rand() % escopoInsercaoMax;
        }
        // std::uniform_int_distribution<> generator(0, escopoInsercaoMax);
        // //unsigned int indiceVert = generator(eng);
        // unsigned int indiceInsercao = generator(Utils::rand);

        // se n tiver beneficio para ninguem, coloca no grupo com menor peso 
        if(candidates[indiceInsercao].beneficio == 0)
        {
            unsigned int grupoInsercao = 0;
            float custo = solution->grupos[0].custo;
            for (unsigned int i = 1; i < solution->grupos.size(); ++i)
            {
                if(solution->grupos[i].custo < custo)
                {
                    custo = solution->grupos[i].custo;
                    grupoInsercao = i;
                }
            }
            candidates[indiceInsercao].idBestGroupInsert = grupoInsercao;
        }


        solution->grupos[candidates[indiceInsercao].idBestGroupInsert].vertices.push_back(candidates[indiceInsercao]);
        solution->grupos[candidates[indiceInsercao].idBestGroupInsert].custo+=graph.vertices[candidates[indiceInsercao].getIdVertice()].getPeso();//candidates[bestVerticeId].custo;
        for(auto& it : solution->grupos[candidates[indiceInsercao].idBestGroupInsert].vertices)
        {
            solution->grupos[candidates[indiceInsercao].idBestGroupInsert].beneficio+=graph.matrizDistancia[candidates[indiceInsercao].getIdVertice()][it.getIdVertice()];
        }
        //candidates.erase(remove(candidates.begin(), candidates.end(), bestVerticeId.getIdVertice()), candidates.end());
        candidates.erase(candidates.begin()+indiceInsercao);



    }

    if(!solution->verificaViabilidade(graph))
    {
        viabilizaSolucao(*solution, graph);
    }
    
    //cout << "RANDOMIZADO eh viavel? "<< (solution->verificaViabilidade(graph)?"true":"false") << " custo "  << solution->getValorSolucaoFinal() << " " << solution->getValorSolucaoFinal() << endl;

    return *solution;
}

int retornaAlfa (int QntAlfa, float vetProb[], int rand){
    int indiceAlfa = 0;
    // float acumulador = vetProb[0];
    float acumulador = 0;
    for(int i=0;i<QntAlfa;i++){
            acumulador += vetProb[i];
            if((acumulador*100)>rand){
                indiceAlfa = i;
                break;
            }
        }
    return indiceAlfa;
}
Solution Heuristics::greedyReativoAlgoritm(Grafo &graph, clock_t timeStart)
{
    // Setando resultado em decimal
    cout.setf(ios::fixed);
    Solution bestSolution = constructive(graph, clock());
    Solution guloso = greedyAlgorithm(graph, 0);
    // Tenta viabilizar, caso não seja possível, pega outra solução
    viabilizaSolucao(bestSolution, graph);
    if(!bestSolution.verificaViabilidade(graph))
    {
        bestSolution = greedyAlgorithm(graph, 0);
    }
    else
        
        if(bestSolution.calculaBeneficio(graph.matrizDistancia)< guloso.calculaBeneficio(graph.matrizDistancia)){
            bestSolution = guloso;
        }

    cout << (double)(clock() - timeStart)/CLOCKS_PER_SEC << "s" << endl;
    // Solution currentSolution;
    // Solution sol0,sol1,sol2,sol3,sol4,sol5,sol6,sol7,sol8,sol9;
    const int numAlfas = 10;
    int indiceAlfaAtual;
    float vetAlfas[numAlfas] = {0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50};
    float probAlfas[numAlfas];
    for(int i=0;i<numAlfas;i++){
        probAlfas[i] = 0.1;
    }
    int ordem = graph.getOrdem();
    const int timeExec= ordem/4; 
    const int nExcecoes= 300;
    const int nbloco= 50;
    float solP[numAlfas];
    for(int i=0;i<numAlfas;i++){
        solP[i] = 0;
    }
    int cont[numAlfas];
    for(int i=0;i<numAlfas;i++){
        cont[i] = 0;
    }
    float sumQ = 0;
    int indiceAlfa = 0;
    // int iteAlfas=0;
    float melhorBene = bestSolution.calculaBeneficio(graph.matrizDistancia);
    int ind = 0;
    for(int i=0; i<nExcecoes;i++){
        if(numAlfas<=i){
            ind = rand() % 100;
            indiceAlfa = retornaAlfa (numAlfas, probAlfas, ind);
        }
        else 
            indiceAlfa = i;
        Solution sol0 = greedyAlgorithm(graph, vetAlfas[indiceAlfa]);
        // Solution sol1 = VND_LocalArtigo(graph, sol0);
        // if(sol1.calculaBeneficio(graph.matrizDistancia)>sol0.calculaBeneficio(graph.matrizDistancia)){
        //     sol0 = sol1;
        // }
        float beneAtual = sol0.calculaBeneficio(graph.matrizDistancia);
        cont[indiceAlfa]++;
        solP[indiceAlfa] += beneAtual;
        if(i%nbloco==0 && i!=0){
            cout<< "ite: "<< i <<endl;
            for(int j=0;j<numAlfas;j++){
            cout<< "sum alfa: "<< j <<endl;
            cout<< "cont sol: "<< cont[j] <<endl;
            cout<< solP[j] <<endl;
            }
            cout <<"----------" <<endl;
            // atualiza probs
            float vetorA[numAlfas];
            for(int j=0;j<numAlfas;j++){
            vetorA[j] = 0;
            }
            for(int j=0;j<numAlfas;j++){
                if(cont[j]==0){
                    vetorA[j] = 0;
                }
                else
                    vetorA[j] = solP[j]/cont[j];
            }
            float vetorQ[numAlfas] = {0};
            for(int i=0;i<numAlfas;i++){
                vetorQ[i] = vetorA[i]/melhorBene;
            }
            // somatorio Qs
            sumQ=0;
            for(int i=0;i<numAlfas;i++){
                sumQ += vetorQ[i];
            }
            // atualiza probAlfas[] probAlfas[numAlfas]
            for(int i=0;i<numAlfas;i++){
                probAlfas[i] = (vetorQ[i]/sumQ);
            }
            for(int i=0;i<numAlfas;i++){
                cout<< "prob alfa i: ";
                cout << probAlfas[i]<< endl;
            }
            for(int i=0;i<numAlfas;i++){
                solP[i] = 0;
                cont[i] = 0;
            }
        }
        if(beneAtual>melhorBene){
            melhorBene = beneAtual;
            bestSolution = sol0;
        }
        // for(int i=0;i<numAlfas;i++){
        // cout<< probAlfas[i] << endl;
        // }
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

Solution Heuristics::VND_CCP(Grafo &graph, int maxIter)
{
    int nRefinamentos = 3;

    float melhora = 0;
    int v[3];
    int aux[3] = {0};
    Solution bestSolution = greedyReativoAlgoritm(graph,0);
    Solution solution = bestSolution;
    int contIter = 0;


    std::mt19937 eng(time(nullptr));

    int i=0;
    int varAux;
    do
    {

    	varAux = Utils::rand() % 3;//eng() % 3;
    	if(aux[varAux] == 0)
    	{
    		v[i] = varAux;
    		aux[varAux] = 1;
    		i++;
    	}
    }
    while(i<3);
    /*
    for(int it = 0; it < nRefinamentos; it++) {
        cout << "Pos " << it << ": " << "Busca local " << v[it] << endl;
    }
    */
    int l=0;
    int bl;
    while(l < nRefinamentos)
    {
        ++contIter;
        if(contIter == maxIter) {
            break;
        }

        bl = v[l];
        switch (bl)
        {
            case 0:
                buscaLocal_1to0(graph, solution, melhora);
                //buscaLocal_2to1(graph, solution, melhora);
                break;
            case 1:
                buscaLocal_1to1(graph, solution, melhora);
                break;
            case 2:
                //buscaLocal_1to0(graph, solution, melhora);
                buscaLocal_2to1(graph, solution, melhora);
                break;
        }
        //cout << melhora << endl;
        if(melhora > 0.0001)
        {
            bestSolution = solution;
            l=0;

        }
        else
        {
            //solution = bestSolution;
            l++;

        }

    }
    cout << "VND " << contIter << " iteracoes" << endl;
    return bestSolution;
}

Solution Heuristics::VND_Local(Grafo &graph, Solution &s, int maxIter)
{
    int nRefinamentos = 3;

    float melhora = 0;
    int v[3];
    int aux[3] = {0};
    Solution bestSolution = s;
    Solution solution = bestSolution;
    int contIter = 0;


    int i=0;
    int varAux;
    do
    {

    	varAux = Utils::rand() % 3;//eng() % 3;
    	if(aux[varAux] == 0)
    	{
    		v[i] = varAux;
    		aux[varAux] = 1;
    		i++;
    	}
    }
    while(i<3);
    /*
    for(int it = 0; it < nRefinamentos; it++) {
        cout << "Pos " << it << ": " << "Busca local " << v[it] << endl;
    }
    */
    int l=0;
    int bl;
    while(l < nRefinamentos)
    {
        ++contIter;
        if(contIter == maxIter) {
            break;
        }

        bl = v[l];
        switch (bl)
        {
            case 0:
                buscaLocal_1to0(graph, solution, melhora);
                //buscaLocal_2to1(graph, solution, melhora);
                break;
            case 1:
                buscaLocal_1to1(graph, solution, melhora);
                break;
            case 2:
                //buscaLocal_1to0(graph, solution, melhora);
                buscaLocal_2to1(graph, solution, melhora);
                break;
        }
        //cout << melhora << endl;
        if(melhora > 0.0001)
        {
            bestSolution = solution;
            l=0;

        }
        else
        {
            //solution = bestSolution;
            l++;

        }

    }
    cout << "VND " << contIter << " iteracoes" << endl;
    return bestSolution;
}

Solution Heuristics::VND_LocalArtigo(Grafo &graph, Solution &s)
{
    int nRefinamentos = 3;
    float melhora = 0;
    Solution bestSolution = s;
    Solution solution = bestSolution;
    int k=0;
    while(k < nRefinamentos)
    {
        switch (k)
        {
            case 0:
                k=1;
                buscaLocal_1to0(graph, solution, melhora);
                if(solution.calculaBeneficio(graph.matrizDistancia)>bestSolution.calculaBeneficio(graph.matrizDistancia)){
                    bestSolution=solution;
                }
            case 1:
                k=2;
                buscaLocal_1to1(graph, solution, melhora);
                if(solution.calculaBeneficio(graph.matrizDistancia)>bestSolution.calculaBeneficio(graph.matrizDistancia)){
                    bestSolution=solution;
                    k=0;
                }
            case 2:
                k=3;
                buscaLocal_2to1(graph, solution, melhora);
                if(solution.calculaBeneficio(graph.matrizDistancia)>bestSolution.calculaBeneficio(graph.matrizDistancia)){
                    bestSolution=solution;
                    k=0;
                }
        }
    }
    return bestSolution;
}

void Heuristics::buscaLocal_1to0(Grafo &g, Solution &s, float &melhora)
{
    // guarda a melhor troca
    int indG1 = -1;
    int indG2 = -1;
    int indVertice1;
    int indVertice2;
    float melhorBeneficio = 0;
    float novoPesoG1;
    float novoPesoG2;
    float adicaoBeneficioG1;
    float adicaoBeneficioG2;
    float reducaoBeneficioG1;
    float reducaoBeneficioG2;


    for(int g1 = 0; g1 < s.grupos.size() - 1; g1++) { // para cada grupo
        for(int v1 = 0; v1 < s.grupos[g1].vertices.size(); v1++) { // para cada vertice desse grupo
            for(int g2 = g1 + 1; g2 < s.grupos.size(); g2++) { // para cada outro grupo
                novoPesoG1 = s.grupos[g1].custo - s.grupos[g1].vertices[v1].getPeso();
                novoPesoG2 = s.grupos[g2].custo + s.grupos[g1].vertices[v1].getPeso();
                // verifica se eh viavel trocar
                if(novoPesoG1 >= s.grupos[g1].lowerLimit && novoPesoG1 <= s.grupos[g1].upperLimit && novoPesoG2 >= s.grupos[g2].lowerLimit && novoPesoG2 <= s.grupos[g2].upperLimit) {
                    adicaoBeneficioG2 = 0;
                    reducaoBeneficioG1 = 0;
                    for(Vertice& v : s.grupos[g1].vertices) {
                        if(v.getIdVertice() != s.grupos[g1].vertices[v1].getIdVertice()) {
                            reducaoBeneficioG1 -= g.matrizDistancia[v.getIdVertice()][s.grupos[g1].vertices[v1].getIdVertice()];
                        }
                    }
                    for(Vertice& v : s.grupos[g2].vertices) {
                        adicaoBeneficioG2 += g.matrizDistancia[v.getIdVertice()][s.grupos[g1].vertices[v1].getIdVertice()];
                    }
                    if(reducaoBeneficioG1+adicaoBeneficioG2 > melhorBeneficio) {
                        melhorBeneficio = reducaoBeneficioG1+adicaoBeneficioG2;
                        //cout << "[bl] Melhora: " << melhorBeneficio << endl;
                        indG1 = g1;
                        indG2 = g2;
                        indVertice1 = v1;
                    }
                }
            }
        }
    }
    // fazer a troca aqui
    if(indG1 != -1 && indG2 != -1) {
        float oldben = s.grupos[indG1].beneficio + s.grupos[indG2].beneficio;
        s.grupos[indG2].addVertice(g.matrizDistancia, s.grupos[indG1].vertices[indVertice1]);
        s.grupos[indG1].removeVertice(g.matrizDistancia, s.grupos[indG1].vertices[indVertice1]);
        float newben = s.grupos[indG1].beneficio + s.grupos[indG2].beneficio;
        melhora = newben - oldben;
        //cout << "Status final 1_0" << endl << "Melhoria: " << melhorBeneficio << " obtida com a doacao do vertice " << s.grupos[indG1].vertices[indVertice1].id;
        //cout << " do grupo " << indG1 << " para o " << indG2 << endl;
    }
    else
        melhora = 0;
}

void Heuristics::buscaLocal_1to1(Grafo &g, Solution &s, float &melhora)
{
    // guarda a melhor troca
    int indG1 = -1;
    int indG2 = -1;
    int indVertice1;
    int indVertice2;
    float melhorBeneficio = 0;
    float novoPesoG1;
    float novoPesoG2;
    float adicaoBeneficioG1;
    float adicaoBeneficioG2;
    float reducaoBeneficioG1;
    float reducaoBeneficioG2;
    float deltaG1;
    float deltaG2;

    for(int g1 = 0; g1 < s.grupos.size() - 1; g1++) { // para cada grupo
        for(int v1 = 0; v1 < s.grupos[g1].vertices.size(); v1++) { // para cada vertice desse grupo
            for(int g2 = g1 + 1; g2 < s.grupos.size(); g2++) { // para cada outro grupo
                for(int v2 = 0; v2 < s.grupos[g2].vertices.size(); v2++) { // para cada vertice do outro grupo // verifica todas as trocas
                    //cout << g1  << " " << v1 << " "  << " " << g2 << " " << v2 << endl;
                    novoPesoG1 = s.grupos[g1].custo - s.grupos[g1].vertices[v1].getPeso() + s.grupos[g2].vertices[v2].getPeso();
                    novoPesoG2 = s.grupos[g2].custo - s.grupos[g2].vertices[v2].getPeso() + s.grupos[g1].vertices[v1].getPeso();
                    // verifica se eh viavel trocar
                    if(novoPesoG1 >= s.grupos[g1].lowerLimit && novoPesoG1 <= s.grupos[g1].upperLimit && novoPesoG2 >= s.grupos[g2].lowerLimit && novoPesoG2 <= s.grupos[g2].upperLimit) {
                        adicaoBeneficioG1 = 0;
                        adicaoBeneficioG2 = 0;
                        reducaoBeneficioG1 = 0;
                        reducaoBeneficioG2 = 0;
                        for(Vertice& v : s.grupos[g1].vertices) {
                            if(v.getIdVertice() != s.grupos[g1].vertices[v1].getIdVertice()) {
                                reducaoBeneficioG1 -= g.matrizDistancia[v.getIdVertice()][s.grupos[g1].vertices[v1].getIdVertice()];
                                adicaoBeneficioG1 += g.matrizDistancia[v.getIdVertice()][s.grupos[g2].vertices[v2].getIdVertice()];
                            }
                        }
                        for(Vertice& v : s.grupos[g2].vertices) {
                            if(v.getIdVertice() != s.grupos[g2].vertices[v2].getIdVertice()) {
                                reducaoBeneficioG2 -= g.matrizDistancia[v.getIdVertice()][s.grupos[g2].vertices[v2].getIdVertice()];
                                adicaoBeneficioG2 += g.matrizDistancia[v.getIdVertice()][s.grupos[g1].vertices[v1].getIdVertice()];
                            }
                        }
                        deltaG1 = adicaoBeneficioG1 + reducaoBeneficioG1;
                        deltaG2 = adicaoBeneficioG2 + reducaoBeneficioG2;
                        if(deltaG1+deltaG2 > melhorBeneficio) {
                            melhorBeneficio = deltaG1+deltaG2;
                            //cout << "[bl] Melhora: " << melhorBeneficio << endl;
                            indG1 = g1;
                            indG2 = g2;
                            indVertice1 = v1;
                            indVertice2 = v2;
                        }
                    }
                }
            }
        }
    }
    // fazer a troca aqui
    if(indG1 != -1 && indG2 != -1) {
        float oldben = s.grupos[indG1].beneficio + s.grupos[indG2].beneficio;
        s.grupos[indG2].addVertice(g.matrizDistancia, s.grupos[indG1].vertices[indVertice1]);
        s.grupos[indG1].removeVertice(g.matrizDistancia, s.grupos[indG1].vertices[indVertice1]);
        s.grupos[indG1].addVertice(g.matrizDistancia, s.grupos[indG2].vertices[indVertice2]);
        s.grupos[indG2].removeVertice(g.matrizDistancia, s.grupos[indG2].vertices[indVertice2]);
        float newben = s.grupos[indG1].beneficio + s.grupos[indG2].beneficio;
        melhora = newben - oldben;
        //cout << "Status final 1_1 -- " << endl << "Melhoria: " << melhorBeneficio << " obtida com a troca " << s.grupos[indG1].vertices[indVertice1].id << " <-> " << s.grupos[indG2].vertices[indVertice2].id;
        //cout << " entre os grupos " << indG1 << " e " << indG2 << endl;
    }
    else
        melhora = 0;

}

void Heuristics::buscaLocal_2to1(Grafo &g, Solution &s, float &melhora)
{
    // guarda a melhor troca
    int indG1 = -1;
    int indG2 = -1;
    int indVertice1 = -1;
    int indVertice11 = -1;
    int indVertice2 = -1;
    float melhorBeneficio = 0;
    float novoPesoG1 = 0;
    float novoPesoG2 = 0;
    float adicaoBeneficioG1;
    float adicaoBeneficioG2;
    float reducaoBeneficioG1;
    float reducaoBeneficioG2;
    float deltaG1 = 0;
    float deltaG2 = 0;
    for(int g1 = 0; g1 < s.grupos.size()-1; g1++) { // para cada grupo
        for(int v1 = 0; v1 < s.grupos[g1].vertices.size()-1; v1++) { // para cada vertice desse grupo
            if(s.grupos[g1].vertices.size() == 0) {
                break;
            }
            for(int v11 = v1+1; v11 < s.grupos[g1].vertices.size(); v11++) { // para cada outro vertice
                for(int g2 = g1+1; g2 < s.grupos.size(); g2++) { // para cada outro grupo
                    for(int v2 = 0; v2 < s.grupos[g2].vertices.size(); v2++) { // para cada vertice do outro grupo // verifica todas as trocas
                        if(s.grupos[g2].vertices.size() == 0) {
                            break;
                        }
                        //cout << g1  << " " << v1 << " " << v11 << " " << g2 << " " << v2 << endl;
                        novoPesoG1 = s.grupos[g1].custo - s.grupos[g1].vertices[v1].getPeso() - s.grupos[g1].vertices[v11].getPeso() + s.grupos[g2].vertices[v2].getPeso();
                        novoPesoG2 = s.grupos[g2].custo - s.grupos[g2].vertices[v2].getPeso() + s.grupos[g1].vertices[v1].getPeso() + s.grupos[g1].vertices[v11].getPeso();

                        // verifica se eh viavel trocar
                        if(novoPesoG1 >= s.grupos[g1].lowerLimit && novoPesoG1 <= s.grupos[g1].upperLimit && novoPesoG2 >= s.grupos[g2].lowerLimit && novoPesoG2 <= s.grupos[g2].upperLimit) {
                            adicaoBeneficioG1 = 0;
                            adicaoBeneficioG2 = 0;
                            reducaoBeneficioG1 = 0;
                            reducaoBeneficioG2 = 0;

                            for(Vertice& v : s.grupos[g1].vertices) {
                                if(v.getIdVertice() != s.grupos[g1].vertices[v1].getIdVertice() && v.getIdVertice() != s.grupos[g1].vertices[v11].getIdVertice()) {
                                    reducaoBeneficioG1 -= g.matrizDistancia[v.getIdVertice()][s.grupos[g1].vertices[v1].getIdVertice()];
                                    reducaoBeneficioG1 -= g.matrizDistancia[v.getIdVertice()][s.grupos[g1].vertices[v11].getIdVertice()];
                                    adicaoBeneficioG1 += g.matrizDistancia[v.getIdVertice()][s.grupos[g2].vertices[v2].getIdVertice()];
                                }
                            }
                            // trata o caso entre os vertices q estao sendo removidos(?? o valor do melhorBeneficio com essa linha fica diferente do beneficio real dps da troca // Causa provavel: nao havia adicinado ao beneficio do g2)
                            reducaoBeneficioG1 -= g.matrizDistancia[s.grupos[g1].vertices[v1].getIdVertice()][s.grupos[g1].vertices[v11].getIdVertice()];
                            for(Vertice& v : s.grupos[g2].vertices) {
                                if(v.getIdVertice() != s.grupos[g2].vertices[v2].getIdVertice()) {
                                    reducaoBeneficioG2 -= g.matrizDistancia[v.getIdVertice()][s.grupos[g2].vertices[v2].getIdVertice()];
                                    adicaoBeneficioG2 += g.matrizDistancia[v.getIdVertice()][s.grupos[g1].vertices[v1].getIdVertice()];
                                    adicaoBeneficioG2 += g.matrizDistancia[v.getIdVertice()][s.grupos[g1].vertices[v11].getIdVertice()];
                                }
                            }
                            adicaoBeneficioG2 += g.matrizDistancia[s.grupos[g1].vertices[v1].getIdVertice()][s.grupos[g1].vertices[v11].getIdVertice()];

                            deltaG1 = adicaoBeneficioG1 + reducaoBeneficioG1;
                            deltaG2 = adicaoBeneficioG2 + reducaoBeneficioG2;

                            if(deltaG1+deltaG2 > melhorBeneficio) {
                                melhorBeneficio = deltaG1+deltaG2;
                                //cout << "[bl] Melhora: " << melhorBeneficio << endl;
                                indG1 = g1;
                                indG2 = g2;
                                indVertice1 = v1;
                                indVertice11 = v11;
                                indVertice2 = v2;

                            }

                        }
                    }
                }
            }
        }
    }


    // fazer a troca aqui
    if(indG1 != -1 && indG2 != -1) {
        int v1id = s.grupos[indG1].vertices[indVertice1].id;
        int v11id = s.grupos[indG1].vertices[indVertice11].id;
        int v2id = s.grupos[indG2].vertices[indVertice2].id;
        float oldben = s.grupos[indG1].beneficio + s.grupos[indG2].beneficio;
        s.grupos[indG2].addVertice(g.matrizDistancia, s.grupos[indG1].vertices[indVertice1]);
        s.grupos[indG2].addVertice(g.matrizDistancia, s.grupos[indG1].vertices[indVertice11]);
        s.grupos[indG1].addVertice(g.matrizDistancia, s.grupos[indG2].vertices[indVertice2]);
        s.grupos[indG1].removeVertice(g.matrizDistancia, s.grupos[indG1].vertices[indVertice1]);
        s.grupos[indG1].removeVertice(g.matrizDistancia, s.grupos[indG1].vertices[indVertice11]);
        s.grupos[indG2].removeVertice(g.matrizDistancia, s.grupos[indG2].vertices[indVertice2]);
        float newben = s.grupos[indG1].beneficio + s.grupos[indG2].beneficio;
        melhora = newben - oldben;
        //cout << "Status final 2_1 -- " << endl << "Melhoria(melhorBeneficio,diferencaReal): (" << melhorBeneficio << ',' << (newben-oldben) << ") obtida com a troca " << v1id << "," << v11id << " <-> " << v2id;
        //cout << " entre os grupos " << indG1 << " e " << indG2 << endl;
    }
    else
        melhora = 0;
}

