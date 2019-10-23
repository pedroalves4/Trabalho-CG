#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
//#include <GLFW/glfw3.h>

#include "extras.h"

/// Estruturas iniciais para armazenar vertices
//  Você poderá utilizá-las adicionando novos métodos (de acesso por exemplo) ou usar suas próprias estruturas.
class vertice
{
public:
    float x,y,z;
};

class triangle
{
public:
    vertice v[3];
};


class vetor
{
public:
    vertice v1;
};

class barrinhas
{
public:
    bool mostra;

    bool getExibe()
    {
        return mostra;
    }
};

typedef struct
{
    float x, y;
} vertex;


/// Globals
float zdist = 3.0;
float rotationX = 0.0, rotationY = 0.0;
float rotacaoX = 0.0, rotacaoY = 0.0;
int   last_x, last_y;
int   width, height;
bool pausado = false;
bool projecao_ortogonal = true;
bool rotacaoLiberada = false;
bool podeMoverABolinha = false;
bool primeiroLancamento = false;
int janela = 0;
float xBarra = 0.0;
float xBolinha = 0.0;
float yBolinha = -0.56;
float xSeta = 0.40;
float ySeta = 0.0;
float PI = 3.1415927;
float raioTorus = 0.05;
bool desenhaSetaControle = true;
bool pintaPlataformaVermelho = false;
vetor* vetorSeta = new vetor();
barrinhas* vetorBloquinhos = new barrinhas[14];
vetor vetorMovimentoBolinha;

// círculo terá 36 vértices
vertex v[36];

void criaCirculo()
{
    float raio = 0.6;
    int g, i;

    for(i = 0; i < 18; i++)
    {
        g = i * 10; // anda no círculo de 10 em 10 graus (para cobrir os 360 graus)
        v[i].x = raio * cos(g * PI / 180) + raioTorus;
        v[i].y = raio * sin(g * PI / 180);
    }
}


/// Functions
void init(void)
{
    initLight(width, height); // Função extra para tratar iluminação.
}

void CalculaNormal(triangle t, vertice *vn)
{
    vertice v_0 = t.v[0],
            v_1 = t.v[1],
            v_2 = t.v[2];
    vertice v1, v2;
    double len;

    /* Encontra vetor v1 */
    v1.x = v_1.x - v_0.x;
    v1.y = v_1.y - v_0.y;
    v1.z = v_1.z - v_0.z;

    /* Encontra vetor v2 */
    v2.x = v_2.x - v_0.x;
    v2.y = v_2.y - v_0.y;
    v2.z = v_2.z - v_0.z;

    /* Calculo do produto vetorial de v1 e v2 */
    vn->x = (v1.y * v2.z) - (v1.z * v2.y);
    vn->y = (v1.z * v2.x) - (v1.x * v2.z);
    vn->z = (v1.x * v2.y) - (v1.y * v2.x);

    /* normalizacao de n */
    len = sqrt(pow(vn->x,2) + pow(vn->y,2) + pow(vn->z,2));

    vn->x /= len;
    vn->y /= len;
    vn->z /= len;
}

void atualizaVetorSeta()
{
    ySeta = sqrt(pow(0.40, 2) - pow(vetorSeta->v1.x, 2)); ///pitágoras: mantém o módulo do vetor constante = 0.40
    if(ySeta >= 0)     ///a bolinha não deve começar indo pra baixo
    {
        vetorSeta->v1.y = ySeta;
    }
}

void moveBolinha()
{
    if(!pausado)
    {

            xBolinha += vetorMovimentoBolinha.v1.x;
            yBolinha += vetorMovimentoBolinha.v1.y;

    }
}

float calculaModuloVetor(vetor v)
{
    float somaQuadrados = pow(v.v1.x, 2.0) + pow(v.v1.y, 2.0);
    float modulo = sqrt(somaQuadrados);
    return modulo;
}

///Proj U em V = (u.v/|v|Â²)*V
vetor calculaProjecao(vetor u, vetor v)
{
    vetor projecao;
    float prodEscalar = u.v1.x * v.v1.x + u.v1.y * v.v1.y;
    float somaQuadradoDasComponentes = pow(v.v1.x, 2) + pow(v.v1.y, 2);
    projecao.v1.x = (prodEscalar/somaQuadradoDasComponentes)*v.v1.x;
    projecao.v1.y = (prodEscalar/somaQuadradoDasComponentes)*v.v1.y;
    return projecao;
}

float calculaProdutoEscalar(vertice v1, vertice v2) {
    return v1.x*v2.x + v1.y*v2.y;
}

void refleteBolinha(vertice verticeNormal)
{
    vetor vetorNormal;
    vetorNormal.v1 = verticeNormal;

    float produtoEscalar = calculaProdutoEscalar(verticeNormal, vetorMovimentoBolinha.v1);

    vetor refletido;
    refletido.v1.x = vetorMovimentoBolinha.v1.x - 2*produtoEscalar*verticeNormal.x;
    refletido.v1.y = vetorMovimentoBolinha.v1.y - 2*produtoEscalar*verticeNormal.y;

    vetorMovimentoBolinha = refletido;
}

