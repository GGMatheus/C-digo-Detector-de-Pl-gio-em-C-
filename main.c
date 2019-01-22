#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashtable.h"
#include "automata.h"
#include "strutils.h"

#define NKEYWORDS 31
#define MAXCOMPKEYWORD 10

#define NVARIAVEIS 6
#define NCOMANDOS 14
#define NFUNCOES 8

char HTML_CABECALHO[] = "<!DOCTYPE html>\
<html class=\"client-nojs\" lang=\"en\" dir=\"ltr\">\
  <head>\
    <title>EP1 do Matheus</title>\
    <meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>\
    <style type=\"text/css\">\
    secao {\
        display: table;\
        }\
    ladoalado {\
        display:table-cell;\
        vertical-align: top;\
        }\
    .tabelaC { line-height: 11px; margin-left: 50px; }  \
    .corLnh { background-color: #aaaaaa; color: #ffffff; }\
    .corT { color: #00aa00; } \
    .corC { color: #0000ff; } \
    .corF { color: #650065; } \
    .corV { color: #005599; } \
    .corI { color: #cc6600; } \
    .code { font-family Courier; }\
    .tabelaTipos {\
        border: 1px solid #000;\
        font-family: sans-serif;\
        border-collapse: collapse\
        }\
    .tabelaTipos th {border: 1px solid #000; padding: 5px 50px 5px 50px;}\
    .tabelaTipos td {border: 1px solid #000; padding: 3px; margin: 0; border-collapse: collapse;}\
    .tabelaTipos tr {margin: 0;}\
    .tabelaTipos .tabelaCabecalho { background: #9abaea; }\
    .tabelaTipos tr .contadorTd { text-align: right; }\
    </style>\
    </head>\
    <body>\
    <table class=\"tabelaTipos\">\n";

char HTML_TABELA_VAR[] = "    <tr class=\"tabelaCabecalho\">\
      <th colspan=\"2\">Tipos de variaveis</th>\
    </tr>";

char HTML_TABELA_FUNC[] = "    <tr class=\"tabelaCabecalho\">\
      <th colspan=\"2\">Tipos de funcoes de bibliotecas</th>\
    </tr>";

char HTML_TABELA_CMD[] = "    <tr class=\"tabelaCabecalho\">\
      <th colspan=\"2\">Tipos de comandos</th>\
    </tr>";

char HTML_TABELA_FIM[] = "</table>";  

char HTML_RODAPE[] = "</body></html>";
enum tipoKeyword {VARIAVEL, COMANDO, FUNCAO, OUTRO};

char *comandos[NCOMANDOS] = {
    "break",
    "case",
    "continue",
    "default",
    "do",
    "else",
    "for",
    "if",
    "return",
    "sizeof",
    "struct",
    "switch",
    "union",
    "while",
};

char *variaveis[NVARIAVEIS] = {
    "char",
    "int",
    "short",
    "float",
    "long",
    "double",
};

char *funcoes[NFUNCOES] = {
    "main",
    "define",
    "include",
    "void",
    "printf",
    "fprintf",
    "scanf",
    "fscanf",
};

char *keywords[NKEYWORDS] = {
    "*/",
    "/*",
    "//",
    "break",
    "case",
    "char",
    "continue",
    "default",
    "define",
    "do",
    "double",
    "else",
    "float",
    "for", 
    "fprintf",
    "fscanf",
    "if", 
    "include",
    "int",
    "long",
    "main",
    "printf",
    "return",
    "scanf",
    "short",
    "sizeof",
    "struct", 
    "switch",
    "union",
    "void",
    "while",
};

char *enumParaClasseHtml(int tipo) {
    if (tipo == FUNCAO) return "corF";
    if (tipo == VARIAVEL) return "corT";
    if (tipo == COMANDO) return "corC";
    if (tipo == OUTRO) return "corI";
    return "corLnh";
}

void imprime_linha_html(FILE *html, int tipo_kw, char *kw, int count) {
        fprintf(html,
                "<tr>\
                  <td class=\"%s\">%s</td>\
                  <td class=\"contadorTd\">%d</td>\
                </tr>\n",
                enumParaClasseHtml(tipo_kw), kw, count);
}

