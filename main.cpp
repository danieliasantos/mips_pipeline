/*
    IFMG Sabara - Arquitetura de Computadores
    Resolucao de conflitos em Pipelines MIPS, utilizando bolha, adiantamento com bolha e reordenamento
    Alunos:
        Daniel Elias
        Jonathan Félix
*/
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <bitset>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <map>
#include <bits/stdc++.h>
#include <functional>
#include <utility>

using namespace std;

/*verifica se ha espaco duplicado*/
bool espacoDuplo(char lhs, char rhs) {
     return (lhs == rhs) && (lhs == ' ');
}

/*
    Funcoes para remocao de espacos duplicados na instrucao lida do arquivo
*/
void removeEspacoDuplo(string &str)
{
    std::string::iterator new_end = std::unique(str.begin(), str.end(), espacoDuplo);
    str.erase(new_end, str.end());
}

/*
    Funcao que verifica a estrutura da instrucao lida do arquivo, por meio de regencia. retorna true se encontrar ocorrencia da regencia na instrucao
*/
bool regexOk(string &str){
    regex express("[a-zA-Z]{1,5}[\\s]+(([\\$][a|f|g|k|r|v|s|t][a|t|p|\\d]\\,\\s+[\\d]*\\([\\$][(a|f|g|k|r|v|s|t)][a|t|p|[\\d]\\)|(\\$szero\\,\\s+[\\d]*\\([\\$][(a|f|g|k|r|v|s|t)][a|t|p|[\\d]\\)|(-?[\\d]*\\,\\s+[\\d]*\\([\\$][(a|f|g|k|r|v|s|t)][a|t|p|[\\d]\\))))|([\\$][a|f|g|k|r|v|s|t][a|t|p|\\d]\\,\\s+|(\\$szero\\,\\s|(-?[\\d]*\\,\\s+)))([\\$][a|f|g|k|r|v|s|t][a|t|p|\\d]\\,\\s+|(\\$szero\\,\\s+|(-?[\\d]*\\,\\s+)))([\\$][a|f|g|k|r|v|s|t][a|t|p|\\d]|(\\$szero|(-?[\\d]*))))");
    return regex_search(str, express);
}

/*
    funcao de leitura de arquivo de instrucoes
    parametros entrada:
        string nome: nome do arquivo que serah lido
        vector linhas: referencia do vector que recebera a lista de instrucoes lida do arquivo
        retorno: void
    funcoes:
        le linhas do arquivo, remove espacos duplicados, muda instrucoes para minusculo e testa regencia e carrega linhas num vector.
*/
void lerArquivo(string nome, vector<string> &linhas){
    ifstream arquivo;
    string linha;
    arquivo.open(nome);
    try{
        while(getline(arquivo, linha)){
            int chr = (int) linha.at(0); //pega o primeiro caractere para testar se eh de nova linha ou de retorno
            if (chr != 10 && chr != 13){ //ignorar se linha tiver soh caractere de retorno
                if (regexOk(linha)){ //verifica regex e insere resultado ou erro no vector
                    transform(linha.begin(), linha.end(), linha.begin(), ::tolower); //transforma string toda em minusculo
                    replace(linha.begin(), linha.end(), ',', ' '); //substituir virgula por espaco
                    removeEspacoDuplo(linha); //remover espacos duplicados
                    linhas.push_back(linha); //insere linha no vetor
                } else {
                    linhas.push_back("Erro. Instrucao nao reconhecida.");
                }
            }
        }
    } catch (exception& erro) {
        cout << "Erro no processamento do arquivo: " << erro.what() << endl;
    }
};

