/*
Considerando que agora são 10 ruas linhas e 10 ruas colunas

0 1 2 3 4 5 6 7 8 9

V I N I C I U S M A     LINHAS 
e e e e d e e d e e

I A D E H O L A N D     COLUNAS
c c b c b b c c c b
*/

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
using namespace std;

#define Lin 28
#define Col 37

#define cim 'c'
#define bai 'b'
#define esq 'e'
#define dir 'd'

#define faceL 'l'
#define faceC 'c'

#define green 'g'
#define yellow 'y'
#define red 'r'

#define desoc "-"
#define quarteirao "x"

typedef struct {
    bool existe;
    string nome; //nome do carro -> pos inicial
    int x,y; //pos atual
    char pref[4];//lista de preferencia
}Carro;

//verde -> amarelo -> vermelho -> verde
typedef struct {
    bool existe; //é semaforo
    char estado; //red, green, yellow
    char face; //se é de linha ou de coluna;
    int timeR, timeG, timeY; //tempos estabelecidos
    int tempo; //tempo no momento 
    int x, y; //posicao
}Semaforo;

typedef struct {
    int l, c; //posicao atual
    char direc; //para que lado a rua vai
    string estado; // espaço -> vazia; numero -> carro; x -> quadra;
    Semaforo semL; //semaforo da linha
    Semaforo semC; //semaforo da coluna
    Semaforo sem; //faciliatador momentaneo
    Carro car;
}Mapa;

void print_estado(Mapa m)
{
    if (m.estado == "100") cout << "00";
    else cout << m.estado;
    if (m.estado.length() == 1) cout << "  ";
    else cout << " ";
}

void print_direc(Mapa m)
{
    if (m.sem.existe) cout << m.sem.estado << " ";
    else cout << m.direc << " ";
}

void print_sems(Mapa m)
{
    if (m.semC.existe && m.semL.existe) 
        cout << m.semL.estado << m.semC.estado << " ";
    else cout << m.direc << "  ";
}

void printM(Mapa m[][Col], bool direc)
{
    for (int i = 0; i < Lin; i++) {
        for (int j = 0; j < Col; j++) {
            // if (direc) print_direc(m[i][j]);
            // else print_estado(m[i][j]);
            print_sems(m[i][j]);
        }
        printf("\n");
    }
}

void remove_carro(Mapa m[][Col], int x, int y)
{
    m[x][y].car.existe = false;
    m[x][y].car.nome = "";
    m[x][y].car.x = -1;
    m[x][y].car.y = -1;
    m[x][y].estado = desoc;
}

void inserir_carro(Mapa m[][Col], Carro c)
{
    m[c.x][c.y].car = c;
    m[c.x][c.y].estado = c.nome;
}

bool pode_avancar(Mapa m[][Col], char pref, int l, int c, int ind, bool semaforo)
{
    if (semaforo) {
        if (pref == cim || pref == bai)
        {
            if (m[ind][c].estado == desoc && m[ind][c].direc == pref) return true;
            else return false;
        }
        else if (pref == dir || pref == esq)
        {
            if (m[l][ind].direc == pref && m[l][ind].estado == desoc) return true;
            else return false;
        }
    }
    else {
        if (pref == cim || pref == bai) 
        {
            if (m[ind][c].estado == desoc) return true;
            else return false;
        }
        else if (pref == dir || pref == esq)
        {
            if (m[l][ind].estado == desoc) return true;
            else return false;
        }
    }
    return false;
}

int novo_ind(char d, int l, int c)
{
    int ind;

    if (d == cim)
        ind = (((l-1) % Lin) + Lin) % Lin;
    else if (d == dir)
        ind = (((c+1) % Col) + Col) % Col;
    else if (d == bai) 
        ind = (((l+1) % Lin) + Lin) % Lin;
    else if (d == esq)
        ind = (((c-1) % Col) + Col) % Col;
    
    return ind;
}

void avanca(Mapa m[][Col], char d, Carro *auxC)
{
    int l = auxC->x;
    int c = auxC->y;
    int ind;
    
    if (m[l][c].sem.existe)
    {
        if(m[l][c].sem.estado == red || m[l][c].sem.estado == yellow)
            return;
        
        else if (m[l][c].sem.estado == green) { //mudar de direção segundo a lista!
            for (int i = 0; i < 4 ; i++) {
                ind = novo_ind(auxC->pref[i], l, c);
                
                if (pode_avancar(m, auxC->pref[i], l, c, ind, true))
                {
                    if (auxC->pref[i] == cim || auxC->pref[i] == bai) 
                        auxC->x = ind;
                    else if (auxC->pref[i] == esq || auxC->pref[i] == dir)
                        auxC->y = ind;

                    remove_carro(m, l, c);
                    inserir_carro(m, *auxC);
                    return;
                }
            }
            return;
        }
    }
    else {
        ind = novo_ind(d, l, c);
        if (pode_avancar(m, d, l, c, ind, false))
        {
            if (d == cim || d == bai) 
                auxC->x = ind;
            else if (d == esq || d == dir)
                auxC->y = ind;

            remove_carro(m, l, c);
            inserir_carro(m, *auxC);
            return;
        }
    }
    return;
}