void reflexaoBarra()
{
    vertice vetorNormal;
    vertice verticesBarraFaceSuperior[3] =  {{0.25 + xBarra, -0.625, 0.0625},
                                            {0.25 + xBarra, -0.625, 0.125},
                                            {-0.25 + xBarra, -0.625, 0.125}};

    triangle t = {verticesBarraFaceSuperior[0], verticesBarraFaceSuperior[1], verticesBarraFaceSuperior[2]};
    CalculaNormal(t, &vetorNormal);

    if(yBolinha < -0.60 && fabs(xBarra - xBolinha) < 0.4)
        refleteBolinha(vetorNormal);
}
void reflexaoBloquinhos()
{
    if(xBolinha > -0.97 && xBolinha < -0.63)
    {
        if(vetorBloquinhos[10].mostra)
        {
            if((yBolinha < 0.227 && yBolinha > 0.223)  ||
                    (yBolinha > 0.352 && yBolinha < 0.358))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[10].mostra = false;
            }
            if((yBolinha > 0.227 && yBolinha < 0.358) && (xBolinha < -0.67 && xBolinha > -0.66))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[10].mostra = false;
            }
        }

        if(vetorBloquinhos[5].mostra)
        {
            if((yBolinha > 0.447 && yBolinha < 0.452) || (yBolinha > 0.577 && yBolinha < 0.583))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[5].mostra = false;
            }
            if((yBolinha > 0.447 && yBolinha < 0.583) && (xBolinha < -0.6 && xBolinha > -0.7))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[5].mostra = false;
            }
        }

        if(vetorBloquinhos[0].mostra)
        {
            if((yBolinha > 0.672 && yBolinha < 0.678) || (yBolinha > 0.802 && yBolinha < 0.808))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[0].mostra = false;
            }
            if((yBolinha > 0.672 && yBolinha < 0.808) && (xBolinha < -0.6 && xBolinha > -0.7))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[0].mostra = false;
            }
        }
    }


    if(xBolinha > -0.57 && xBolinha < -0.23 )
    {
        if(vetorBloquinhos[11].mostra)
        {
            if((yBolinha < 0.227 && yBolinha > 0.223)  ||
                    (yBolinha > 0.352 && yBolinha < 0.358))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[11].mostra = false;
            }
            if((yBolinha > 0.227 && yBolinha < 0.358) && (xBolinha < -0.67 && xBolinha > -0.66))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[11].mostra = false;
            }
        }

        if(vetorBloquinhos[6].mostra)
        {
            if((yBolinha > 0.44 && yBolinha < 0.46) || (yBolinha > 0.577 && yBolinha < 0.583))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[6].mostra = false;
            }
            if((yBolinha > 0.447 && yBolinha < 0.583) && (xBolinha < -0.6 && xBolinha > -0.7))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[6].mostra = false;
            }
        }

        if(vetorBloquinhos[1].mostra)
        {
            if((yBolinha > 0.67 && yBolinha < 0.68) || (yBolinha > 0.802 && yBolinha < 0.808))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[1].mostra = false;
            }
            if((yBolinha > 0.672 && yBolinha < 0.808) && (xBolinha < -0.6 && xBolinha > -0.7))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[1].mostra = false;
            }
        }
    }

    if(xBolinha > -0.17 && xBolinha < 0.23 )
    {
        if(vetorBloquinhos[12].mostra)
        {
            if((yBolinha < 0.227 && yBolinha > 0.223)  ||
                    (yBolinha > 0.352 && yBolinha < 0.358))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[12].mostra = false;
            }
            if((yBolinha > 0.227 && yBolinha < 0.358) && (xBolinha < -0.67 && xBolinha > -0.66))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[12].mostra = false;
            }
        }

        if(vetorBloquinhos[7].mostra)
        {
            if((yBolinha > 0.44 && yBolinha < 0.46) || (yBolinha > 0.577 && yBolinha < 0.583))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[7].mostra = false;
            }
            if((yBolinha > 0.447 && yBolinha < 0.583) && (xBolinha < -0.6 && xBolinha > -0.7))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[7].mostra = false;
            }
        }

        if(vetorBloquinhos[2].mostra)
        {
            if((yBolinha > 0.67 && yBolinha < 0.68) || (yBolinha > 0.802 && yBolinha < 0.808))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[2].mostra = false;
            }
            if((yBolinha > 0.672 && yBolinha < 0.808) && (xBolinha < -0.6 && xBolinha > -0.7))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[2].mostra = false;
            }
        }
    }

    if(xBolinha > 0.27 && xBolinha < 0.63 )
    {
        if(vetorBloquinhos[13].mostra)
        {
            if((yBolinha < 0.227 && yBolinha > 0.223)  ||
                    (yBolinha > 0.352 && yBolinha < 0.358))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[13].mostra = false;
            }
            if((yBolinha > 0.227 && yBolinha < 0.358) && (xBolinha < -0.67 && xBolinha > -0.66))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[13].mostra = false;
            }
        }

        if(vetorBloquinhos[8].mostra)
        {
            if((yBolinha > 0.44 && yBolinha < 0.46) || (yBolinha > 0.577 && yBolinha < 0.583))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[8].mostra = false;
            }
            if((yBolinha > 0.44 && yBolinha < 0.583) && (xBolinha < -0.6 && xBolinha > -0.7))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[8].mostra = false;
            }
        }

        if(vetorBloquinhos[3].mostra)
        {
            if((yBolinha > 0.67 && yBolinha < 0.68) || (yBolinha > 0.802 && yBolinha < 0.808))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[3].mostra = false;
            }
            if((yBolinha > 0.672 && yBolinha < 0.808) && (xBolinha < -0.6 && xBolinha > -0.7))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[3].mostra = false;
            }
        }
    }

    if(xBolinha > 0.67 && xBolinha < 0.97 )
    {
        if(vetorBloquinhos[14].mostra)
        {
            if((yBolinha < 0.227 && yBolinha > 0.223)  ||
                    (yBolinha > 0.352 && yBolinha < 0.358))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[14].mostra = false;
            }
            if((yBolinha > 0.227 && yBolinha < 0.358) && (xBolinha < -0.67 && xBolinha > -0.66))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[14].mostra = false;
            }
        }

        if(vetorBloquinhos[9].mostra)
        {
            if((yBolinha > 0.44 && yBolinha < 0.46) || (yBolinha > 0.577 && yBolinha < 0.583))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[9].mostra = false;
            }
            if((yBolinha > 0.44 && yBolinha < 0.583) && (xBolinha < -0.6 && xBolinha > -0.7))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[9].mostra = false;
            }
        }

        if(vetorBloquinhos[4].mostra)
        {
            if((yBolinha > 0.67 && yBolinha < 0.68) || (yBolinha > 0.802 && yBolinha < 0.808))
            {
                vetorMovimentoBolinha.v1.y *= -1;
                vetorBloquinhos[4].mostra = false;
            }
            if((yBolinha > 0.672 && yBolinha < 0.808) && (xBolinha < -0.6 && xBolinha > -0.7))
            {
                vetorMovimentoBolinha.v1.x *= -1;
                vetorBloquinhos[4].mostra = false;
            }
        }
    }
}

