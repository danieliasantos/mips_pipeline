/*
    IFMG Sabara - Arquitetura de Computadores
    Gerador de binarios MIPS
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

/*
    Funcao para remocao de espacos duplicados na instrucao lida do arquivo
*/
void removeEspacoDuplo(const string &input, string &output)
{
    output.clear();  // unless you want to add at the end of existing sring...
    unique_copy (input.begin(), input.end(), back_insert_iterator<string>(output), [](char a,char b){ return isspace(a) && isspace(b);});
}

/*
    Funcao que verifica a estrutura da instrucao lida do arquivo, por meio de regencia. retorna true se encontrar ocorrencia da regencia na instrucao
*/
bool regexOk(string &str){
    // regex original {"[a-z]{2,4}[\s](([\$][a|f|g|k|r|v|s|t][a|t|p|\d]\,\s[\d]*\([\$][(a|f|g|k|r|v|s|t)][a|t|p|[\d]\)|(\$szero\,\s[\d]*\([\$][(a|f|g|k|r|v|s|t)][a|t|p|[\d]\)|(-?[\d]*\,\s[\d]*\([\$][(a|f|g|k|r|v|s|t)][a|t|p|[\d]\))))|([\$][a|f|g|k|r|v|s|t][a|t|p|\d]\,\s|(\$szero\,\s|(-?[\d]*\,\s)))([\$][a|f|g|k|r|v|s|t][a|t|p|\d]\,\s|(\$szero\,\s|(-?[\d]*\,\s)))([\$][a|f|g|k|r|v|s|t][a|t|p|\d]|(\$szero|(-?[\d]*))))"}
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
            /* um metodo para verificar e corrigir a linha. caso nao de certo, retorna linha com o texto "erro". */
            //verifica regex e insere resultado ou erro no vector
            if (regexOk(linha)){
                //transforma string toda em minusculo
                transform(linha.begin(), linha.end(), linha.begin(), ::tolower);
                //substituir virgula por espaco
                replace(linha.begin(), linha.end(), ',', ' ');
                //remover espacos duplicados
                //string::iterator new_end = unique(linha.begin(), linha.end(), BothAreSpaces);
                //linha.erase(new_end, linha.end());
                removeEspacoDuplo(linha, linha);
                //insere linha no vetor
                linhas.push_back(linha);
            } else {
                linhas.push_back("Erro. Instrucao nao reconhecida.");
            }
            //regexOk(linha) ? (linhas.push_back(linha)) : (linhas.push_back("Erro. Instrucao nao reconhecida."));
        }
    } catch (exception& erro) {
        cout << "Erro no processamento do arquivo: " << erro.what() << endl;
    }
};

/*
    Structs que guardam o tipo das instrucoes quanto as operacoes com registradores:
        - R: instrucoes que leem regisradores
        - RW: instrucoes que leem e gravam em registradores
    Metodo ini:
        - recebe o tipo de instrucao
    Metodo tipo:
        - retorna o tipo da instrucao
*/
struct Instrucao{

    string atributo; //permite a indexacao da struct no map

    void ini(string a){
        atributo = a;
    }

    string tipo(){
        return atributo;
    }

    bool operator<(const Instrucao &other) const { //indexacao da struct no map
        return (atributo < other.atributo);
    }
};

/*
    funcao que faz split em uma string, com base em espacos, e popula um vector de string
*/

