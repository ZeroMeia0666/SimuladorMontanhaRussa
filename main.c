#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"

#define G            10.0   /* gravidade (m/s^2) */
#define FATOR_MASSA  1.25   /* o carrinho em repouso tem 25% mais massa */
#define FAIXA        0.30   /* intervalo mostrado ao jogador: +-30% */
#define TOLERANCIA   0.10   /* erro aceito: +-10% */

/* 
 * PARTE 1 - FISICA - Arthur
 *
 * Alturas: A = H | B = D = F = 0 | C = 5H/4 | E = 3H/4 | G = H/2 | H = I = 3H/8
 * Distancia do trecho H-I: 3H

/* Colisao elastica em A: carrinho 1 (M, V) contra carrinho 2 (1,25M, parado). */
void colisao_elastica(double M, double V, double *m2, double *v1_depois, double *v2_depois)
{
    *m2 = FATOR_MASSA * M;
    *v1_depois = (M - *m2) / (M + *m2) * V;
    *v2_depois = (2.0 * M) / (M + *m2) * V;
}

/* Massa exata para chegar a um topo (altura h_topo) com velocidade nula:
 * energia mecanica total (conservada) = m * g * h_topo */
double massa_para_topo(double energia, double h_topo)
{
    return energia / (G * h_topo);
}

/* Trecho C-D: trabalho dissipado para chegar a D com a energia do ponto E */
double trabalho_freio_CD(double massa, double H)
{
    return massa * G * (5.0 * H / 4.0 - 3.0 * H / 4.0);
}

/* Trecho H-I: energia cinetica em H e forca de frenagem em D_HI = 3H */
double energia_cinetica_H(double massa, double H)
{
    return massa * G * (H / 2.0 - 3.0 * H / 8.0);
}

double forca_freio_HI(double massa, double H)
{
    return energia_cinetica_H(massa, H) / (3.0 * H);
}

// parte 2
#define LARGURA  1200
#define ALTURA   700
#define N_PONTOS 9               

static const float PX[N_PONTOS]     = { 110, 250, 400, 555, 700, 830, 950, 1000, 1090 };
static const float FATOR[N_PONTOS]  = { 1.0f, 0.0f, 1.25f, 0.0f, 0.75f, 0.0f, 0.5f, 0.375f, 0.375f };
static const char  NOMES[N_PONTOS]  = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I' };

static const char *TITULO = "";
static char L1[200], L2[200], L3[200];
static const char *RODAPE = "ENTER confirma  |  ESC sai";


static float carro_x, carro_y;
static float carro_angulo = 0.0f;
static void desenhar_carrinho(void);
static float ponto_y(int k)
{
    return 440.0f - FATOR[k] * 200.0f;
}

/* Calcula a posicao sobre a curva de Bezier */
static Vector2 posicao_curva(int trecho, float t)
{
    Vector2 p0 = { PX[trecho], ponto_y(trecho) };
    Vector2 p3 = { PX[trecho + 1], ponto_y(trecho + 1) };

    float dx = p3.x - p0.x;

    Vector2 p1 = { p0.x + dx * 0.5f, p0.y };
    Vector2 p2 = { p3.x - dx * 0.5f, p3.y };

    float u = 1.0f - t;

    Vector2 resultado;

    resultado.x =
        u*u*u*p0.x +
        3*u*u*t*p1.x +
        3*u*t*t*p2.x +
        t*t*t*p3.x;

    resultado.y =
        u*u*u*p0.y +
        3*u*u*t*p1.y +
        3*u*t*t*p2.y +
        t*t*t*p3.y;

    return resultado;
}

/* Calcula a inclinacao da pista em graus */
static float angulo_curva(int trecho, float t)
{
    float antes = t - 0.001f;
    float depois = t + 0.001f;

    if (antes < 0.0f) antes = 0.0f;
    if (depois > 1.0f) depois = 1.0f;

    Vector2 p1 = posicao_curva(trecho, antes);
    Vector2 p2 = posicao_curva(trecho, depois);

    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;

    return atan2f(dy, dx) * RAD2DEG;
}


static void desenhar_pista(void)
{
    DrawLineEx(
        (Vector2){30, ponto_y(0)},
        (Vector2){PX[0], ponto_y(0)},
        5,
        DARKGRAY
    );

    for (int k = 0; k < N_PONTOS - 1; k++)
    {
        Color cor = (k == 2 || k == 7)
                    ? ORANGE : DARKGRAY;

        Vector2 anterior = posicao_curva(k, 0.0f);

        for (int i = 1; i <= 80; i++)
        {
            float t = (float)i / 80.0f;

            Vector2 atual = posicao_curva(k, t);

            DrawLineEx(anterior, atual, 5, cor);

            anterior = atual;
        }
    }
}