void preencheVetorBarrinhas()
{
    for(int i=0; i < 15; i++)
    {
        vetorBloquinhos[i].mostra = true;
    }
}

void restart()
{
    pintaPlataformaVermelho = false;
    preencheVetorBarrinhas();
    xBarra = 0;
    xBolinha = 0;
    yBolinha = -0.56;
    vetorMovimentoBolinha.v1.x = 0;
    vetorMovimentoBolinha.v1.y = 0;
    desenhaSetaControle = true;
    primeiroLancamento = false;
}

bool GameOver()
{
    if(yBolinha < -0.8)
        pintaPlataformaVermelho = true;
    if(yBolinha < -1.5)
    {
        vertice v;
        v.x = 0;
        v.y = 0;
        vetor VetorZero;
        VetorZero.v1 = v;
        vetorMovimentoBolinha = VetorZero;
        restart();
        return true;
    }
    return false;
}

bool verificaColisaoX(vertice v) {
    return(fabs(v.x - xBolinha) < 0.1);
}

bool verificaColisaoY(vertice v) {
    return (fabs(v.y - yBolinha) < 0.1);
}

void desenhaPlataforma()
{
    float raio = 0.0;

    vertice vetorNormal;
    vertice base[4] = {{-1.0f, -1.0f,  0.0f},
        { 1.0f, -1.0f,  0.0f},
        { 1.0f,  1.0f,  0.0f},
        {-1.0f,  1.0f,  0.0f}
    };

    vertice faceDireita[4] = {{ 1.0f, -1.0f,  0.0f},
        { 1.0f,  1.0f,  0.0f},
        { 1.0f,  1.0f,  0.25f},
        { 1.0f, -1.0f,  0.25f}
    };

    vertice faceEsquerdaInferior[9];
    float xFaceEsquerdaInferior = -1.0f;
    float yFaceEsquerdaInferior = -1.0f;
    float zFaceEsquerdaInferior = 0.25f;
    faceEsquerdaInferior[0] = {xFaceEsquerdaInferior, yFaceEsquerdaInferior, zFaceEsquerdaInferior};
    for(int i = 1; i < 9; i++) {
        if(i%2==0) {
            zFaceEsquerdaInferior += 0.25f;
        }
        else {
            zFaceEsquerdaInferior -= 0.25f;
        }
        faceEsquerdaInferior[i] = {xFaceEsquerdaInferior, yFaceEsquerdaInferior, zFaceEsquerdaInferior};

        if(i == 1 || i== 6) {
            yFaceEsquerdaInferior += 0.10;
        }
        else if(i == 7) {
            continue;
        }
        else {
            yFaceEsquerdaInferior += 0.05;
        }
    }

    vertice faceEsquerdaSuperior[9];
    float xFaceEsquerdaSuperior = -1.0f;
    float yFaceEsquerdaSuperior = 0.6f;
    float zFaceEsquerdaSuperior = 0.25f;
    faceEsquerdaSuperior[0] = {xFaceEsquerdaSuperior, yFaceEsquerdaSuperior, zFaceEsquerdaSuperior};
    for(int i = 1; i < 9; i++) {
        if(i%2==0) {
            zFaceEsquerdaSuperior += 0.25f;
        }
        else {
            zFaceEsquerdaSuperior -= 0.25f;
        }
        faceEsquerdaSuperior[i] = {xFaceEsquerdaSuperior, yFaceEsquerdaSuperior, zFaceEsquerdaSuperior};

        if(i == 1 || i== 6) {
            yFaceEsquerdaSuperior += 0.10;
        }
        else if(i == 7) {
            continue;
        }
        else {
            yFaceEsquerdaSuperior += 0.05;
        }
    }


    vertice faceSuperior[4] = {{ -1.0f, 1.0f,  0.0f},
        {  1.0f, 1.0f,  0.0f},
        {  1.0f, 1.0f,  0.25f},
        { -1.0f, 1.0f,  0.25f}
    };


    vertice faceEsquerdaBarriga[20];
    float passoX = 0.01f;
    float passoY = 0.1333f;    /// 1.2/9
    float xBarriga = -1.0f;
    float yBarriga = -0.6f;
    float zBarriga = 0.0f;
    int k = 4;
    faceEsquerdaBarriga[0] = {xBarriga, yBarriga, zBarriga};
    for(int i = 1; i < 20; i++) {
        if(i%2==0)  zBarriga = 0.00f;
        else        zBarriga = 0.25f;

        faceEsquerdaBarriga[i] = {xBarriga, yBarriga, zBarriga};

        if(i%2 != 0) {
            if(i >= 9) {
                xBarriga -= k*passoX;
                k++;
            }
            else {
                xBarriga += k*passoX;
                k--;
            }
            yBarriga += passoY;
        }
    }

    triangle trianguloBarrigaEsquerda[18];
    for(int i = 0; i < 18; i++) {
        trianguloBarrigaEsquerda[i] = {faceEsquerdaBarriga[i], faceEsquerdaBarriga[i+1], faceEsquerdaBarriga[i+2]};
    }

    vertice faceDireitaBarriga[20];
    float passoXDir = 0.01f;
    float passoYDir = 0.1333f;    /// 1.2/9
    float xBarrigaDir = 1.0f;
    float yBarrigaDir = -0.6f;
    float zBarrigaDir= 0.0f;
    int kDir = 4;
    faceDireitaBarriga[0] = {xBarrigaDir, yBarrigaDir, zBarrigaDir};
    for(int i = 1; i < 20; i++) {
        if(i%2==0)  zBarrigaDir = 0.00f;
        else        zBarrigaDir = 0.25f;

        faceDireitaBarriga[i] = {xBarrigaDir, yBarrigaDir, zBarrigaDir};

        if(i%2 != 0) {
            if(i >= 9) {
                xBarrigaDir += kDir*passoXDir;
                kDir++;
            }
            else {
                xBarrigaDir -= kDir*passoXDir;
                kDir--;
            }
            yBarrigaDir += passoYDir;
        }
    }

    triangle trianguloBarrigaDireita[18];
    for(int i = 0; i < 18; i++) {
        trianguloBarrigaDireita[i] = {faceDireitaBarriga[i], faceDireitaBarriga[i+1], faceDireitaBarriga[i+2]};
    }

    triangle t[25] = {{base[0], base[1], base[3]},
        {faceEsquerdaInferior[0], faceEsquerdaInferior[1], faceEsquerdaInferior[2]},
        {faceEsquerdaInferior[1], faceEsquerdaInferior[2], faceEsquerdaInferior[3]},
        {faceEsquerdaInferior[2], faceEsquerdaInferior[3], faceEsquerdaInferior[4]},
        {faceEsquerdaInferior[3], faceEsquerdaInferior[4], faceEsquerdaInferior[5]},
        {faceEsquerdaInferior[4], faceEsquerdaInferior[5], faceEsquerdaInferior[6]},
        {faceEsquerdaInferior[5], faceEsquerdaInferior[6], faceEsquerdaInferior[7]},

        {faceEsquerdaSuperior[0], faceEsquerdaSuperior[1], faceEsquerdaSuperior[2]},  /// indice 7
        {faceEsquerdaSuperior[1], faceEsquerdaSuperior[2], faceEsquerdaSuperior[3]},
        {faceEsquerdaSuperior[2], faceEsquerdaSuperior[3], faceEsquerdaSuperior[4]},
        {faceEsquerdaSuperior[3], faceEsquerdaSuperior[4], faceEsquerdaSuperior[5]},
        {faceEsquerdaSuperior[4], faceEsquerdaSuperior[5], faceEsquerdaSuperior[6]},
        {faceEsquerdaSuperior[5], faceEsquerdaSuperior[6], faceEsquerdaSuperior[7]},

        {faceEsquerdaBarriga[1], faceEsquerdaBarriga[9], faceEsquerdaBarriga[2]},   /// indice 13
        {faceEsquerdaBarriga[9], faceEsquerdaBarriga[2], faceEsquerdaBarriga[10]},
        {faceEsquerdaBarriga[2], faceEsquerdaBarriga[10], faceEsquerdaBarriga[3]},
        {faceEsquerdaBarriga[10], faceEsquerdaBarriga[3], faceEsquerdaBarriga[11]},
        {faceEsquerdaBarriga[3], faceEsquerdaBarriga[11], faceEsquerdaBarriga[4]},
        {faceEsquerdaBarriga[11], faceEsquerdaBarriga[4], faceEsquerdaBarriga[12]},
        {faceEsquerdaBarriga[4], faceEsquerdaBarriga[12], faceEsquerdaBarriga[5]},
        {faceEsquerdaBarriga[12], faceEsquerdaBarriga[5], faceEsquerdaBarriga[13]},
        {faceEsquerdaBarriga[5], faceEsquerdaBarriga[13], faceEsquerdaBarriga[6]},
        {faceEsquerdaBarriga[13], faceEsquerdaBarriga[6], faceEsquerdaBarriga[14]}, /// indice 22

        {faceDireita[0], faceDireita[1], faceDireita[2]},
        {faceSuperior[0], faceSuperior[1], faceSuperior[2]}
    };

    setColorBase();
    if(pintaPlataformaVermelho)
        setColor(1.0, 0.0, 0.0);
    else
        setColor(0.1, 0.1, 0.1);
    glBegin(GL_QUADS);
    CalculaNormal(t[0], &vetorNormal);
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(base[i].x, base[i].y, base[i].z);
    }
    glEnd();


    setColor(0.6, 0.6, 0.9);
    glBegin(GL_QUADS);
    CalculaNormal(t[23], &vetorNormal);
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(faceDireita[i].x, faceDireita[i].y, faceDireita[i].z);
    }
    glEnd();
    if(verificaColisaoX(faceDireita[0])) {
        refleteBolinha(vetorNormal);
    }


    /// ---------------- FACE ESQUERDA ---------------
    /// parte inferior
    setColor(0.6, 0.6, 0.9);
    glBegin(GL_TRIANGLE_STRIP);
    for(int i = 1; i < 6; i++){
        CalculaNormal(t[i], &vetorNormal);
        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    }
    for(int i = 0; i < 9; i++){
        glVertex3f(faceEsquerdaInferior[i].x, faceEsquerdaInferior[i].y, faceEsquerdaInferior[i].z);
    }
    glEnd();

    ///------BARRIGA ESQUERDA-----
    for(int i = 1; i <= 18; i++){    /// setta as normais
        CalculaNormal(trianguloBarrigaEsquerda[i], &vetorNormal);
        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    }
    glBegin(GL_TRIANGLE_STRIP); ///desenha a barriga já costurando os triângulos
    for(int i = 0; i < 20; i++){
        glVertex3f(faceEsquerdaBarriga[i].x, faceEsquerdaBarriga[i].y, faceEsquerdaBarriga[i].z);
    }
    glEnd();

    glBegin(GL_POLYGON);    /// Preenche a tampa da barriga
    for(int i = 1; i < 20; i+=2){
        glVertex3f(faceEsquerdaBarriga[i].x, faceEsquerdaBarriga[i].y, faceEsquerdaBarriga[i].z);
    }
    glEnd();

    glBegin(GL_POLYGON);    /// preenche a lateral de fora da barriga
        glVertex3f(faceEsquerdaBarriga[0].x, faceEsquerdaBarriga[0].y, faceEsquerdaBarriga[0].z);
        glVertex3f(faceEsquerdaBarriga[18].x, faceEsquerdaBarriga[18].y, faceEsquerdaBarriga[18].z);
        glVertex3f(faceEsquerdaBarriga[19].x, faceEsquerdaBarriga[19].y, faceEsquerdaBarriga[19].z);
        glVertex3f(faceEsquerdaBarriga[1].x, faceEsquerdaBarriga[1].y, faceEsquerdaBarriga[1].z);
    glEnd();

    ///------BARRIGA DIREITA-----
     glBegin(GL_TRIANGLE_STRIP);
    for(int i = 1; i <= 18; i++){    /// setta as normais
        CalculaNormal(trianguloBarrigaDireita[i], &vetorNormal);
        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    }
    ///desenha a barriga já costurando os triângulos
    for(int i = 0; i < 20; i++){
        glVertex3f(faceDireitaBarriga[i].x, faceDireitaBarriga[i].y, faceDireitaBarriga[i].z);
    }
    glEnd();

    glBegin(GL_POLYGON);    /// Preenche a tampa da barriga
    for(int i = 1; i < 20; i+=2){
        glVertex3f(faceDireitaBarriga[i].x, faceDireitaBarriga[i].y, faceDireitaBarriga[i].z);
    }
    glEnd();

    glBegin(GL_POLYGON);    /// preenche a lateral de fora da barriga
        glVertex3f(faceDireitaBarriga[0].x, faceDireitaBarriga[0].y, faceDireitaBarriga[0].z);
        glVertex3f(faceDireitaBarriga[18].x, faceDireitaBarriga[18].y, faceDireitaBarriga[18].z);
        glVertex3f(faceDireitaBarriga[19].x, faceDireitaBarriga[19].y, faceDireitaBarriga[19].z);
        glVertex3f(faceDireitaBarriga[1].x, faceDireitaBarriga[1].y, faceDireitaBarriga[1].z);
    glEnd();

    /// parte superior
    glBegin(GL_TRIANGLE_STRIP);
    for(int i = 7; i < 12; i++){
        CalculaNormal(t[i], &vetorNormal);
        glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    }
    for(int i = 0; i < 9; i++){
        glVertex3f(faceEsquerdaSuperior[i].x, faceEsquerdaSuperior[i].y, faceEsquerdaSuperior[i].z);
    }
    glEnd();
    ///--------FIM DA FACE ESQUERDA---------

    setColor(0.6, 0.6, 0.9);
    glBegin(GL_QUADS);
    CalculaNormal(t[24], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(faceSuperior[i].x, faceSuperior[i].y, faceSuperior[i].z);
    }
    glEnd();
    if(verificaColisaoY(faceSuperior[0])) {
        refleteBolinha(vetorNormal);
    }
}

