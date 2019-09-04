#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define LARGURA_DO_MUNDO 1920
#define ALTURA_DO_MUNDO 1080

GLuint carregaTextura(const char* arquivo) {
    GLuint idTextura = SOIL_load_OGL_texture(
                           arquivo,
                           SOIL_LOAD_AUTO,
                           SOIL_CREATE_NEW_ID,
                           SOIL_FLAG_INVERT_Y
                       );

    if (idTextura == 0) {
        printf("Erro do SOIL: '%s'\n", SOIL_last_result());
    }

    return idTextura;
}

struct vetor {

    float x, y;

};

struct tamanho {
    float largura, altura;
};

struct sprite {

    struct vetor posicao;
    struct tamanho tamanho;
    int movendo;        // 1 = cima, 2 = baixo;
    GLuint idTextura;
    struct vetor direcao;
    int pontos;
    int vitoria;

};

struct sprite player1;
struct sprite player2;
struct sprite pokemon1;
struct sprite pokemon2;
struct sprite bola;

struct vetor posicaoMouse;

int velocidade_players=15;
GLuint texturaFundo;
GLuint texturaBarras;
GLuint texturaPause;
GLuint texturaVitoria1;
GLuint texturaVitoria2;
GLuint texturaBulbassauroDireita;
GLuint texturaBulbassauroEsquerda;
GLuint texturaCharmanderDireita;
GLuint texturaCharmanderEsquerda;
GLuint texturaSquirtleDireita;
GLuint texturaSquirtleEsquerda;
GLuint texturaReset;
GLuint texturaEsc;
int pause=0;
int resetar=0;
int sair=0;

void checarVitoria(){

    if(player1.pontos==4)
        player1.vitoria=1;
    if(player2.pontos==4)
        player2.vitoria=1;

}

void inicializa();
void reiniciarJogada();

void resetarJogo(){

    if(resetar==2)
    {
        inicializa();
        reiniciarJogada();
        resetar=0;

    }

}

void iniciarTexturas(){

    texturaBulbassauroDireita=carregaTextura("bulbassauro_direita.png");
    texturaBulbassauroEsquerda=carregaTextura("bulbassauro_esquerda.png");
    texturaCharmanderDireita=carregaTextura("charmander_direita.png");
    texturaCharmanderEsquerda=carregaTextura("charmander_esquerda.png");
    texturaSquirtleDireita=carregaTextura("squirtle_direita.png");
    texturaSquirtleEsquerda=carregaTextura("squirtle_esquerda.png");
    texturaEsc=carregaTextura("esc.png");
    texturaFundo=carregaTextura("fundo_cortado.png");
    texturaBarras=carregaTextura("barras_laterais.png");
    texturaPause=carregaTextura("pause.png");
    texturaVitoria1=carregaTextura("player1_ganhou.png");
    texturaVitoria2=carregaTextura("player2_ganhou.png");
    texturaReset=carregaTextura("reset.png");

}

void reiniciarJogada() {

    player1.posicao.x=477.5;
    player1.posicao.y=540;
    player2.posicao.x=1442.5;
    player2.posicao.y=540;
    bola.posicao.x=960;
    bola.posicao.y=540;
    pokemon1.posicao.x=300;
    pokemon1.posicao.y=540;
    pokemon2.posicao.x=1620;
    pokemon2.posicao.y=540;

    if(1+rand()%2==1)                       //gera um vetor aleatorio de ate de (-1,-1) a (1,1).
        bola.direcao.x=0.5+rand()%100/200.0;
    else
        bola.direcao.x=-0.5-rand()%100/200.0;
    if(1+rand()%2==1)
        bola.direcao.y=0.5+rand()%100/200.0;
    else
        bola.direcao.y=-0.5-rand()%100/200.0;

}

void alterarTexturaPokemon(){

    if(player1.pontos==1)
        pokemon2.idTextura=carregaTextura("bulbassauro_direita.png");
    if(player1.pontos==2)
        pokemon2.idTextura=carregaTextura("squirtle_direita.png");
    if(player2.pontos==1)
        pokemon1.idTextura=carregaTextura("bulbassauro_esquerda.png");
    if(player2.pontos==2)
        pokemon1.idTextura=carregaTextura("squirtle_esquerda.png");;
    if(player1.pontos==3)
        pokemon2.idTextura=carregaTextura("mewtwo_direita.png");
    if(player2.pontos==3)
        pokemon1.idTextura=carregaTextura("mewtwo_esquerda.png");

}

