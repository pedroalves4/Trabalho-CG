#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <GLFW/glfw3.h>

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


class vetor {
    public:
        vertice v1;
        vertice v0;

    vetor() {
        v0.x = 0.0;
        v0.y = 0.0;
    }
};


/// Globals
float zdist = 3.0;
float rotationX = 0.0, rotationY = 0.0;
float rotacaoX = 0.0, rotacaoY = 0.0;
int   last_x, last_y;
int   width, height;
int game = 0;
int projecao = 0;
int liberaRotacao = 0;
int janela = 0;
float xBarra = 0.0;
float xBolinha = 0.0;
float yBolinha = -0.56;
float xSeta = 0.15;
vetor* vetorSeta = new vetor();
vetor* vetorMovimentoBolinha = new vetor();


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

void atualizaVetorMovimentoBolinha() {
     vetorMovimentoBolinha->v1.y = 0.15*0.15 - vetorMovimentoBolinha->v1.x;
}

void moveBolinha() {
    xBolinha += vetorMovimentoBolinha->v1.x;
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

    setColor(0.6, 0.6, 0.9);
    glBegin(GL_QUADS);
    CalculaNormal(t[2], &vetorNormal);
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(faceEsquerda[i].x, faceEsquerda[i].y, faceEsquerda[i].z);
    }
    glEnd();

    setColor(0.6, 0.6, 0.9);
    glBegin(GL_QUADS);
    CalculaNormal(t[3], &vetorNormal); // Passa face triangular e endereço do vetor normal de saída
    glNormal3f(vetorNormal.x, vetorNormal.y,vetorNormal.z);
    for(int i=0; i < 4; i++)
    {
        glVertex3f(faceSuperior[i].x, faceSuperior[i].y, faceSuperior[i].z);
    }
    glEnd();
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

            colunas++;


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
    setColor(1.0, 0.5, 0.1);
    glPushMatrix();
    glTranslatef(xBolinha, yBolinha, 0.125);
    glutSolidSphere(0.0625, 20, 20);
    glPopMatrix();
}


void desenhaSeta() {
    vetorSeta->v1.x = xSeta;
    glPushMatrix();
        setColor(0.0, 1.0, 0.3);
        glBegin(GL_LINES);
            glVertex3f(xBolinha, yBolinha, 0.125);
            glVertex3f(xBolinha + vetorSeta->v1.x, yBolinha + 0.25, 0.125);
        glEnd();
    glPopMatrix();
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


    if(projecao%2==0)
    {
        gluLookAt (.0, -2.0, 1.8, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        glPushMatrix();
        if(game%2==0 && liberaRotacao%2==0)
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


    if(projecao%2 != 0)
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
    atualizaVetorMovimentoBolinha();
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
            game++;
            break;
        case 'p':
            projecao++;
            break;
        case 'c':
            if(projecao%2==0 && game%2==0){
                liberaRotacao++;
            }
        case 't':
            vetorMovimentoBolinha = vetorSeta;
            break;


        case 'i':
            janela++;
            if(janela%2==0)
            {
                glutReshapeWindow(1000, 600);
                glutPositionWindow(100, 100);

            } else{

                glutFullScreen();
            }
            break;
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

            } else{

                glutFullScreen();
            }
        break;
    }
    glutPostRedisplay();
}

// Motion callback
void motion(int x, int y )
{
    if(game%2 == 0)
    {
        if(projecao%2 == 0 && liberaRotacao%2==0)
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
    if(game%2 == 1)
    {
        if(xBarra >= -1.0 && xBarra <= 1.0)

        {
            xBarra = (float)x/250 - 2;
            xBolinha = (float)x/250 - 2;
        }
    }

}

// Mouse callback
void mouse(int button, int state, int x, int y)
{
    if(game%2 == 0) {
        if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
        {
            last_x = x;
            last_y = y;
        }
        if(button == 3) // Scroll up
        {
            xSeta += 0.05;
        }
        if(button == 4) // Scroll Down
        {
            xSeta -= 0.05;
        }
    }
   /* else {
        if( button ==  GLUT_LEFT_BUTTON) {
            vetorMovimentoBolinha = vetorSeta;
        }
    }*/
}


/// Main
int main(int argc, char** argv)
{
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