void desenhaBarra()
{
    vertice vetorNormal;
    vertice barraFaceTampa[4] = {{-0.25 + xBarra, -0.75, 0.125},
        {0.25 + xBarra, -0.75, 0.125},
        {0.25 + xBarra, -0.625, 0.125},
        {-0.25 + xBarra, -0.625, 0.125}
    };

    vertice barraFaceBase[4] = {{-0.25 + xBarra, -0.625, 0.0625},
        {-0.25 + xBarra, -0.75, 0.0625},
        {0.25 + xBarra, -0.75, 0.0625},
        {0.25 + xBarra, -0.625, 0.0625}
    };

    vertice barraFaceDireita[4] = {{0.25 + xBarra, -0.75, 0.0625},
        {0.25 + xBarra, -0.625, 0.0625},
        {0.25 + xBarra, -0.625, 0.125},
        {0.25 + xBarra, -0.75, 0.125}
    };

    vertice barraFaceSuperior[4] = {{0.25 + xBarra, -0.625, 0.0625},
        {0.25 + xBarra, -0.625, 0.125},
        {-0.25 + xBarra, -0.625, 0.125},
        {-0.25 + xBarra, -0.625, 0.0625}
    };

    vertice barraFaceEsquerda[4] = {{-0.25 + xBarra, -0.75, 0.0625},
        {-0.25 + xBarra, -0.625, 0.0625},
        {-0.25 + xBarra, -0.625, 0.125},
        {-0.25 + xBarra, -0.75, 0.125}
    };

    vertice barraFaceInferior[4] = {{0.25 + xBarra, -0.75, 0.0625},
        {0.25 + xBarra, -0.75, 0.125},
        {-0.25 + xBarra, -0.75, 0.125},
        {-0.25 + xBarra, -0.75, 0.0625}
    };

    triangle t[6] = {{barraFaceTampa[0], barraFaceTampa[1], barraFaceTampa[2]},
        {barraFaceBase[0], barraFaceBase[1], barraFaceBase[2]},
        {barraFaceDireita[0], barraFaceDireita[1], barraFaceDireita[2]},
        {barraFaceSuperior[0], barraFaceSuperior[1], barraFaceSuperior[2]},
        {barraFaceEsquerda[0], barraFaceEsquerda[1], barraFaceEsquerda[2]},
        {barraFaceInferior[0], barraFaceInferior[1], barraFaceInferior[2]}
    };


    setColor(0.1, 0.9, 0.1);
    glPushMatrix();
    glBegin(GL_QUADS);
    CalculaNormal(t[0], &vetorNormal);
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(barraFaceTampa[i].x, barraFaceTampa[i].y, barraFaceTampa[i].z);
    }
    glEnd();

    glBegin(GL_QUADS);
    CalculaNormal(t[1], &vetorNormal);
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(barraFaceBase[i].x, barraFaceBase[i].y, barraFaceBase[i].z);
    }
    glEnd();

    glBegin(GL_QUADS);
    CalculaNormal(t[2], &vetorNormal);
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(barraFaceDireita[i].x, barraFaceDireita[i].y, barraFaceDireita[i].z);
    }
    glEnd();

    glBegin(GL_QUADS);
    CalculaNormal(t[3], &vetorNormal);
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(barraFaceSuperior[i].x, barraFaceSuperior[i].y, barraFaceSuperior[i].z);
    }
    glEnd();

    glBegin(GL_QUADS);
    CalculaNormal(t[4], &vetorNormal);
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(barraFaceEsquerda[i].x, barraFaceEsquerda[i].y, barraFaceEsquerda[i].z);
    }
    glEnd();

    glBegin(GL_QUADS);
    CalculaNormal(t[5], &vetorNormal);
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(barraFaceInferior[i].x, barraFaceInferior[i].y, barraFaceInferior[i].z);
    }
    glEnd();

    glPopMatrix();
}