void imprime_keywords_html(FILE *html, tabelaHash_t *tabelaHash,
                           int tipoKw, char *kws[], int tamKw,
                           char *cabecalho) {
    int i, count;
    char buffer[MAXCOMPKEYWORD];
    fprintf(html, "%s\n", cabecalho);
    for (i = 0; i < tamKw; i++) {
        strcpy(buffer, "");
        char *kw = kws[i];
        strcpy(buffer, kw); 
        count = ht_get(tabelaHash, buffer);

        printf("[%s] = %d\n",buffer, count);
        if (count != -1) imprime_linha_html(html, tipoKw, buffer, count);
    }
}

void imprime_linha_th(linha_t *linha) {
    if (linha != NULL) {
        printf("[%s:%lu] = %d\n",linha->chave, linha->gaveta, linha->valor);
        imprime_linha_th(linha->prox);
    }
}

void imprime_th(tabelaHash_t *th) {
    for(int i = 0; i< th->tamanho; i++) if (th->tabela[i] != NULL) imprime_linha_th(th->tabela[i]);
}

int main(int argc, char *argv[]) {

    char *nomeArqEnt = argv[1];
    char *nomeArqSaida = argv[2];

    char buffer[MAXCOMPKEYWORD] = "", strLimpa[MAXCOMPKEYWORD];
    int pos_buffer = 0, valor;
    FILE *in_file  = fopen(nomeArqEnt, "r"); /* read only */
    char c;
    int flag_ML = 0, flag_1L = 0;

    estado_t *automato = NULL;
    automato = montaAutomato(keywords, NKEYWORDS);

    tabelaHash_t *th = cria_th(1021);

    while(!(feof(in_file) || ferror(in_file))) {

        fscanf(in_file, "%c", &c);

        // checagem de final 1L
        if (c == '\n' && !flag_ML) flag_1L = 0;

        buffer[pos_buffer] = c;
        buffer[pos_buffer + 1] = '\0';

        switch(encontraEstado(automato, buffer, 0)) {

            case INEXISTENTE:
                strcpy(buffer, "");
                pos_buffer = 0;
                break;

            case FINAL:
                // se entrou aqui, buffer contem uma keyword valida
                // incrementa o contador para aquela keyword
              
                /*
                 * comentarios 1L só desativam ao  encontrar \n
                 * comentarios ML não são ativados dentro de comentario 1L ou de comentario ML
                 * comentarios ML, quando ativados, não desativam ao encontrar \n
                 * comentario 1L pode ser precedido de comentário ML, caso ele feche antes da inicialização do mesmo
                 */
                strClear(strLimpa, MAXCOMPKEYWORD);
                strcpy(strLimpa, buffer);
                if(!(flag_ML || flag_1L)) { 
                    valor = ht_get(th, strLimpa);
                    if(valor == -1) ht_set(th, strLimpa, 1);
                    else ht_set(th, strLimpa, valor + 1);

                }

                if(strcmp(buffer, "/*") == 0 && !flag_1L)  flag_ML = 1;
                if(strcmp(buffer, "*/") == 0) flag_ML = 0;     
                if(strcmp(buffer, "//") == 0 && !flag_ML) flag_1L = 1;
                
                // limpa buffer
                strcpy(buffer, "");
                pos_buffer = 0;
                break;
            case INICIAL:
            case INTERMEDIARIO:
                pos_buffer++;
                break;
        }
    }

    
    FILE *out_file  = fopen(nomeArqSaida, "w"); 
    fprintf(out_file, "%s", HTML_CABECALHO);
    imprime_keywords_html(out_file, th, VARIAVEL, variaveis, NVARIAVEIS, HTML_TABELA_VAR); 
    imprime_keywords_html(out_file, th, COMANDO, comandos, NCOMANDOS, HTML_TABELA_CMD); 
    imprime_keywords_html(out_file, th, FUNCAO, funcoes, NFUNCOES, HTML_TABELA_FUNC); 
    imprime_th(th);
    fprintf(out_file, "%s", HTML_TABELA_FIM);
    fprintf(out_file, "%s", HTML_RODAPE);
    fclose(out_file);

    return 0;
}