/*
    funcao que faz split em uma string, com base em espacos, e popula um vector de string
*/
void splitString(string &text, vector<string> &v){
    std::istringstream iss(text);
    std::vector<std::string> results(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
    v = results;
    results.clear();
    iss.clear();
}
/*
    - funcao de resolucao de conflito por bolha: pega a linha, verifica o tipo de instrucao, quanto a manipulacao de registrador,
    verifica as duas linhas seguintes e insere a bolha, se necessario
    - popula o vector listaBolha com as instrucoes e bolhas inseridas
*/
void bolha(vector<string> &listaArquivo, vector<string> &listaBolha, map<string, string> &mapInstr){
    vector<string> splitLinha0; //vector que recebera as partes da linha
    for(int i = 0; i < listaArquivo.size(); i++){ //percorrer todas as linhas do vector listaArquivo
        string linha0 = listaArquivo[i]; //pegar a primeira linha do vetor listaArquivo
        listaBolha.push_back(linha0); //insere primeira linha na lista de instrucoes com bolha
        splitString(linha0, splitLinha0); //faz o split na linha
        if(splitLinha0[0].compare("Erro.") != 0){ //verifica se a instrucao contem erro: == 0: erro, pois sao iguais. != 0: nao eh erro, pois nao sao iguais
            if(mapInstr.at(splitLinha0[0]).compare("RW") == 0){ // == 0: tipo da instrucao eh RW
                if(i + 1 < listaArquivo.size()){ //verifica se existe uma segunda linha
                    vector<string> splitLinha1; //vector que recebera o split da segunda linha
                    string linha1 = listaArquivo[i + 1]; //pega a segunda linha
                    splitString(linha1, splitLinha1); //executa o split na segunda linha
                    if(splitLinha1[0].compare("Erro.") != 0){ // == 0: erro, pois sao iguais. != 0: nao eh erro, pois nao sao iguais
                        if(mapInstr.at(splitLinha1[0]).compare("R") == 0){ //verifica se a proxima linha eh do tipo R
                            listaBolha.push_back("NOP"); //insere uma bolha
                            listaBolha.push_back("NOP"); //insere uma bolha
                            listaBolha.push_back(linha1);
                            ++i;
                            if(i + 1 < listaArquivo.size()){ //verifica se existe uma terceira linha
                                string linha2 = listaArquivo[i + 1]; //pega a terceira linha
                                vector<string> splitLinha2; //vector que recebera o split da terceira linha
                                splitString(linha2, splitLinha2); //executa o split na terceira linha
                                if(splitLinha2[0].compare("Erro.") != 0){ // == 0: erro, pois sao iguais. != 0: nao eh erro, pois nao sao iguais
                                    if(mapInstr.at(splitLinha2[0]).compare("R") == 0){ //verifica se a terceira linha eh do tipo R
                                        listaBolha.push_back("NOP"); //insere uma bolha
                                        listaBolha.push_back("NOP"); //insere uma bolha
                                    }
                                }
                                splitLinha2.clear(); //limpa o vetor de split da terceira linha
                            }
                        }
                    }
                    splitLinha1.clear(); //limpa o vetor de split da segunda linha
                }
            }
        }
        splitLinha0.clear(); //limpa o vetor de split da primeira linha
    }
}
/*
    - funcao de resolucao de conflito por aditamento com bolha: pega a linha, verifica se eh bolha. verifica a quarta seguinte e testa se pode adiantar.
    se for o caso, adianta e verifica a proxima.verifica as duas linhas seguintes e insere a bolha, se necessario
    - popula o vector listaAdiantBolha com as instrucoes e bolhas inseridas
*/
void adiantamentoBolha(vector<string> &listaBolha, vector<string> &listaAdiantBolha, map<string, string> &mapInstr){
    vector<string> splitLinha; //vector que recebera o split da linha 0
    vector<int> saltarLinhas; //vector que recebera as linhas que devem ser saltadas no vector listaBolha, pois jah foram movidas
    for (int i = 0; i < listaBolha.size(); i++){ //percorrer o vector da listaBolha verificando as bolhas
        bool pulaLinha = false; //verifica e a linha ja foi avaliada
        for (int k = 0; k < saltarLinhas.size(); k++){ //pesquisa no vector de linhas avaliadas, se a atual jah foi
            if(i == saltarLinhas[k]){ //se sim, pula
                pulaLinha = true; //pula
                break; //encerra a pesquisa
            }
        }
        if(!pulaLinha){ //pula a linha se ela jah foi avaliada
            listaAdiantBolha.push_back(listaBolha[i]); //insere a linha atual no vector de adiantamentoBolha
        }
        if(listaBolha[i + 1].compare("NOP") == 0){ //verifica se a proxima instrucao eh NOP
            for (int j = i + 4; j < listaBolha.size(); j++){ //loop para testar as proximas linhas e ver se alguma pode ser inserida no lugar do nop
                splitString(listaBolha[j], splitLinha); //split da proxima linha
                if(mapInstr.at(splitLinha[0]).compare("RW") == 0){ //teste da instrucao do tipo R
                    listaAdiantBolha.push_back(listaBolha[j]); // insere adiantamento da instrucao
                    saltarLinhas.push_back(j); //linha nao deve ser inserida novamente, entao deve ser saltada
                    i++; //proxima linha jah testada e inserida, saltar tambem
                    splitLinha.clear(); //limpa o vector de split da linha para testar a linha seguinte
                    splitString(listaBolha[j + 1], splitLinha); //split da linha seguinte
                    if(mapInstr.at(splitLinha[0]).compare("RW") == 0){ //testa a linha seguinte
                        listaAdiantBolha.push_back(listaBolha[j + 1]); //insere a linha seguinte
                        saltarLinhas.push_back(j + 1); //linha nao deve ser inserida novamente, entao deve ser saltada
                        i++; //incrementa a posicao de leitura da listaBolha
                    }
                    break;
                }
            }
        }
    }
    splitLinha.clear();
    saltarLinhas.clear();
}
/*
    - funcao de resolucao de conflito por reordenamento: pega a linha, verifica se eh RW. se sim, verifica a proxima. Se R, reordena com as RW seguintes.
    - popula o vector listaReordenamento com as instrucoes e bolhas inseridas
*/
void reordenamento(vector<string> &listaArquivo, vector<string> &listaReordenamento, map<string, string> &mapInstr){
    vector<string> splitLinha0; //vector que recebe  a linha atual para verificar se eh RW ou R
    vector<string> splitLinha1; //vector que recebe a proxima linha e verifica se eh R
    vector<string> splitLinha2; //vector que recebe a linha seguinte e verifica se eh R
    vector<int> saltarLinhas; //linhas jah tratadas que devem ser saltadas
    for (int i = 0; i < listaArquivo.size(); i++){ //percorrer todo o vector listaArquivo
        bool pulaLinha = false; //verifica e a linha ja foi avaliada
        for (int k = 0; k < saltarLinhas.size(); k++){ //pesquisa no vector de linhas avaliadas, se a atual jah foi
            if(i == saltarLinhas[k]){ //se sim, pula
                pulaLinha = true; //pula
                break; //encerra a pesquisa
            }
        }
        if(!pulaLinha){ //pula a linha se ela jah foi avaliada
            listaReordenamento.push_back(listaArquivo[i]); //insere a linha atual no vector de adiantamentoBolha
        }
        splitString(listaArquivo[i], splitLinha0); //split da linha atual para verificar se eh RW
        if (mapInstr.at(splitLinha0[0]).compare("RW") == 0){ //verifica se eh RW
            splitString(listaArquivo[i + 1], splitLinha1); //verificacao da proxima liha
            if(mapInstr.at(splitLinha1[0]).compare("R") == 0){ //verifica se eh R
                int cont = 0; //controle do while, soh deve reordenar duas linhas
                int j = i + 2; //comecar da terceira linha, a partir da atual
                while( j < listaArquivo.size() && cont < 2){ //busca por linhas RW
                    splitString(listaArquivo[j], splitLinha2); //teste das linhas seguintes
                    if(mapInstr.at(splitLinha2[0]).compare("RW") == 0){ //se for RW, reordena
                        listaReordenamento.push_back(listaArquivo[j]); //reordenamento
                        saltarLinhas.push_back(j); //linha reordenada deve ser saltada
                        cont++; //incremento do controle
                    }
                    j++; //incremento do controle
                    splitLinha2.clear(); //limpa memoria
                }
            }
        }
    }
    splitLinha0.clear();
    splitLinha1.clear();
    splitLinha2.clear();
}

void gravarArquivo(string nome, vector<string> &listaInstrucoes){
    ofstream arquivoSaida;
    arquivoSaida.open(nome); //, ios::app); parâmetro para abrir o arquivo e posicionar o cursor no final do arquivo.
    for (auto i : listaInstrucoes){
        arquivoSaida << i << "\n";
    }
    arquivoSaida.close();
}

int main(int argc, char *argv[]){
    map<string, string> mapInstr; // Mapeamento das instrucoes para pesquisa dos tipos R e RW quanto a manipulacao de registradores
    mapInstr.insert({"j", "R"});
    mapInstr.insert({"jal", "R"});
    mapInstr.insert({"jr", "R"});
    mapInstr.insert({"bltz", "R"});
    mapInstr.insert({"blez", "R"});
    mapInstr.insert({"bgtz", "R"});
    mapInstr.insert({"bgez", "R"});
    mapInstr.insert({"bltzal", "R"});
    mapInstr.insert({"bgezal", "R"});
    mapInstr.insert({"beq", "R"});
    mapInstr.insert({"bne", "R"});
    mapInstr.insert({"sb", "R"});
    mapInstr.insert({"sh", "R"});
    mapInstr.insert({"swl", "R"});
    mapInstr.insert({"sw", "R"});
    mapInstr.insert({"swr", "R"});
    mapInstr.insert({"addi", "RW"});
    mapInstr.insert({"addiu", "RW"});
    mapInstr.insert({"slti", "RW"});
    mapInstr.insert({"sltiu", "RW"});
    mapInstr.insert({"andi", "RW"});
    mapInstr.insert({"ori", "RW"});
    mapInstr.insert({"xori", "RW"});
    mapInstr.insert({"sllv", "RW"});
    mapInstr.insert({"srlv", "RW"});
    mapInstr.insert({"srav", "RW"});
    mapInstr.insert({"add", "RW"});
    mapInstr.insert({"addu", "RW"});
    mapInstr.insert({"sub", "RW"});
    mapInstr.insert({"subu", "RW"});
    mapInstr.insert({"and", "RW"});
    mapInstr.insert({"or", "RW"});
    mapInstr.insert({"xor", "RW"});
    mapInstr.insert({"nor", "RW"});
    mapInstr.insert({"slt", "RW"});
    mapInstr.insert({"sltu", "RW"});
    mapInstr.insert({"mthi", "RW"});
    mapInstr.insert({"mtlo", "RW"});
    mapInstr.insert({"mult", "RW"});
    mapInstr.insert({"multu", "RW"});
    mapInstr.insert({"div", "RW"});
    mapInstr.insert({"divu", "RW"});
    mapInstr.insert({"jalr", "RW"});
    mapInstr.insert({"mfhi", "RW"});
    mapInstr.insert({"mflo", "RW"});
    mapInstr.insert({"sll", "RW"});
    mapInstr.insert({"srl", "RW"});
    mapInstr.insert({"sra", "RW"});
    mapInstr.insert({"lui", "RW"});
    mapInstr.insert({"lb", "RW"});
    mapInstr.insert({"lh", "RW"});
    mapInstr.insert({"lwl", "RW"});
    mapInstr.insert({"lw", "RW"});
    mapInstr.insert({"lbu", "RW"});
    mapInstr.insert({"lhu", "RW"});
    mapInstr.insert({"lwr", "RW"});
    mapInstr.insert({"NOP", "0"});
    mapInstr.insert({"ERRO.", "0"});

    string nomeArquivo = "programa1.txt"; //arquivo txt de instrucoes
    cout << "Execucao iniciada. Programa MIPS lido: " << nomeArquivo << endl << "Processando... " << endl;
    vector<string> listaArquivo; //vector com linhas do arquivo
    lerArquivo(nomeArquivo, listaArquivo); //ler o arquivo e popular o vetor de strings com as linhas do arquivo
    vector<string> listaBolha; //vector com as linhas do arquivo separadas em structs do tipo Instrucao
    bolha(listaArquivo, listaBolha, mapInstr); //metodo de resolucao de conflitos usando a estrategia da bolha
    gravarArquivo("Resolucao_Bolha_" + nomeArquivo, listaBolha); //gravar resultado no arquivo de saida
    cout << "Resolucao de conflito com Bolha concluido. Saida gravada em arquivo." << endl;
    vector<string> listaAdiantBolha;
    adiantamentoBolha(listaBolha, listaAdiantBolha, mapInstr);
    gravarArquivo("Resolucao_Aditantamento_Bolha_" + nomeArquivo, listaAdiantBolha);
    cout << "Resolucao de conflito com Adiantamento-Bolha concluido. Saida gravada em arquivo." << endl;
    vector<string> listaReordenamento;
    reordenamento(listaArquivo, listaReordenamento, mapInstr);
    gravarArquivo("Resolucao_Reordenamento_" + nomeArquivo, listaReordenamento);
    cout << "Resolucao de conflito com Reordenamento concluido. Saida gravada em arquivo." << endl;
    listaArquivo.clear(); //limpa memoria
    listaBolha.clear(); //limpa memoria
    listaReordenamento.clear(); //limpa memoria
    mapInstr.clear(); //limpa memoria
    return 0;
}

/*
void bolha(vector<string> &listaArquivo, vector<string> &listaBolha, map<string, string> &mapInstr){
    vector<string> splitLinha0; //vector que recebera as partes da linha
    for(int i = 0; i < listaArquivo.size(); i++){ //percorrer todas as linhas do vector listaArquivo
        string linha0 = listaArquivo[i]; //pegar a primeira linha do vetor listaArquivo
        listaBolha.push_back(linha0); //insere primeira linha na lista de instrucoes com bolha
        splitString(linha0, splitLinha0); //faz o split na linha
        if(splitLinha0[0].compare("Erro.") != 0){ //verifica se a instrucao contem erro: == 0: erro, pois sao iguais. != 0: nao eh erro, pois nao sao iguais
            if(mapInstr.at(splitLinha0[0]).compare("RW") == 0){ // == 0: tipo da instrucao eh RW
                if(i + 1 < listaArquivo.size()){ //verifica se existe uma segunda linha
                    vector<string> splitLinha1; //vector que recebera o split da segunda linha
                    string linha1 = listaArquivo[i + 1]; //pega a segunda linha
                    splitString(linha1, splitLinha1); //executa o split na segunda linha
                    if(splitLinha1[0].compare("Erro.") != 0){ // == 0: erro, pois sao iguais. != 0: nao eh erro, pois nao sao iguais
                        if(mapInstr.at(splitLinha1[0]).compare("R") == 0){ //verifica se a proxima linha eh do tipo R
                            for(int k = 1; k < splitLinha1.size(); k++){ //testar todos os registradores da instrucao da proxima linha
                                size_t pos = splitLinha1[k].find("$"); //pegar posicao do cifrao, que indica o inicio de uma instrucao
                                string txt = splitLinha1[k].substr(pos, 3);
                                if (splitLinha0[1].compare(splitLinha1[k].substr(pos, 3)) == 0){ //registrador que serah escrito eh utilizado pela instrucao R
                                    listaBolha.push_back("NOP"); //insere uma bolha
                                    listaBolha.push_back("NOP"); //insere uma bolha
                                    listaBolha.push_back(linha1);
                                    ++i;
                                }
                            }
                            if(i + 1 < listaArquivo.size()){ //verifica se existe uma terceira linha
                                string linha2 = listaArquivo[i + 1]; //pega a terceira linha
                                vector<string> splitLinha2; //vector que recebera o split da terceira linha
                                splitString(linha2, splitLinha2); //executa o split na terceira linha
                                if(splitLinha2[0].compare("Erro.") != 0){ // == 0: erro, pois sao iguais. != 0: nao eh erro, pois nao sao iguais
                                    if(mapInstr.at(splitLinha2[0]).compare("R") == 0){ //verifica se a terceira linha eh do tipo R
                                        for(int j = 1; j < splitLinha1.size(); j++){ //testar todos os registradores da instrucao da terceira linha
                                            size_t pos2 = splitLinha1[j].find("$"); //pegar posicao do cifrao, que indica o inicio de uma instrucao
                                            if (splitLinha1[1].compare(splitLinha2[j].substr(pos2, 3)) == 0){ //registrador que serah escrito eh utilizado pela instrucao R
                                                listaBolha.push_back("NOP"); //insere uma bolha
                                                listaBolha.push_back("NOP"); //insere uma bolha
                                            }
                                        }
                                    }
                                }
                                splitLinha2.clear(); //limpa o vetor de split da terceira linha
                            }
                        }
                    }
                    splitLinha1.clear(); //limpa o vetor de split da segunda linha
                }
            }
        }
        splitLinha0.clear(); //limpa o vetor de split da primeira linha
    }
}

void adiantamentoBolha(vector<string> &listaBolha, vector<string> &listaAdiantBolha, map<string, string> &mapInstr){
    vector<string> splitLinha0; //vector que recebera o split da linha 0
    vector<string> splitLinha1; //vector que recebera o split das proximas linhas
    vector<int> saltarLinhas;
    bool saltar = false;
    int contAdiant = 0; //recebe o indice da linha que deve ser testada apos encontrar um nop
    for (int i = 0; i < listaBolha.size(); i++){ //percorrer o vector da listaBolha verificando as bolhas
        for(int x = 0; x < saltarLinhas.size(); x++){
            if (saltarLinhas[x] == i){
                saltar = true;
            }
        }
        if (!saltar){
            listaAdiantBolha.push_back(listaBolha[i]); //insere a linha atual no vector de adiantamentoBolha
            splitString(listaBolha[i], splitLinha0); //split da linha
            if(splitLinha0[0].compare("Erro.") != 0){ //verifica se a instrucao contem erro: == 0: erro, pois sao iguais. != 0: nao eh erro, pois nao sao iguais
                if(listaBolha[i + 1].compare("NOP") == 0){ //verifica se a proxima instrucao eh NOP
                    for (int j = i + contAdiant + 3; j < listaBolha.size(); j++){ //loop para testar as proximas linhas e ver se alguma pode ser inserida no lugar do nop
                        cout << " - j: " << j << endl;
                        splitString(listaBolha[j], splitLinha1); //split na proxima linha
                        if(mapInstr.at(splitLinha1[0]).compare("R") == 0){ //teste da instrucao do tipo R
                            int k = 1; //controle do while, vai da posicao 1 ateh size splitLinha1
                            bool usaReg = false; //recebe true se a instrucao utiliza o registrador da linha0
                            while((k < splitLinha1.size()) && !usaReg){ //testar todos os registradores da instrucao da terceira linha
                                size_t pos = splitLinha1[k].find("$"); //pegar posicao do cifrao, que indica o inicio de uma instrucao
                                if (splitLinha0[1].compare(splitLinha1[k].substr(pos, 3)) == 0){ //registrador que serah escrito eh utilizado pela instrucao R
                                    usaReg = true; //registrador eh utilizado
                                    break;
                                }
                                k++; // incremento do controle do while
                            }
                            if(!usaReg){ //nenhum registrador da instrucao eh utilizado
                                listaAdiantBolha.push_back(listaBolha[j]); // insere adiantamento da instrucao
                                saltarLinhas.push_back(j); //linha nao deve ser inserida novamente, entao deve ser saltada
                                i++; //proxima linha jah testada e inserida, saltar tambem
                                contAdiant++;
                            }
                        }
                    }
                }
            }
        }
    }
    splitLinha0.clear();
    splitLinha1.clear();
    saltarLinhas.clear();
}
*/