void desenhaBarrinhasDeBater()
{
    int colunas = 0;
    int linhas = 0;
    int cont = 0;

    float inicio = -0.95;
    float fim = -0.65;

    float inicioh = 0.90;
    float fimh = 0.775;

    while(linhas < 3)
    {
        colunas = 0;
        inicio = -0.95;
        fim = -0.65;

        while(colunas < 5)
        {
            vertice vetorNormal;
            vertice barraFaceTampa[4] = {{fim, inicioh, 0.125},
                {inicio, inicioh, 0.125},
                {inicio, fimh, 0.125},
                {fim, fimh, 0.125}
            };

            vertice barraFaceBase[4] = {{fim, fimh, 0.0625},
                {fim, inicioh, 0.0625},
                {inicio, inicioh, 0.0625},
                {inicio, fimh, 0.0625}
            };

            vertice barraFaceDireita[4] = {{inicio, inicioh, 0.0625},
                {inicio, fimh, 0.0625},
                {inicio, fimh, 0.125},
                {inicio, inicioh, 0.125}
            };

            vertice barraFaceSuperior[4] = {{inicio, fimh, 0.0625},
                {inicio, fimh, 0.125},
                {fim, fimh, 0.125},
                {fim, fimh, 0.0625}
            };

            vertice barraFaceEsquerda[4] = {{fim, inicioh, 0.0625},
                {fim, fimh, 0.0625},
                {fim, fimh, 0.125},
                {fim, inicioh, 0.125}
            };

            vertice barraFaceInferior[4] = {{inicio, inicioh, 0.0625},
                {inicio, inicioh, 0.125},
                {fim, inicioh, 0.125},
                {fim, inicioh, 0.0625}
            };

            triangle t[6] = {{barraFaceTampa[0], barraFaceTampa[1], barraFaceTampa[2]},
                {barraFaceBase[0], barraFaceBase[1], barraFaceBase[2]},
                {barraFaceDireita[0], barraFaceDireita[1], barraFaceDireita[2]},
                {barraFaceSuperior[0], barraFaceSuperior[1], barraFaceSuperior[2]},
                {barraFaceEsquerda[0], barraFaceEsquerda[1], barraFaceEsquerda[2]},
                {barraFaceInferior[0], barraFaceInferior[1], barraFaceInferior[2]}
            };


            setColor(0.1, 0.9, 0.1);
            glPushMatrix();

            if(vetorBloquinhos[cont].mostra == true)
            {
                glBegin(GL_QUADS);
                CalculaNormal(t[0], &vetorNormal);
                glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                for(int i=0; i < 4; i++)
                {
                    glVertex3f(barraFaceTampa[i].x, barraFaceTampa[i].y, barraFaceTampa[i].z);
                }
                glEnd();



                glBegin(GL_QUADS);
                CalculaNormal(t[1], &vetorNormal);
                glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                for(int i=0; i < 4; i++)
                {
                    glVertex3f(barraFaceBase[i].x, barraFaceBase[i].y, barraFaceBase[i].z);
                }
                glEnd();



                glBegin(GL_QUADS);
                CalculaNormal(t[2], &vetorNormal);
                glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                for(int i=0; i < 4; i++)
                {
                    glVertex3f(barraFaceDireita[i].x, barraFaceDireita[i].y, barraFaceDireita[i].z);
                }
                glEnd();



                glBegin(GL_QUADS);
                CalculaNormal(t[3], &vetorNormal);
                glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                for(int i=0; i < 4; i++)
                {
                    glVertex3f(barraFaceSuperior[i].x, barraFaceSuperior[i].y, barraFaceSuperior[i].z);
                }
                glEnd();



                glBegin(GL_QUADS);
                CalculaNormal(t[4], &vetorNormal);
                glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                for(int i=0; i < 4; i++)
                {
                    glVertex3f(barraFaceEsquerda[i].x, barraFaceEsquerda[i].y, barraFaceEsquerda[i].z);
                }
                glEnd();




                glBegin(GL_QUADS);
                CalculaNormal(t[5], &vetorNormal);
                glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
                for(int i=0; i < 4; i++)
                {
                    glVertex3f(barraFaceInferior[i].x, barraFaceInferior[i].y, barraFaceInferior[i].z);
                }
                glEnd();
            }

            glPopMatrix();

            colunas++;
            cont++;

            inicio += 0.40;
            fim += 0.40;

        }
        linhas++;
        inicioh += -0.225;
        fimh += -0.225;
    }

}