void func_sem(Mapa m[][Col], Semaforo *s) 
{
    s->tempo -= 1;
    if (s->tempo == 0)
    {
        if (s->estado == green) {
            s->estado = yellow;
            s->tempo = s->timeY;
        }
        else if (s->estado == yellow) {
            s->estado = red;
            s->tempo = s->timeR;
        }
        else if (s->estado == red) {
            s->estado = green;
            s->tempo = s->timeG;
        }
    }
    if (s->face == faceC) m[s->x][s->y].semC = *s;
    else m[s->x][s->y].semL = *s;
}

void rodar(Mapa m[][Col], vector<Semaforo> *semaforos, vector<Carro> *carros)
{
    int i;
    for (i = 0; i < semaforos->size(); i++)
        func_sem(m, &semaforos->at(i));

    // for (i = 0; i < carros->size(); i++) {
    //     Carro *c = &carros->at(i);
    //     avanca(m, m[c->x][c->y].direc, c);
    // }
}

void inserir_carro_first(Mapa m[][Col], Carro *c)
{
    int l = c->x, col = c->y;
 
    if (c->nome != "100") { //o do 100 já tá certo
        l *= 3;
        if (col != 1)
            col = ((col-1) * 4)+1;
    }
    
    for (int j = col; j < j+2; j++) //é decisivo na coluna
    { 
        if (m[l][j].estado == desoc) {
            c->x = l;
            c->y = j;
            m[l][j].estado = c->nome;
            m[l][j].car = *c;
            break;
        }
    }
}

void preferencias(Carro *aux)
{
    char lista_1[] = {cim, esq, bai, dir};
    char lista_2[] = {cim, dir, bai, esq};
    char lista_3[] = {bai, esq, cim, dir};
    char lista_4[] = {bai, dir, cim, esq};
    char lista_5[] = {esq, bai, dir, cim};
    char lista_6[] = {dir, bai, esq, cim};
    char lista_7[] = {esq, cim, dir, bai};
    char lista_8[] = {dir, bai, esq, cim};
    char lista_9[] = {cim, esq, bai, dir};
    char lista_0[] = {cim, dir, bai, esq};

    for (int i = 0; i < 4; i++) {
        switch (aux->y)
        {
            case 1:
                aux->pref[i] = lista_1[i]; break;
            case 2:
                aux->pref[i] = lista_2[i]; break;
            case 3:
                aux->pref[i] = lista_3[i]; break;
            case 4:
                aux->pref[i] = lista_4[i]; break;
            case 5:
                aux->pref[i] = lista_5[i]; break;
            case 6:
                aux->pref[i] = lista_6[i]; break;
            case 7:
                aux->pref[i] = lista_7[i]; break;
            case 8:
                aux->pref[i] = lista_8[i]; break;
            case 9:
                aux->pref[i] = lista_9[i]; break;
            case 0:
                aux->pref[i] = lista_0[i]; break;

            default:
                break;
        }
    }
}

void preencher_carros(Mapa m[][Col], vector<Carro> *carros) 
{
    int lista_carros[] ={1,2,3,4,5,6,7,8,9,10,
                        11,12,13,14,15,16,17,18,19,20,
                        21,22,23,24,25,26,27,28,29,30,
                        31,32,33,34,35,36,37,38,39,40,
                        41,42,43,44,45,46,47,48,49,50,
                        51,52,53,54,55,56,57,58,59,60,
                        61,62,63,64,65,66,67,68,69,70,
                        71,72,73,74,75,76,77,78,79,80,
                        81,82,83,84,85,86,87,88,89,90,
                        91,92,93,94,95,96,97,98,99,100};
    
    int len_l = sizeof(lista_carros) / sizeof(lista_carros[0]);
    
    Carro aux;
    aux.existe = true;
    for (int i = 0; i < len_l; i++) {
        // printf("Inserindo carro... %d\n", lista_carros[i]);
        aux.nome = to_string(lista_carros[i]);
        if (lista_carros[i] == 100) {
            aux.x = 15;
            aux.y = 15;
        }
        else {
            aux.x = lista_carros[i] / 10;
            if (aux.x == 0) aux.x = 5;
            aux.y = lista_carros[i] % 10;
            if (aux.y == 0) aux.y = 5;
        }
        preferencias(&aux); //preencher a lista de preferências
        
        inserir_carro_first(m, &aux); //inserir o carro na sua primeira posição no mapa
        carros->push_back(aux);
    }

}

