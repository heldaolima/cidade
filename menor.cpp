/*
it may be required a cars' positions list
it may be required an anti-loop function
*/


#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

#define Lin 4
#define Col 37

#define cim 'c'
#define bai 'b'
#define esq 'e'
#define dir 'd'

#define green 'g'
#define yellow 'y'
#define red 'r'


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
    int timeR, timeG, timeY; //tempos estabelecidos
    int tempo; //tempo no momento 
    int x, y; //posicao
}Semaforo;

typedef struct {
    int l, c; //posicao atual
    char direc; //para que lado a rua vai
    string estado; // espaço -> vazia; numero -> carro; x -> quadra;
    Semaforo sem;
    Carro car;
}Mapa;

void printM(Mapa m[][Col])
{
    for (int i = 0; i < Lin; i++) {
        for (int j = 0; j < Col; j++) {
            if (m[i][j].sem.existe) cout << m[i][j].sem.estado << " ";
            else cout << "* ";
            // else
                // printf("%c ", m[i][j].direc);
            // if (m[i][j].car.existe) cout << m[i][j].estado << " ";
            // else cout << "* ";
        }
        printf("\n");
    }
}

void remove_carro(Mapa m[][Col], int x, int y)
{
    m[x][y].car.existe = false;
    m[x][y].car.x = -1;
    m[x][y].car.y = -1;
    // if (m[x][y].sem.existe) m[x][y].estado = "S";
    m[x][y].estado = " ";
}

void inserir_carro(Mapa m[][Col], Carro c)
{
    m[c.x][c.y].car = c;
    m[c.x][c.y].estado = c.nome;
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

bool pode_avancar(Mapa m[][Col], char pref, int l, int c, int ind)
{
    if (pref == cim || pref == bai)
    {
        if (m[ind][c].estado == " " && m[ind][c].direc != '0') return true;
        else return false;
    }
    else // pref == dir || pref == esq
    {
        if (m[l][ind].estado == " " && m[l][ind].direc != '0') return true;
        else return false;
    }
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
        
        else { //semaforo verde -> mudar de direção segundo a lista!
            for (int i = 0; i < 4 ; i++) {
                ind = novo_ind(auxC->pref[i], l, c);
                if (pode_avancar(m, auxC->pref[i], l, c, ind))
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
        }
    }
    else {
        ind = novo_ind(d, l, c);
        if (d == cim || d == bai) 
            auxC->x = ind;
        else if (d == esq || d == dir)
            auxC->y = ind;

        remove_carro(m, l, c);
        inserir_carro(m, *auxC);
    }
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
    m[s->x][s->y].sem = *s;
}

void rodar(Mapa m[][Col], vector<Semaforo> *semaforos, vector<Carro> *carros)
{
    int i;
    for (i = 0; i < semaforos->size(); i++) {
        Semaforo *s = &semaforos->at(i);
        func_sem(m, s);
    }

    for (i = 0; i < carros->size(); i++) {
        Carro *c = &carros->at(i);
        avanca(m, m[c->x][c->y].direc, c);
    }
}

void roda(Mapa m[][Col])
{
    int i, j;
    for (i = 0; i < Lin; i++) {
        for (j = 0; j < Col; j++) {
            if (m[i][j].car.existe)
            {
                // func_sem(&m[6][0].sem);
                // avanca(m, m[i][j].direc, i, j);
                return;
            }
        }
    }
}

void desenha(Mapa m[][Col], vector<Semaforo> *semaforos)
{
    char dirCol[] = {cim, cim, cim, cim, cim, cim, cim, bai};
    int l_dirCol = sizeof(dirCol) / sizeof(dirCol[0]);

    char dirLin[] = {esq, esq, esq, esq, dir, esq, esq, dir};
    int l_dirLin = sizeof(dirLin) / sizeof(dirLin[0]);

    int i, j, cont = 0; //isso aqui já funciona pra colocar as direções das colunas
    for (j = 4; j < Col; j+=4) {
        for (i = 0; i < Lin; i++) {
            m[i][j].direc = dirCol[cont];
        }
        cont++;
        if (cont == l_dirCol) break;
    }
    
    for (j = 0; j < Col; j++) {
        if (m[3][j].direc == '0') m[3][j].direc = esq;
    }
    //posicionando semáforos; existe um esquema para a primeira e a última linhas 
    //outro para os resto
    
    Semaforo s; //bo testar
    s.existe = true;
    s.estado = green;
    s.timeR = 4;
    s.timeG = 3;
    s.timeY = 1;
    s.tempo = s.timeR;
    
    for (i = 0; i < Lin; i += 3) {
        for (j = 0; j < Col;j += 4) {
            // if (i == 0 || i == Lin-1) //primeira e ultimas, quando mapa completo
            s.x = i;
            s.y = j;
            m[i][j].sem = s;
            semaforos->push_back(s);
        }
    }

}

void init(Mapa m[][Col]) {
    for (int i = 0; i < Lin; i++) {
        for (int j = 0; j < Col; j++) {
            m[i][j].car.existe = m[i][j].sem.existe = false ;
            m[i][j].direc = '0';
            m[i][j].estado = " ";
            m[i][j].c = i;
            m[i][j].l = j;
        }
    }
}

int main() 
{
    Mapa teste[Lin][Col];
    vector<Semaforo> semaforos;
    init(teste);
    desenha(teste, &semaforos);

    vector<Carro> carros;
    
    Carro civic, monza;
    civic.existe = true;
    civic.nome = "30";
    
    char prefe[] = {bai, esq, dir, cim};
    for (int i = 0; i < 4; i++)
        civic.pref[i] = prefe[i];

    civic.x = stoi(civic.nome) / 10;
    civic.y = stoi(civic.nome) % 10;

    monza.existe = true;
    monza.nome = "39";
    
    for (int i = 0; i < 4; i++)
        monza.pref[i] = prefe[i];

    monza.x = stoi(monza.nome) / 10;
    monza.y = stoi(monza.nome) % 10;


    carros.push_back(civic);
    carros.push_back(monza);
    inserir_carro(teste, civic);
    inserir_carro(teste, monza);

    // printM(teste);
    printf("\n");
    for (int i = 0; i < 5; i++) {
        rodar(teste, &semaforos, &carros);
        printM(teste);
        cout << endl;
    }
    cout << endl;

    return 0;
}