static void desenhar_cena(const char *rotulo, const char *digitado)
{
    int k, bx;
    char nome[2] = { 0, 0 };

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText("Simulador de Montanha-Russa 2D", 30, 14, 28, BLACK);

    desenhar_pista();

    
    DrawText("FREIO", 505, 290, 16, ORANGE);
    DrawText("FREIO", 1030, 385, 16, ORANGE);

   desenhar_carrinho();

    DrawRectangle(20, 480, LARGURA - 40, 205, (Color){ 240, 244, 250, 255 });
    DrawRectangleLines(20, 480, LARGURA - 40, 205, DARKGRAY);
    DrawText(TITULO, 40, 494, 24, DARKBLUE);
    DrawText(L1, 40, 532, 20, BLACK);
    DrawText(L2, 40, 560, 20, BLACK);
    DrawText(L3, 40, 588, 20, BLACK);
    DrawText(RODAPE, 40, 657, 16, GRAY);

    if (rotulo) {
        bx = 40 + MeasureText(rotulo, 20) + 16;
        DrawText(rotulo, 40, 626, 20, BLACK);
        DrawRectangle(bx, 618, 200, 36, WHITE);
        DrawRectangleLines(bx, 618, 200, 36, DARKBLUE);
        DrawText(digitado, bx + 10, 625, 22, BLACK);
    }

    EndDrawing();
}


static Vector2 girar_ponto(Vector2 p, float angulo)
{
    float rad = angulo * DEG2RAD;

    Vector2 resultado = {
        p.x * cosf(rad) - p.y * sinf(rad),
        p.x * sinf(rad) + p.y * cosf(rad)
    };

    return resultado;
}

static void desenhar_carrinho(void)
{
    Vector2 centro = {carro_x, carro_y - 17};

    Rectangle corpo = {
        centro.x,
        centro.y,
        38,
        20
    };

    DrawRectanglePro(
        corpo,
        (Vector2){19, 10},
        carro_angulo,
        RED
    );

    Vector2 roda1 = girar_ponto(
        (Vector2){-12, 10},
        carro_angulo
    );

    Vector2 roda2 = girar_ponto(
        (Vector2){12, 10},
        carro_angulo
    );

    roda1.x += centro.x;
    roda1.y += centro.y;

    roda2.x += centro.x;
    roda2.y += centro.y;

    DrawCircleV(roda1, 5, BLACK);
    DrawCircleV(roda2, 5, BLACK);
}


static void checar_janela(void)
{
    if (WindowShouldClose()) {
        CloseWindow();
        exit(0);
    }
}

static double pedir_valor(const char *rotulo)
{
    char texto[24] = "";
    int n = 0, k;

    while (1) {
        desenhar_cena(rotulo, texto);
        checar_janela();

        while ((k = GetCharPressed()) > 0) {
            if (k == ',') k = '.';
            if (n < 18 && ((k >= '0' && k <= '9') || (k == '.' && strchr(texto, '.') == NULL))) {
                texto[n++] = (char)k;
                texto[n] = '\0';
            }
        }
        if (IsKeyPressed(KEY_BACKSPACE) && n > 0)
            texto[--n] = '\0';
        if (IsKeyPressed(KEY_ENTER) && atof(texto) > 0.0)
            return atof(texto);
    }
}

static void esperar_enter(void)
{
    while (1) {
        desenhar_cena(NULL, NULL);
        checar_janela();
        if (IsKeyPressed(KEY_ENTER)) return;
    }
}


static void mover(int de, int para)
{
    float t = 0.0f;

    TITULO = "Carrinho em movimento";

    while (t < 1.0f)
    {
        t += GetFrameTime() / 1.5f;

        if (t > 1.0f)
            t = 1.0f;

        Vector2 pos = posicao_curva(de, t);

        carro_x = pos.x;
        carro_y = pos.y;

        carro_angulo = angulo_curva(de, t);

        desenhar_cena(NULL, NULL);
        checar_janela();
    }
}