void desenhaBolinha()
{
    if(!GameOver())
    {
        setColor(1.0, 0.5, 0.1);
        glPushMatrix();
        glTranslatef(xBolinha, yBolinha, 0.125);
        glutSolidSphere(0.0625, 20, 20);
        glPopMatrix();
    }
}

void desenhaSeta()
{
    if(desenhaSetaControle)
    {
        vetorSeta->v1.x = xSeta;
        atualizaVetorSeta();
        glPushMatrix();
        setColor(0.0, 1.0, 0.3);
        glBegin(GL_LINES);
        glVertex3f(xBolinha, yBolinha, 0.125);
        glVertex3f(xBolinha + vetorSeta->v1.x, yBolinha + vetorSeta->v1.y, 0.125);
        glEnd();
        glPopMatrix();
    }
}


void drawObject()
{
    desenhaPlataforma();
    desenhaBarra();
    desenhaBarrinhasDeBater();

    desenhaBolinha();
    desenhaSeta();
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    if(projecao_ortogonal)
    {
        gluLookAt (.0, -2.0, 1.8, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        glPushMatrix();
        if(pausado && rotacaoLiberada)
        {
            glRotatef( rotationY, 0.0, 1.0, 0.0 );
            rotacaoY = rotationY;
            glRotatef( rotationX, 1.0, 0.0, 0.0 );
            rotacaoX = rotationX;
        }
        else
        {
            glRotatef( rotacaoY, 0.0, 1.0, 0.0 );
            glRotatef( rotacaoX, 1.0, 0.0, 0.0 );
        }

        drawObject();

        glPopMatrix();
    }


    if(!projecao_ortogonal)
    {

        gluLookAt (0.0, 0.0, zdist, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

        glPushMatrix();
        glRotatef( 0, 0.0, 1.0, 0.0 );
        glRotatef( 0, 1.0, 0.0, 0.0 );
        drawObject();
        glPopMatrix();

    }
    glutSwapBuffers();
    moveBolinha();
    reflexaoBarra();
    reflexaoBloquinhos();
}

void idle ()
{
    glutPostRedisplay();
}

void reshape (int w, int h)
{
    width = w;
    height = h;

    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.01, 200.0);
}

void keyboard (unsigned char key, int x, int y)
{

    switch (tolower(key))
    {

    case 27:
        exit(0);
        break;
    case 32:
        if(pausado)
        {
            pausado = false;
            podeMoverABolinha = false;
        }
        else{
            pausado = true;

        }
        break;
    case 'p':
        if(projecao_ortogonal)
        {
            projecao_ortogonal = false;
        }
        else{
            projecao_ortogonal = true;
        }
        break;
    case 'c':
        if(projecao_ortogonal && pausado)
        {
            if(rotacaoLiberada){
                rotacaoLiberada = false;
            }
            else{
                rotacaoLiberada = true;
            }
        break;
    case 'r':
        restart();
        break;
    }
}
}

void specialKeys(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_F12:
        janela++;
        if(janela%2==0)
        {
            glutReshapeWindow(1000, 600);
            glutPositionWindow(100, 100);

        }
        else
        {

            glutFullScreen();
        }
        break;
    }
    glutPostRedisplay();
}

