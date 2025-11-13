# missao-estrategica_war
Projeto em C implementando a funcionalidade de missões estratégicas individuais, com alocação dinâmica das missões, sorteio automático, verificação após cada turno e liberação de memória, mantendo a modularização e o uso de ponteiros e gerenciamento de memória.
Principais escolhas de implementação (resumido):

Dois jogadores (facilita a lógica de comparação) — você pode estender para mais jogadores facilmente.

Vetor com 5 missões pré-definidas; a verificação usa uma lógica simples que reconhece palavras-chave em cada missão.

atribuirMissao(char* destino, char* missoes[], int totalMissoes) faz o sorteio e copia a missão para a área de memória já alocada.

verificarMissao(char* missao, Territorio* mapa, int tamanho, const char* corJogador) verifica a missão para um jogador (recebe a cor do jogador para checar propriedade). (A assinatura foi levemente ampliada para incluir a cor do jogador — necessário para avaliar condições do jogador.)

exibirMissao(const char* missao) mostra a missão por valor (apenas uma vez).

Todas as missões e o vetor de territórios são liberados ao final.


Observações

A verificação de missões está baseada em busca de palavras-chave na string da missão — isso facilita criar diferentes missões, e torna as descrições padronizadas para que o verificarMissao consiga identificá-las.

Autor

Cesar Ramos de Andrade

Este projeto faz parte de atividades do Curso Analise e Desenvolvimento de Sistemas.