static double ajuste(const char *titulo, const char *grandeza, const char *unidade,
                     double exato, const char *motivo_maior, const char *motivo_menor)
{
    double minimo = (1.0 - FAIXA) * exato;
    double maximo = (1.0 + FAIXA) * exato;
    double digitado, erro;
    char rotulo[100];

    TITULO = titulo; 
    snprintf(L1, sizeof L1, "Valores possiveis: de %.3f a %.3f %s", minimo, maximo, unidade);
    snprintf(L2, sizeof L2, "Dentro de %.0f%% do valor correto o jogo ajusta para o valor exato; fora disso, fim de jogo.",
             TOLERANCIA * 100.0);
    L3[0] = '\0';
    snprintf(rotulo, sizeof rotulo, "%s (%s):", grandeza, unidade);

    digitado = pedir_valor(rotulo);
    erro = fabs(digitado - exato) / exato;

    if (erro <= TOLERANCIA) {
        snprintf(L1, sizeof L1, "Valor aceito (erro de %.1f%%). Ajustado para o valor correto: %.3f %s",
                 erro * 100.0, exato, unidade);
        L2[0] = '\0';
        return exato;
    }

    TITULO = "Fim de jogo!";
    snprintf(L1, sizeof L1, "%s", digitado > exato ? motivo_maior : motivo_menor);
    snprintf(L2, sizeof L2, "Valor digitado: %.3f   |   Valor correto: %.3f   |   Erro: %.1f%%",
             digitado, exato, erro * 100.0);
    RODAPE = "Pressione ENTER para sair";
    esperar_enter();
    CloseWindow();
    exit(0);
}


int main(void)
{
    double M, V, H;
    double m2, v1_depois, v2_depois;
    double massa, energia, exato;

    InitWindow(LARGURA, ALTURA, "Montanha-Russa 2D");
    SetTargetFPS(60);

    carro_x = PX[0];
    carro_y = ponto_y(0);

    TITULO = "Parametros iniciais";
    snprintf(L1, sizeof L1, "g = %.0f m/s^2 (fixo). O carrinho 2 tem massa 1,25 M e esta parado em A.", G);
    M = pedir_valor("Massa M do carrinho em movimento (kg):");
    snprintf(L2, sizeof L2, "M = %.3f kg", M);
    V = pedir_valor("Velocidade V do carrinho em movimento (m/s):");
    snprintf(L2, sizeof L2, "M = %.3f kg   |   V = %.3f m/s", M, V);
    H = pedir_valor("Altura de referencia H (m):");

    colisao_elastica(M, V, &m2, &v1_depois, &v2_depois);
    massa = m2;
    energia = 0.5 * massa * v2_depois * v2_depois + massa * G * H;

    TITULO = "Colisao elastica no ponto A";
    snprintf(L1, sizeof L1, "Carrinho 1 (M = %.3f kg) volta com v = %.3f m/s.", M, v1_depois);
    snprintf(L2, sizeof L2, "Carrinho 2 (%.3f kg) segue pela pista com v = %.3f m/s.", m2, v2_depois);
    snprintf(L3, sizeof L3, "Energia mecanica em A = %.3f J.", energia);
    RODAPE = "Pressione ENTER para soltar o carrinho  |  ESC sai";
    esperar_enter();
    RODAPE = "ENTER confirma  |  ESC sai";

    mover(0, 1);

    exato = massa_para_topo(energia, 5.0 * H / 4.0);
    massa = ajuste("Ponto B: ajuste de massa para chegar ao topo C", "Nova massa", "kg", exato,
                   "Massa grande demais: o carrinho nao conseguiu alcancar o topo C.",
                   "Massa pequena demais: o carrinho decolou no topo C.");
    mover(1, 2);

    exato = trabalho_freio_CD(massa, H);
    ajuste("Trecho C-D: freio para chegar ao topo E", "Trabalho dissipado pelo freio", "J", exato,
           "Freio forte demais: o carrinho nao conseguiu alcancar o topo E.",
           "Freio fraco demais: o carrinho decolou no topo E.");
    energia = massa * G * (3.0 * H / 4.0);     
    mover(2, 3);
    mover(3, 4);
    mover(4, 5);

    exato = massa_para_topo(energia, H / 2.0);
    massa = ajuste("Ponto F: ajuste de massa para chegar ao topo G", "Nova massa", "kg", exato,
                   "Massa grande demais: o carrinho nao conseguiu alcancar o topo G.",
                   "Massa pequena demais: o carrinho decolou no topo G.");
    mover(5, 6);
    mover(6, 7);

    exato = forca_freio_HI(massa, H);
    ajuste("Trecho H-I: freio final", "Forca de frenagem", "N", exato,
           "Freio forte demais: o carrinho parou antes do ponto I.",
           "Freio fraco demais: o carrinho passou do ponto I e saiu dos trilhos.");
    mover(7, 8);

    TITULO = "PARABENS!";
    snprintf(L1, sizeof L1, "O carrinho parou exatamente no ponto I.");
    snprintf(L2, sizeof L2, "Voce completou a montanha russa mantendo o carrinho nos trilhos!");
    L3[0] = '\0';
    RODAPE = "Pressione ENTER para sair";
    esperar_enter();

    CloseWindow();
    return 0;
}