void checarPontuacao() {

    if(bola.posicao.x < (pokemon1.posicao.x + pokemon1.tamanho.largura/2) && bola.posicao.x > (pokemon1.posicao.x - pokemon1.tamanho.largura/2))
    {
        player2.pontos++;
        alterarTexturaPokemon();
        reiniciarJogada();
    }
    if(bola.posicao.x < (pokemon2.posicao.x + pokemon2.tamanho.largura/2) && bola.posicao.x > (pokemon2.posicao.x - pokemon2.tamanho.largura/2))
    {
        player1.pontos++;
        alterarTexturaPokemon();
        reiniciarJogada();
    }

}

void movimentarBola() {

    //colisão no teto e chão
    if(bola.posicao.y >= 1080-bola.tamanho.altura/2)
        bola.direcao.y = -bola.direcao.y;
    if(bola.posicao.y <= 0+bola.tamanho.altura/2)
        bola.direcao.y = -bola.direcao.y;
    //colisao nas paredes, depois se tornará pontos.
    if(bola.posicao.x >= 1720-bola.tamanho.largura/2)
        bola.direcao.x = -bola.direcao.x;
    if(bola.posicao.x <= 200+bola.tamanho.largura/2)
        bola.direcao.x = -bola.direcao.x;
    //colisao com o player 1
    if(bola.posicao.x-bola.tamanho.largura/2 < player1.posicao.x+player1.tamanho.largura/2 && bola.posicao.x-bola.tamanho.largura/2 > player1.posicao.x-player1.tamanho.largura/2 && bola.posicao.y-bola.tamanho.altura/2 < player1.posicao.y+player1.tamanho.altura/2 && bola.posicao.y-bola.tamanho.altura/2 > player1.posicao.y-player1.tamanho.altura/2)
    {
        bola.direcao.x = -bola.direcao.x;
        bola.direcao.x *= 1.1;
        bola.direcao.y *= 1.1;
    }
    //colisao com o player 2
    if(bola.posicao.x+bola.tamanho.largura/2 < player2.posicao.x+player2.tamanho.largura/2 && bola.posicao.x+bola.tamanho.largura/2 > player2.posicao.x-player2.tamanho.largura/2 && bola.posicao.y+bola.tamanho.altura/2 < player2.posicao.y+player2.tamanho.altura/2 && bola.posicao.y+bola.tamanho.altura/2 > player2.posicao.y-player2.tamanho.altura/2)
    {
        bola.direcao.x = -bola.direcao.x;
        bola.direcao.x *= 1.1;
        bola.direcao.y *= 1.1;
    }
    bola.posicao.x += 10*bola.direcao.x;
    bola.posicao.y += 10*bola.direcao.y;

}

void movimentarPlayer() {

    if(player1.movendo==1 && player1.posicao.y < 1050-player1.tamanho.altura/2)
        player1.posicao.y+=velocidade_players;
    else if(player1.movendo==2 && player1.posicao.y > 30+player1.tamanho.altura/2)
        player1.posicao.y-=velocidade_players;
    if(player2.movendo==1 && player2.posicao.y < 1050-player1.tamanho.altura/2)
        player2.posicao.y+=velocidade_players;
    else if(player2.movendo==2 && player2.posicao.y > 30+player1.tamanho.altura/2)
        player2.posicao.y-=velocidade_players;


}

void desenhaRetangulo(float pos_x, float pos_y , float larg, float alt){

    glPushMatrix();
        glTranslatef(pos_x, pos_y, 0);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(-larg/2, -alt/2);
            glVertex2f( larg/2, -alt/2);
            glVertex2f( larg/2,  alt/2);
            glVertex2f(-larg/2,  alt/2);
        glEnd();
    glPopMatrix();

}