// Motion callback
void motion(int x, int y )
{
    if(pausado)
    {
        if(projecao_ortogonal && rotacaoLiberada)
        {
            rotationX += (float) (y - last_y);
            rotationY += (float) (x - last_x);

            last_x = x;
            last_y = y;
        }
    }
}

void motionBarra(int x, int y)
{
    if(!pausado){
        xBarra = (float)x/250 - 2;
        if(xBarra >= -1.0 && xBarra <= 1.0)
        {
            if(!primeiroLancamento)
            {
                xBolinha = (float)x/250 - 2;
            }
        }
    }

}



// Mouse callback
void mouse(int button, int state, int x, int y)
{

        if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
        {
            last_x = x;
            last_y = y;
        }
        if(button == 3) // Scroll up
        {
            if(xSeta <= 0.40)
            {
                xSeta += 0.05;
                atualizaVetorSeta();
            }
        }
        if(button == 4) // Scroll Down
        {
            if(xSeta >= -0.40)
            {
                xSeta -= 0.05;
                atualizaVetorSeta();
            }
        }


    if(!pausado)
    {
        if(!primeiroLancamento){
            if(button ==  GLUT_LEFT_BUTTON)
            {
                desenhaSetaControle = false;
                vetorMovimentoBolinha.v1.x = vetorSeta->v1.x/110;
                vetorMovimentoBolinha.v1.y = vetorSeta->v1.y/110;
                primeiroLancamento = true;

            }
        }
    }

}



/// Main
int main(int argc, char** argv)
{
    preencheVetorBarrinhas();
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (1000, 600);
    glutInitWindowPosition (100, 100);
    glutCreateWindow (argv[0]);
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc( mouse );
    glutMotionFunc( motion );
    glutPassiveMotionFunc( motionBarra );
    glutKeyboardFunc(keyboard);
    glutSpecialFunc( specialKeys );
    glutSetCursor(GLUT_CURSOR_NONE);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