void splitString(string &text, vector<string> &v){
    std::istringstream iss(text);
    std::vector<std::string> results(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
    int x = results.size();
    cout << "Tamanho: " << x << "Primeira posicao: " << results[0] << endl;
}
/*
    - funcao de resolucao de conflito por bolha: pega a linha, verifica o tipo de instrucao, quanto a manipulacao de registrador,
    verifica as duas linhas seguintes e insere a bolha, se necessario
    - popula o vector listaBolha com as instrucoes e bolhas inseridas
*/
void bolha(vector<string> &listaArquivo, vector<string> &listaBolha, map<string, Instrucao> &mapInstr){
    //percorrer todas as linhas do vector listaArquivo
    for(int i = 0; i < listaArquivo.size(); i++){
        //pegar a primeira linha do vetor listaArquivo
        string linha0 = listaArquivo[i];
        //insere primeira linha na lista de instrucoes com bolha
        listaBolha.push_back(linha0); //insere primeira instrucao
        //vector que recebera as partes da linha
        vector<string> splitLinha0;
        //faz o split na linha
        splitString(linha0, splitLinha0);
        //verifica se a instrucao contem erro
/*
        if(splitLinha0[0].compare("Erro.") != 0){ // == 0: erro, pois sao iguais. != 0: nao eh erro, pois nao sao iguais
            string t0 = mapInstr.at(splitLinha0[0]).tipo(); //tipo da instrucao
            if(t0.compare("RW") == 0){ // == 0: tipo da instrucao eh RW
                if(i + 1 < listaArquivo.size()){ //verifica se existe uma proxima linha
                    //primeira linha apos
                    string linha1 = listaArquivo[i + 1]; //pega a proxima linha
                    vector<string> splitLinha1; //vector que recebera o split da proxima linha
                    splitString(linha1, splitLinha1); //executa o split na proxima linha
                    if(splitLinha1[0].compare("Erro.") != 0){ // == 0: erro, pois sao iguais. != 0: nao eh erro, pois nao sao iguais
                        string t1 = mapInstr.at(splitLinha1[0]).tipo();
                        if(t1.compare("R") == 0){ //verifica se a proxima linha eh do tipo R
                            //testar todos os registradores da instrucao da proxima linha
                            for(int k = 1; k < splitLinha1.size(); k++){
                                size_t pos = splitLinha1[k].find("$"); //pegar posicao do cifrao, que indica o inicio de uma instrucao
                                string reg = splitLinha1[k].substr(pos, 3); //identifica o registrador
                                if (splitLinha0[1].compare(reg) == 0){ //registrador que serah escrito eh utilizado pela instrucao R
                                    //inserir bolhas
                                    listaBolha.push_back("NOP"); //insere uma bolha
                                    listaBolha.push_back("NOP"); //insere uma bolha
                                }
                            }
                        }
                    }
                    listaBolha.push_back(linha1); //insere proxima instrucao
                    splitLinha1.clear(); //limpa o vetor de split da linha 1
                    i++;
                }
            }
        }
        */
        splitLinha0.clear();
    }
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

    /*
        Instanciacao das structs das instrucoes com o respectivo tipo
    */
    Instrucao LUI, BLTZ, BLEZ, BGTZ, BGEZ, BLTZAL, BGEZAL, BEQ, BNE, ADDI, ADDIU, SLTI, SLTIU, ANDI, ORI, XORI, LB, LH, LWL, LW, LBU, LHU, LWR, SB, SH, SWL, SW, SWR, J, JAL, SLLV, SRLV, SRAV, ADD, ADDU, SUB, SUBU, AND, OR, XOR, NOR, SLT, SLTU, JR, MTHI, MTLO, MULT, MULTU, DIV, DIVU, JALR, MFHI, MFLO, SLL, SRL, SRA;

    J.ini("R");
    JAL.ini("R");
    JR.ini("R");
    BLTZ.ini("R");
    BLEZ.ini("R");
    BGTZ.ini("R");
    BGEZ.ini("R");
    BLTZAL.ini("R");
    BGEZAL.ini("R");
    BEQ.ini("R");
    BNE.ini("R");
    SB.ini("R");
    SH.ini("R");
    SWL.ini("R");
    SW.ini("R");
    SWR.ini("R");
    ADDI.ini("RW");
    ADDIU.ini("RW");
    SLTI.ini("RW");
    SLTIU.ini("RW");
    ANDI.ini("RW");
    ORI.ini("RW");
    XORI.ini("RW");
    SLLV.ini("RW");
    SRLV.ini("RW");
    SRAV.ini("RW");
    ADD.ini("RW");
    ADDU.ini("RW");
    SUB.ini("RW");
    SUBU.ini("RW");
    AND.ini("RW");
    OR.ini("RW");
    XOR.ini("RW");
    NOR.ini("RW");
    SLT.ini("RW");
    SLTU.ini("RW");
    MTHI.ini("RW");
    MTLO.ini("RW");
    MULT.ini("RW");
    MULTU.ini("RW");
    DIV.ini("RW");
    DIVU.ini("RW");
    JALR.ini("RW");
    MFHI.ini("RW");
    MFLO.ini("RW");
    SLL.ini("RW");
    SRL.ini("RW");
    SRA.ini("RW");
    LUI.ini("RW");
    LB.ini("RW");
    LH.ini("RW");
    LWL.ini("RW");
    LW.ini("RW");
    LBU.ini("RW");
    LHU.ini("RW");
    LWR.ini("RW");

    /*
        Mapeamento das instrucoes para pesquisa
    */
    map<string, Instrucao> mapInstr;
    mapInstr.insert({"lui", LUI});
    mapInstr.insert({"bltz", BLTZ});
    mapInstr.insert({"blez", BLEZ});
    mapInstr.insert({"bgtz", BGTZ});
    mapInstr.insert({"bgez", BGEZ});
    mapInstr.insert({"bltzal", BLTZAL});
    mapInstr.insert({"bgezal", BGEZAL});
    mapInstr.insert({"beq", BEQ});
    mapInstr.insert({"bne", BNE});
    mapInstr.insert({"addi", ADDI});
    mapInstr.insert({"addiu", ADDIU});
    mapInstr.insert({"slti", SLTI});
    mapInstr.insert({"sltiu", SLTIU});
    mapInstr.insert({"andi", ANDI});
    mapInstr.insert({"ori", ORI});
    mapInstr.insert({"xori", XORI});
    mapInstr.insert({"lb", LB});
    mapInstr.insert({"lh", LH});
    mapInstr.insert({"lwl", LWL});
    mapInstr.insert({"lw", LW});
    mapInstr.insert({"lbu", LBU});
    mapInstr.insert({"lhu", LHU});
    mapInstr.insert({"lwr", LWR});
    mapInstr.insert({"sb", SB});
    mapInstr.insert({"sh", SH});
    mapInstr.insert({"swl", SWL});
    mapInstr.insert({"sw", SW});
    mapInstr.insert({"swr", SWR});
    mapInstr.insert({"j", J});
    mapInstr.insert({"jal", JAL});
    mapInstr.insert({"sllv", SLLV});
    mapInstr.insert({"srlv", SRLV});
    mapInstr.insert({"srav", SRAV});
    mapInstr.insert({"add", ADD});
    mapInstr.insert({"addu", ADDU});
    mapInstr.insert({"sub", SUB});
    mapInstr.insert({"subu", SUBU});
    mapInstr.insert({"and", AND});
    mapInstr.insert({"or", OR});
    mapInstr.insert({"xor", XOR});
    mapInstr.insert({"nor", NOR});
    mapInstr.insert({"slt", SLT});
    mapInstr.insert({"sltu", SLTU});
    mapInstr.insert({"jr", JR});
    mapInstr.insert({"mthi", MTHI});
    mapInstr.insert({"mtlo", MTLO});
    mapInstr.insert({"mult", MULT});
    mapInstr.insert({"multu", MULTU});
    mapInstr.insert({"div", DIV});
    mapInstr.insert({"divu", DIVU});
    mapInstr.insert({"jalr", JALR});
    mapInstr.insert({"mfhi", MFHI});
    mapInstr.insert({"mflo", MFLO});
    mapInstr.insert({"sll", SLL});
    mapInstr.insert({"srl", SRL});
    mapInstr.insert({"sra", SRA});

    //arquivo txt de instrucoes
    string nomeArquivo = "programa1.txt";

    //vector com linhas do arquivo
    vector<string> listaArquivo;

    //ler o arquivo e popular o vetor de strings com as linhas do arquivo
    lerArquivo(nomeArquivo, listaArquivo);

    //vector com as linhas do arquivo separadas em structs do tipo Instrucao
    vector<string> listaBolha;

    //metodo de resolucao de conflitos usando a estrategia da bolha
    //bolha(listaArquivo, listaBolha, mapInstr);

    //nomeArquivo = "Resolucao_Bolha_" + nomeArquivo;
    //gravarArquivo(nomeArquivo, listaBolha);

    string texto = "Texto para split.";

    splitString(texto, listaBolha);

    cout << "Concluido! Os binarios foram gravados no arquivo " << endl;
    //limpa memoria
    listaArquivo.clear();
    listaBolha.clear();
    mapInstr.clear();
    return 0;
}