void desenhaRetanguloTextura(float pos_x, float pos_y , float larg, float alt, GLuint textura){

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textura);
    glPushMatrix();
        glTranslatef(pos_x, pos_y, 0);
        glBegin(GL_TRIANGLE_FAN);
            glTexCoord2f(0,0); glVertex2f(-larg/2, -alt/2);
            glTexCoord2f(1,0); glVertex2f( larg/2, -alt/2);
            glTexCoord2f(1,1); glVertex2f( larg/2,  alt/2);
            glTexCoord2f(0,1); glVertex2f(-larg/2,  alt/2);
        glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}

void inicializa_Players_e_Bola(){

    player1.tamanho.largura = 177.5;
    player1.tamanho.altura = 200;
    player2.tamanho.largura = 177.5;
    player2.tamanho.altura = 200;
    bola.tamanho.largura = 75;
    bola.tamanho.altura = 75;
    pokemon1.tamanho.largura = 177.5;
    pokemon1.tamanho.altura = 177.5;
    pokemon2.tamanho.largura = 177.5;
    pokemon2.tamanho.altura = 177.5;

    player1.posicao.x=477.5;
    player1.posicao.y=540;
    player2.posicao.x=1442.5;
    player2.posicao.y=540;
    bola.posicao.x=960;
    bola.posicao.y=540;
    pokemon1.posicao.x=300;
    pokemon1.posicao.y=540;
    pokemon2.posicao.x=1620;
    pokemon2.posicao.y=540;

    player1.idTextura=carregaTextura("player1_cortado.png");
    player2.idTextura=carregaTextura("player2_cortado.png");
    bola.idTextura=carregaTextura("bola.png");
    pokemon1.idTextura=carregaTextura("charmander_esquerda.png");
    pokemon2.idTextura=carregaTextura("charmander_direita.png");

    player1.movendo=0;
    player2.movendo=0;
    bola.movendo=1;

    if(1+rand()%2==1)                       //gera um vetor aleatorio de ate de (-1,-1) a (1,1).
        bola.direcao.x=0.5+rand()%100/200.0;
    else
        bola.direcao.x=-0.5-rand()%100/200.0;
    if(1+rand()%2==1)
        bola.direcao.y=0.5+rand()%100/200.0;
    else
        bola.direcao.y=-0.5-rand()%100/200.0;

    player1.pontos=0;
    player2.pontos=0;
    player1.vitoria=0;
    player2.vitoria=0;

}

void desenhaCena() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1,1,1);
    //Desenhar fundo
    desenhaRetanguloTextura(960,540,1920,1080,texturaFundo);
    //Desenhar barras
    desenhaRetanguloTextura(100,540,200,1080,texturaBarras);
    desenhaRetanguloTextura(1820,540,200,1080,texturaBarras);
    //Desenhar players , bola e pokemons.
    desenhaRetanguloTextura(player1.posicao.x,player1.posicao.y,player1.tamanho.largura,player1.tamanho.altura,player1.idTextura);
    desenhaRetanguloTextura(player2.posicao.x,player2.posicao.y,player2.tamanho.largura,player2.tamanho.altura,player2.idTextura);
    desenhaRetanguloTextura(pokemon1.posicao.x,pokemon1.posicao.y,pokemon1.tamanho.largura,pokemon1.tamanho.altura,pokemon1.idTextura);
    desenhaRetanguloTextura(pokemon2.posicao.x,pokemon2.posicao.y,pokemon2.tamanho.largura,pokemon2.tamanho.altura,pokemon2.idTextura);
    desenhaRetanguloTextura(bola.posicao.x,bola.posicao.y,bola.tamanho.largura,bola.tamanho.altura,bola.idTextura);
    //Desenhar pontuaçao
    if(player1.pontos>0)
    desenhaRetanguloTextura(100,717.5,177.5,177.5,texturaCharmanderEsquerda);
    if(player1.pontos>1)
    desenhaRetanguloTextura(100,540,177.5,177.5,texturaBulbassauroEsquerda);
    if(player1.pontos>2)
    desenhaRetanguloTextura(100,362.5,177.5,177.5,texturaSquirtleEsquerda);
    if(player2.pontos>0)
    desenhaRetanguloTextura(1820,717.5,177.5,177.5,texturaCharmanderDireita);
    if(player2.pontos>1)
    desenhaRetanguloTextura(1820,540,177.5,177.5,texturaBulbassauroDireita);
    if(player2.pontos>2)
    desenhaRetanguloTextura(1820,362.5,177.5,177.5,texturaSquirtleDireita);
    //Desenhar texturas das telas
    if(pause==1)
        desenhaRetanguloTextura(960,540,1920,1080,texturaPause);
    if(player1.vitoria==1)
        desenhaRetanguloTextura(960,540,1920,1080,texturaVitoria1);
    if(player2.vitoria==1)
        desenhaRetanguloTextura(960,540,1920,1080,texturaVitoria2);
    if(resetar==1)
        desenhaRetanguloTextura(960,540,1920,1080,texturaReset);
    if(sair==1)
        desenhaRetanguloTextura(960,540,1920,1080,texturaEsc);
    glEnd();

    glutSwapBuffers();
}