void desenha(Mapa m[][Col], vector<Semaforo> *semaforos)
{
    char dirCol[] = {cim, cim, bai, cim, bai, bai, cim, cim, cim, bai};
    int l_dirCol = sizeof(dirCol) / sizeof(dirCol[0]);

    char dirLin[] = {esq, esq, esq, esq, dir, esq, esq, dir, esq, esq};
    int l_dirLin = sizeof(dirLin) / sizeof(dirLin[0]);

    int i, j, cont = 0; // ruas colunas
    for (j = 0; j < Col; j+=4) {
        for (i = 0; i < Lin; i++) {
            m[i][j].direc = dirCol[cont];
        }
        cont++;
        if (cont == l_dirCol) break;
    }

    cont = 0;
    for (i = 0; i < Lin; i += 3) { //ruas linhas
        for (j = 0; j < Col; j++) {
            if (m[i][j].direc == '0')
                m[i][j].direc = dirLin[cont];
        }
        cont++;
        if (cont == l_dirLin) break;
    }

    for (i = 1; i < Lin; i += 3) { //quadras
        for (j = 1; j < Col; j += 4) {
            m[i][j].estado = "x";
            m[i][j+1].estado = "x";
            m[i][j+2].estado = "x";

            m[i+1][j].estado = "x";
            m[i+1][j+1].estado = "x";
            m[i+1][j+2].estado = "x";
        }
    }

    //posicionando semáforos
    cont = 0;

    Semaforo s1, s2, s3; 
    s1.existe = s2.existe = s3.existe = true;
    s1.timeR = s2.timeR = s3.timeR = 3;
    s1.timeG = s2.timeG = s2.timeG = 3;
    s1.timeY = s2.timeY = s2.timeY = 1;
    s1.tempo = s2.tempo = s3.tempo = s1.timeG;
    s1.estado = green;
    s2.estado = red;
    s3.estado = yellow;

    Semaforo s_inic[3] = {s1, s2, s3};

    for (i = 0; i < Lin; i += 3) {
        for (j = 0; j < Col; j += 4) {
            if (i == 0 || i == Lin-1)
            {
                if (j != 0 && j != Col-1) {
                    s_inic[cont].face = faceL;
                    s_inic[cont].x = i;
                    s_inic[cont].y = j;
                    m[i][j].semL = s_inic[cont];
                    semaforos->push_back(s_inic[cont]);
                    
                    cont++;
                    if (cont > 2) cont = 0;
                    
                    s_inic[cont].face = faceC;
                    s_inic[cont].x = i;
                    s_inic[cont].y = j;
                    m[i][j].semC = s_inic[cont];
                    semaforos->push_back(s_inic[cont]);
                    
                    cont++;
                    if (cont > 2) cont = 0;
                }
            }
            else
            {
                s_inic[cont].face = faceL;
                s_inic[cont].x = i;
                s_inic[cont].y = j;
                m[i][j].semL = s_inic[cont];
                semaforos->push_back(s_inic[cont]);

                cont++;
                if (cont > 2) cont = 0;
                
                s_inic[cont].face = faceC;
                s_inic[cont].x = i;
                s_inic[cont].y = j;
                m[i][j].semC = s_inic[cont];
                semaforos->push_back(s_inic[cont]);

                cont++;
                if (cont > 2) cont = 0;
            }
        }
    }
}

void init(Mapa m[][Col]) 
{
    for (int i = 0; i < Lin; i++) {
        for (int j = 0; j < Col; j++) {
            m[i][j].car.existe = m[i][j].semL.existe = m[i][j].semC.existe = false ;
            m[i][j].direc = '0';
            m[i][j].estado = desoc;
            m[i][j].c = i;
            m[i][j].l = j;
        }
    }
}

void espere(int sec)
{
    this_thread::sleep_for(chrono::nanoseconds(10));
    this_thread::sleep_until(chrono::system_clock::now() + chrono::milliseconds(sec));
}

void clrscr()
{
    cout << "\033[2J\033[1;1H"; 
}

int main() 
{
    Mapa m[Lin][Col];
    vector<Semaforo> semaforos;
    
    init(m);
    desenha(m, &semaforos);

    vector<Carro> carros;
    
    preencher_carros(m, &carros);
    printM(m, true);

    // cout << "Semáforos: ";
    // for (int i = 0; i < semaforos.size(); i++) 
    //     cout << "" << semaforos[i].x << " " << semaforos[i].y << " Face: " << semaforos[i].face 
    //     << " Estado: " << semaforos[i].estado << endl;

    for (int i = 0; i < 10; i++) {
        rodar(m, &semaforos, &carros);
        printM(m, false);
        espere(1000);
        clrscr();
        cout << endl;
    }
    cout << endl;
    
    return 0;
}