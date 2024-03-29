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
bool desenhaSetaControle = true;
bool pintaPlataformaVermelho = false;
vetor* vetorSeta = new vetor();
barrinhas* vetorBloquinhos = new barrinhas[14];
vetor vetorMovimentoBolinha;


/// Functions
void init(void)
{
    initLight(width, height); // Função extra para tratar iluminação.
}

/* Exemplo de cálculo de vetor normal que são definidos a partir dos vértices do triângulo;
  v_2
  ^
  |\
  | \
  |  \       'vn' é o vetor normal resultante
  |   \
  +----> v_1
  v_0
*/
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

///Proj U em V = (u.v/|v|²)*V
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

///R = I - 2*(N.I)*N
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

ou usa gluperspective ou glortho (dentro da matriz de projecao, que vai ter um ou outro)


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
    if(fabs(v.x - xBolinha) < 0.1)
        return true;

    return false;
}

bool verificaColisaoY(vertice v) {
    if(fabs(v.y - yBolinha) < 0.1)
        return true;

    return false;
}

void desenhaPlataforma()
{
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

    vertice faceEsquerda[4] = {{ -1.0f, -1.0f,  0.0f},
        { -1.0f, 1.0f,  0.0f},
        { -1.0f, 1.0f,  0.25f},
        { -1.0f, -1.0f,  0.25f}
    };

    vertice faceSuperior[4] = {{ -1.0f, 1.0f,  0.0f},
        {  1.0f, 1.0f,  0.0f},
        {  1.0f, 1.0f,  0.25f},
        { -1.0f, 1.0f,  0.25f}
    };



    triangle t[4] = {{base[0], base[1], base[3]},
        {faceEsquerda[0], faceEsquerda[1], faceEsquerda[2]},
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
    CalculaNormal(t[1], &vetorNormal);
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(faceDireita[i].x, faceDireita[i].y, faceDireita[i].z);
    }
    glEnd();
    if(verificaColisaoX(faceDireita[0])) {
        refleteBolinha(vetorNormal);
    }

    setColor(0.6, 0.6, 0.9);
    glBegin(GL_QUADS);
    CalculaNormal(t[2], &vetorNormal);
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(faceEsquerda[i].x, faceEsquerda[i].y, faceEsquerda[i].z);
    }
    glEnd();
    if(verificaColisaoX(faceEsquerda[0])) {
        refleteBolinha(vetorNormal);
    }

    setColor(0.6, 0.6, 0.9);
    glBegin(GL_QUADS);
    CalculaNormal(t[3], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
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
    //preencheVetorBarrinhas();
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