// Inicia algumas variáveis de estado
void inicializa() {

    glClearColor(1, 1, 1, 1);      // branco

    iniciarTexturas();

    glEnable(GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    inicializa_Players_e_Bola();
}

void redimensiona(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, LARGURA_DO_MUNDO, 0, ALTURA_DO_MUNDO, -1, 1);

    float razaoAspectoJanela = ((float)width)/height;
    float razaoAspectoMundo = ((float) LARGURA_DO_MUNDO)/ ALTURA_DO_MUNDO;
    if (razaoAspectoJanela < razaoAspectoMundo) {
        float hViewport = width / razaoAspectoMundo;
        float yViewport = (height - hViewport)/2;
        glViewport(0, yViewport, width, hViewport);
    }
    else if (razaoAspectoJanela > razaoAspectoMundo) {
        float wViewport = ((float)height) * razaoAspectoMundo;
        float xViewport = (width - wViewport)/2;
        glViewport(xViewport, 0, wViewport, height);
    } else {
        glViewport(0, 0, width, height);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void teclado(unsigned char key, int x, int y) {
   switch(key) {

      // Tecla ESC
        case 27:
            if(pause==0 && resetar==0 && sair != 1)
                sair++;
            break;
        case 'w':
            player1.movendo=1;
            break;
        case 's':
            player1.movendo=2;
            if(sair==1)
                sair++;
            if(sair==2)
                exit(0);
            break;
        case 'o':
            player2.movendo=1;
            break;
        case 'l':
            player2.movendo=2;
            break;
        case 'p':
            if(pause == 0 && resetar == 0 && sair == 0)
                pause = 1;
            else
                pause = 0;
            break;
        case 'r':
            if(sair == 0 && pause == 0)
                resetar++;
            resetarJogo();
            if(resetar==1)
            {
                resetarJogo();
            }
            break;
        case 'n':
            resetar=0;
            sair=0;
            break;
        default:
            break;
   }
}

void tecladoUp(unsigned char key, int x, int y) {

    switch(key) {

        case 'w':
            player1.movendo=0;
            break;
        case 's':
            player1.movendo=0;
            break;
        case 'o':
            player2.movendo=0;
            break;
        case 'l':
            player2.movendo=0;
            break;
        default:
            break;

    }

}

void movimentoMouse(int x, int y) {



}

void atualizaCena(int periodo) {

    if(pause == 0 && player1.vitoria==0 && player2.vitoria==0 && resetar==0 && sair==0)
    {
        movimentarPlayer();
        movimentarBola();
    }

    //Igualar o y da bola e dos pokemons;
    pokemon1.posicao.y=bola.posicao.y;
    pokemon2.posicao.y=bola.posicao.y;

    checarPontuacao();
    checarVitoria();

    glutPostRedisplay();

    glutTimerFunc(periodo, atualizaCena, periodo);

}

int main(int argc, char **argv) {

    srand(time(0));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 450);

    glutCreateWindow("Ping Phong");
    inicializa();

    glutDisplayFunc(desenhaCena);
    glutReshapeFunc(redimensiona);
    glutKeyboardFunc(teclado);
    glutKeyboardUpFunc(tecladoUp);
    glutPassiveMotionFunc(movimentoMouse);
    glutTimerFunc(0, atualizaCena, 33);

    glutMainLoop();
    return 0;
}
