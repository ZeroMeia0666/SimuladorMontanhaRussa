# Simulador de Montanha-Russa 2D

Trabalho de Modelagem Computacional.

Programa em C, feito com a biblioteca raylib, que simula um carrinho numa montanha-russa. O jogador digita a massa, a velocidade e a altura iniciais e depois precisa acertar a massa e os freios em alguns pontos do trajeto para o carrinho chegar até o final.

## Como o jogo funciona

No ponto A, o carrinho 1 (massa M, velocidade V) bate num carrinho parado de massa 1,25 M. A colisão é elástica, e quem segue pela pista é o carrinho que estava parado.

Depois da colisão o jogador faz quatro ajustes:

- ponto B: nova massa para o carrinho chegar ao topo C
- trecho C-D: freio (trabalho dissipado, em J) para chegar ao topo E
- ponto F: nova massa para chegar ao topo G
- trecho H-I: força de frenagem (em N) para parar no ponto I

Em cada ajuste o programa mostra um intervalo de 30% para menos e 30% para mais em volta do valor correto. Se o valor digitado errar por até 10%, o programa troca pelo valor certo e o jogo continua. Se errar mais que isso, é fim de jogo: o carrinho não alcança o topo ou decola dos trilhos (no trecho final, para antes do I ou passa dele).

A gravidade usada é g = 10 m/s².

## Física usada

Colisão elástica, com m1 = M e m2 = 1,25 M:

    v1 = (m1 - m2) / (m1 + m2) * V
    v2 = 2 * m1 / (m1 + m2) * V

A energia mecânica do carrinho em A é E = m * v2² / 2 + m * g * H.

Alturas: A = H, C = 5H/4, E = 3H/4, G = H/2, H e I = 3H/8, e B, D e F = 0.

Nos topos a velocidade é zero, então a energia é só potencial. Considerei que a energia se conserva quando a massa é trocada. Assim:

- em B: massa = E / (g * 5H/4)
- em C-D: o freio dissipa m * g * (5H/4 - 3H/4)
- em F: massa = E / (g * H/2), com E = m * g * 3H/4 (a energia que o carrinho tem em E)
- em H-I: força = m * g * (H/2 - 3H/8) / (3H), porque o trecho tem 3H de comprimento

## Como rodar (Windows)

1. Instalar a raylib pelo instalador de raylib.com, deixando em C:\raylib.
2. Abrir C:\raylib\w64devkit\w64devkit.exe.
3. Ir até a pasta do projeto e compilar:

```
gcc main.c -o montanha.exe -lraylib -lgdi32 -lwinmm -lm
```

4. Executar com `./montanha.exe`.

No jogo, digite os números e aperte ENTER para confirmar. Backspace apaga e ESC fecha.

## Exemplo

Com M = 2, V = 9 e H = 10, os valores certos são 2,64 kg, 132 J, 3,96 kg e 1,65 N.

## Simulações

Todas com M = 2, V = 9 e H = 10.

1. Ajustes 2.75, 125, 4.1 e 1.7: todos dentro de 10%, o carrinho chega no ponto I.
2. Ajuste 2.0 no ponto B: massa pequena demais, o carrinho decola no topo C.
3. Ajustes 2.7, 130 e 4.5: a massa em F fica grande demais e o carrinho não alcança o topo G.

## Código

Tudo está no main.c, que tem uma parte com as fórmulas de física e outra com a interface (janela, desenho, digitação e validação).