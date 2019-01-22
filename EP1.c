/***************************************************************/
/**                                                           **/
/**   Matheus Guilherme Gonçalves       Número USP: 9345126   **/
/**   Exercício-Programa 01                                   **/
/**   Professor: Leônidas de Oliveira Brandão                 **/
/**   Turma: 50                                               **/
/**                                                           **/
/***************************************************************/

/* Descricao sobre o EP:
	A resolução do EP pode ser dividida em 3 partes principais, a criação de uma tabela de espalhamento(hash), responsável
	pelo espalhamento e contabilização das palavras reservadas do enunciado, funções chamadas genericamente de strutils, que são 
	operações feitas com strings necessárias para a implementação do código e a função main, que recebe um arquivo .c do usuário, 
    processa esse arquivo num autômato de estados finito que tokeniza o código (segmenta o código recebido em pedaços de string) 
    e classifica adequadamente esses tokens de acordo com as palavras reservadas que queremos contabilizar (comandos, funções 
	especiais, variáveis e comentários). Quando um novo token é formado, é feita uma busca binária pelo tipo de informação 
	que está sendo guardada nesse token, e se a palavra contida nele for uma palavra reservada, ela é contabilizada para ser
	mostrada posteriormente na saída da nossa main, que será um arquivo .html na forma de uma tabela que contabiliza comandos, 
	funções, variáveis e comentários, além de mostrar também o nome de todas as variáveis declaradas no programa, que é obtido 
	através de outro autômato que salva o nome das variáveis numa lista ligada.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAPSIZE 27

#define TRUE 1
#define FALSE 0

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

char HTML_TABELA_TIPO[] = "    <tr class=\"tabelaCabecalho\">\
      <th colspan=\"2\">Tipos de variaveis</th></tr>\n";

char HTML_TABELA_FUNC[] = "    <tr class=\"tabelaCabecalho\">\
      <th colspan=\"2\">Tipos de funcoes de bibliotecas</th></tr>\n";

char HTML_TABELA_CMD[] = "    <tr class=\"tabelaCabecalho\">\
      <th colspan=\"2\">Tipos de comandos</th></tr>\n";

char HTML_TABELA_VAR[] = "    <tr class=\"tabelaCabecalho\">\
      <th colspan=\"2\">Variáveis</th></tr>\n";

char HTML_TABELA_COM[] = "    <tr class=\"tabelaCabecalho\">\
      <th colspan=\"2\">Comentários</th></tr>\n";

char HTML_TABELA_FIM[] = "</table>";  

char HTML_RODAPE[] = "</body></html>";

enum tipoToken {
    PONTO_VIRGULA, VIRGULA, PAREN_ESQ, PAREN_DIR, IGUAL, NAO_IGUAL, LITERAL_S,
    LITERAL_C, DESCONHECIDO, FUNCAO, COMANDO, VARIAVEL, OUTRO, KEYWORD, OPERADOR,
    NOVA_LINHA, CHAVE_ESQ, CHAVE_DIR, OPERADOR_LOGICO, NUMERO, COMENTARIO_1L, 
    COMENTARIO_ML, ASPAS_DUPLAS, ASPAS_SIMPLES, TIPO_VARIAVEL, 
    VAR_INT, VAR_FLOAT, VAR_SHORT, VAR_DOUBLE, VAR_LONG, VAR_CHAR, VAR_DESCONHECIDO,
    TIPO_INT, TIPO_FLOAT, TIPO_SHORT, TIPO_DOUBLE, TIPO_LONG, TIPO_CHAR, TIPO_DESCONHECIDO};

struct Keyword_s {
    char *str;
    enum tipoToken tipo;
};

typedef struct Keyword_s Keyword;

Keyword map[] = {
    "break", COMANDO,
    "case", COMANDO,
    "char", TIPO_CHAR,
    "continue", COMANDO,
    "default", COMANDO,
    "define", FUNCAO,
    "do", COMANDO,
    "double", TIPO_DOUBLE,
    "else", COMANDO,
    "float", TIPO_FLOAT,
    "for", COMANDO,
    "fprintf", FUNCAO,
    "fscanf", FUNCAO,
    "if", COMANDO,
    "include", FUNCAO,
    "int", TIPO_INT,
    "long", TIPO_LONG,
    "main", FUNCAO,
    "printf", FUNCAO,
    "return", COMANDO,
    "scanf", FUNCAO,
    "short", TIPO_SHORT,
    "sizeof", COMANDO,
    "struct", COMANDO,
    "switch", COMANDO,
    "union", COMANDO,
    "void", FUNCAO,
    "while", COMANDO
};

int buscaTipoKeyword(Keyword *vet, char *kw, int inicio, int fim) {

    int meio = inicio + (fim - inicio) / 2;

    if (inicio > fim) return KEYWORD;
    if (strcmp(vet[meio].str, kw) == 0) return vet[meio].tipo;
    if (strcmp(vet[meio].str, kw) >  0)
        return buscaTipoKeyword(vet, kw, inicio, meio - 1);
    return buscaTipoKeyword(vet, kw, meio + 1, fim);
        
}

//strutils
char **particionaString(char *str) {
    int i, j;
    int tamanho = strlen(str);
    char **ret, *aux;    
    ret =  malloc(tamanho * sizeof(char*));

    for(i = 0; i < tamanho; i++) {
        aux = malloc((i + 1) * sizeof(char));
        for(j = 0; j <= i; j++) {
            aux[j] = str[j]; 
        }    
        aux[j] ='\0';
        ret[i] = aux;
    } 

    return ret;
}

int strComecaCom(char *palheiro, char *agulha) {

    if (!*agulha) {
        /* Se agulha é \0 ou NULL, retorne verdadeiro. */
        return 1;
    }
  
    char *p1 = palheiro;
    char *p2 = agulha;
    while (*p1 && *p2 && *p1 == *p2) {
        /* Vá para a próxima posição nos dois e execute a mesma comparação. O loop
            termina caso qualquer uma das posições subsequentes não seja igual ou
            cheguemos ao final de quaisquer vetores. */
        p1++;
        p2++;
    }
    if (!*p2) {
        /* O vetor agulha acabou (chegou em \0). Isto significa que p1Comeco
            aponta para a primeira ocorrencia do inicio da substring. */
        return 1;
    }
    /* Se não, palheiro nao começa com agulha*/
    return 0;
}

void imprimeVetorString(char **vetString, int tamanho) {
    int i;
    for(i = 0; i < tamanho; i++) {
        printf("[%d] -> %s\n", i, vetString[i]);
    }
}

char *strdupNosso(char *s) {
    char *d = malloc(strlen(s) + 1);
    strcpy(d, s);
    return d;
}


void strClear(char *s, int size) {
    for (int i = 0; i < size; i++) s[i] = '\0';
}

struct linha_s {
    char *chave;
    int valor;
    unsigned long int gaveta;
    struct linha_s *prox;
};

typedef struct linha_s linha_t;

struct tabelaHash_s {
    int tamanho;
    struct linha_s **tabela;    
};

typedef struct tabelaHash_s tabelaHash_t;

/* Cria uma nova tabela hash. */
tabelaHash_t *cria_th( int tamanho ) {

    tabelaHash_t *tabelaHash = NULL;
    int i;

    if( tamanho < 1 ) return NULL;

    /* Aloca a tabela. */
    if( ( tabelaHash = malloc( sizeof( tabelaHash_t ) ) ) == NULL ) {
        return NULL;
    }

    /* Aloca os ponteiros aos nos de referencia. */
    if( ( tabelaHash->tabela = malloc( sizeof( linha_t * ) * tamanho ) ) == NULL ) {
        return NULL;
    }
    for( i = 0; i < tamanho; i++ ) {
        tabelaHash->tabela[i] = NULL;
    }

    tabelaHash->tamanho = tamanho;

    return tabelaHash;    
}

/* Espalha(hash) a string para uma tabelaHash em particular. */
int ht_hash( tabelaHash_t *tabelaHash, char *chave ) {

    unsigned long int hashval = 0;
    int i = 0;

    /* Converte nossa string num inteiro */
    while(i < strlen( chave ) ) {
        // 00000000 00000000 00000000 00001111
        // 00000000 00000000 00001111 00000000
        hashval = hashval << 8;
        hashval += chave[i] & 0b0000000011111111;
        i++;
    }
    return hashval % tabelaHash->tamanho;
}

/* Cria um par chave-valor. */
linha_t *novopar_th( char *chave, int valor ) {
    linha_t *novopar;

    if( ( novopar = malloc( sizeof( linha_t ) ) ) == NULL ) {
        return NULL;
    }

    if( ( novopar->chave = strdupNosso( chave ) ) == NULL ) {
        return NULL;
    }

    novopar->valor = valor;
        
    novopar->prox = NULL;

    return novopar;
}

/* Insere um par chave-valor numa tabelaHash. */
void ht_set( tabelaHash_t *tabelaHash, char *chave, int valor ) {
    int gaveta = 0;
    linha_t *novopar = NULL;
    linha_t *prox = NULL;
    linha_t *ultima = NULL;

    gaveta = ht_hash( tabelaHash, chave );


    prox = tabelaHash->tabela[ gaveta ];

    while( prox != NULL && prox->chave != NULL && strcmp( chave, prox->chave ) > 0 ) {
        ultima = prox;
        prox = prox->prox;
    }

    /* Ja temos um par. Vamos substituir essa string. */
    if( prox != NULL && prox->chave != NULL && strcmp( chave, prox->chave ) == 0 ) {

        prox->valor = valor;

    /* Nao achamos um par. Hora de criar um. */
    } else {
        novopar = novopar_th( chave, valor );

        /* Estamos no comeco da lista ligada nessa gaveta. */
        if( prox == tabelaHash->tabela[ gaveta ] ) {
            novopar->prox = prox;
            tabelaHash->tabela[ gaveta ] = novopar;
    
        /* Estamos no fim da lista ligada nessa gaveta. */
        } else if ( prox == NULL ) {
            ultima->prox = novopar;
    
        /* Estamos no meio da lista. */
        } else  {
            novopar->prox = prox;
            ultima->prox = novopar;
        }
    }
}

/* Receba o par chave-valor de uma tabelaHash. */
int ht_get( tabelaHash_t *tabelaHash, char *chave ) {
    int gaveta = 0;
    linha_t *par;

    gaveta = ht_hash( tabelaHash, chave );

    /* Percorra as gavetas, procurando por nosso valor. */
    par = tabelaHash->tabela[ gaveta ];
    while( par != NULL && par->chave != NULL && strcmp( chave, par->chave ) > 0 ) {
        par = par->prox;
    }

    /* Achamos alguma coisa? */
    if( par == NULL || par->chave == NULL || strcmp( chave, par->chave ) != 0 ) {
        return -1;

    } else {
        return par->valor;
    }
    
}

struct Token_s {
    char *valor;
    enum tipoToken tipo;
    struct Token_s *prox;
};

typedef struct Token_s Token_t;

int isLetra(char c) {
    if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return TRUE;
    return FALSE;
}

int isNumero(char c) {
    if(c >= '0' && c <= '9') return TRUE;
    else return FALSE;
}

char *charToString(char c, char *buffer) {
    
    buffer[0] = c;
    buffer[1] = '\0';
    return buffer;
}

char getChar(FILE *fp) {
    char aux;
    fscanf(fp, "%c", &aux);
    return aux;
}

Token_t *criaToken(char *valor, int tipo, Token_t *prox) {
    Token_t *ret;
    ret = malloc(sizeof(Token_t));

    ret->valor = strdupNosso(valor);
    ret->tipo = tipo;
    ret->prox = prox;

    return ret;
}

Token_t *insereToken(Token_t *lista, Token_t *novo) {
    Token_t *aux = lista;
    if (lista == NULL) {
        lista = novo;
    }
    else {
        while(aux->prox != NULL) {
            aux = aux->prox;
        }
        aux->prox = novo;
    }

    return lista;
}

void imprimeListaDeToken(FILE *html, Token_t* lista, int tipo) {
    Token_t *aux = lista;
    fprintf(html, "<pre> tipo: %d\n", tipo);
    while(aux != NULL) {
        if (aux->tipo == tipo) fprintf(html, "[%s]", aux->valor, aux->tipo);
        aux = aux->prox;
    }
    fprintf(html, "</pre>\n");
}

void imprimeTodosTokens(FILE *html, Token_t* lista) {
    Token_t *aux = lista;
    fprintf(html, "<pre>");
    while(aux != NULL) {
        if (aux->tipo == NOVA_LINHA) fprintf(html, "\n");
        else fprintf(html, "[%s]", aux->valor);
        aux = aux->prox;
    }
    fprintf(html, "</pre>\n");
}

int contaTipoToken(Token_t* lista, int tipo) {
    if (lista == NULL) return 0;
    else return (lista->tipo == tipo? 1: 0) + contaTipoToken(lista->prox, tipo);
}

void imprimeTokensHTML(FILE *html, Token_t* lista, char *nome_tipo, int tipo) {
    Token_t *aux = lista;
    int i = 0, n = contaTipoToken(lista, tipo);
    if ( n > 0) {
        fprintf(html, "<tr><td class=\"corT\">%s</td><td class=\"corV\">", nome_tipo);
        while(aux != NULL) {
            if (aux->tipo == tipo) {
                i++;
                fprintf(html, "%s", aux->valor, aux->tipo); 
                if (i < n) fprintf(html, ", ");
            }
            aux = aux->prox;
        }
        fprintf(html, "</td></tr>\n");    
    }
}

void imprimeContadoresHTML(
    FILE *html, Keyword* kws, tabelaHash_t *th, char *cabecalho,
    char *classe, int tipo_low, int tipo_high) {

    int i, count;

    fprintf(html, cabecalho);

    for (i = 0; i < MAPSIZE; i++) {
        if (kws[i].tipo >= tipo_low && kws[i].tipo <= tipo_high) {
            count = ht_get(th, kws[i].str);
            if (count != -1)
                fprintf(html, "<tr><td class=\"%s\">%s</td><td\
                               class=\"contadorTd\">%d</td></tr>\n",
                        classe, kws[i].str, count);
        }
    }
}

void imprimeComentariosHTML(FILE *html, tabelaHash_t *th, char *cabecalho) {
    fprintf(html, cabecalho);
    fprintf(html, "<tr><td class=\"corV\">Uma linha</td><td\
                   class=\"contadorTd\">%d</td></tr>\n",
            ht_get(th, "//"));
    fprintf(html, "<tr><td class=\"corV\">Multi linha</td><td\
                   class=\"contadorTd\">%d</td></tr>\n",
            ht_get(th, "*/"));
}

int isEOF(FILE *fp){
    return (feof(fp) || ferror(fp));
}

int isTipoVariavel(int tipo) {
    return (tipo >= TIPO_INT && tipo <= TIPO_DESCONHECIDO)? TRUE: FALSE;
}

void removeComentario(Token_t *lista) {
    Token_t *aux = lista;

    while(aux != NULL) {
        if(aux->tipo == COMENTARIO_1L || aux->tipo == COMENTARIO_ML) {
            strcpy(aux->valor, "");
        }
        aux = aux->prox;
    }
}

void trocaNomeVariavel(Token_t *lista) {
    Token_t *aux = lista, *prox;
    int tipo_encontrado = TIPO_DESCONHECIDO;

    while(aux != NULL) {
        if(isTipoVariavel(aux->tipo)) {
            if (aux->prox != NULL) prox = aux->prox;
            if(prox->prox != NULL && prox->prox->tipo == PAREN_ESQ) {
                // é funcao
                aux = prox->prox->prox;
                continue;
            }
        }
        switch(aux->tipo) {
            case VAR_INT:
                strcpy(aux->valor, "vi");
                break;
            case VAR_FLOAT:
                strcpy(aux->valor, "vf");
                break;
            case VAR_SHORT:
                strcpy(aux->valor, "vs");
                break;
            case VAR_DOUBLE:
                strcpy(aux->valor, "vd");
                break;
            case VAR_LONG:
                strcpy(aux->valor, "vl");
                break;
            case VAR_CHAR:
                strcpy(aux->valor, "vc");
                break; 

        }
        if(prox->prox->tipo == VIRGULA) {
                
        }
        aux = aux->prox;
    }
}

// void trocaForPorWhile(Token_t *lista) {
    
// }

void marcaVariaveis(Token_t* lista) {
    Token_t *aux = lista, *ret, *alvo;
    Token_t *prox, *proxprox;

    int i = 0, tipo_encontrado = TIPO_DESCONHECIDO;
    while (aux != NULL) {
        if (isTipoVariavel(aux->tipo)) {
            if (aux->prox != NULL) prox = aux->prox;
            if(prox->prox != NULL && prox->prox->tipo == PAREN_ESQ) {
                // é funcao
                aux = prox->prox->prox;
                continue;
            }

            switch (aux->tipo) {
                case TIPO_INT:
                    tipo_encontrado = VAR_INT;
                    break;
                case TIPO_FLOAT:
                    tipo_encontrado = VAR_FLOAT;
                    break;
                case TIPO_SHORT:
                    tipo_encontrado = VAR_SHORT;
                    break;
                case TIPO_DOUBLE:
                    tipo_encontrado = VAR_DOUBLE;
                    break;
                case TIPO_LONG:
                    tipo_encontrado = VAR_LONG;
                    break;
                case TIPO_CHAR:
                    tipo_encontrado =  VAR_CHAR;
                    break;
                default:
                    tipo_encontrado = VAR_DESCONHECIDO;
            }
            prox->tipo = tipo_encontrado;
            aux = prox->prox;
        }
        if (aux->tipo == VIRGULA) {
            if (aux->prox != NULL) prox = aux->prox;
            if (prox->tipo == KEYWORD) prox->tipo = tipo_encontrado;
        }
        if (aux->tipo == PONTO_VIRGULA) {
            tipo_encontrado = TIPO_DESCONHECIDO;
        }
        aux = aux->prox;
    }
}

int main(int argc, char *argv[]) {

    char *nomeArqEnt = argv[1];
    char *nomeArqSaida = argv[2];
    char buffer[2048];

    Token_t *lista = NULL;

    FILE *in_file  = fopen(nomeArqEnt, "r"); /* read only */
    char c, d;
    int flag_ML = 0, flag_1L = 0, i, j, tipoKw, count;

    tabelaHash_t *th = cria_th(1021);

    while(!isEOF(in_file)) {

        c = getChar(in_file);
        strcpy(buffer, "");
        
        if (c == '#') {
            i = 0;
            lista = insereToken(lista, criaToken("#", DESCONHECIDO, NULL));
            do {
                c = getChar(in_file);
                buffer[i] = c;
                i++;
            } while(isLetra(c) && !isEOF(in_file));
            buffer[i - 1] = '\0';
            lista = insereToken(lista, criaToken(buffer, FUNCAO, NULL));
            count = ht_get(th, buffer);
            if (count == -1) ht_set(th, buffer, 1);
            else ht_set(th, buffer, count + 1); 
            strcpy(buffer, "");
            buffer[0] = c;
            i = 1;
            do {
                c = getChar(in_file);
                buffer[i] = c;
                i++;
            } while(c != '\n' && !isEOF(in_file));
            buffer[i - 1] = '\0';
            
            lista = insereToken(lista, criaToken(buffer, DESCONHECIDO, NULL));
            lista = insereToken(lista, criaToken("\n", NOVA_LINHA, NULL));
        }
        else if (c == '(') {
            lista = insereToken(lista, criaToken(charToString(c, buffer), PAREN_ESQ, NULL));
        }
        else if (c == ')') {
            lista = insereToken(lista, criaToken(charToString(c, buffer), PAREN_DIR, NULL));
        }
        else if (c == '{') {
            lista = insereToken(lista, criaToken(charToString(c, buffer), CHAVE_ESQ, NULL));
        }
        else if (c == '}') {
            lista = insereToken(lista, criaToken(charToString(c, buffer), CHAVE_DIR, NULL));
        }
        else if (c == ';') {
            lista = insereToken(lista, criaToken(charToString(c, buffer), PONTO_VIRGULA, NULL));
        }
        else if (c == ',') {
            lista = insereToken(lista, criaToken(charToString(c, buffer), VIRGULA, NULL));
        }
        else if (isLetra(c) || c == '*') {
            // palavras, fecha comentario multi linha, multiplicacao
            strcpy(buffer, "");
            i = 0;

            j = i;
            while((c == '*' || c =='&') && !isEOF(in_file)) {
                buffer[i] = c;
                c = getChar(in_file);
                i++;   
            }

            j = i;
            while((isLetra(c) || isNumero(c) || c == '_' || c == '[' || c == ']') && !isEOF(in_file)) {
                buffer[i] = c;
                c = getChar(in_file);
                i++;
            }
            if (i != j) fseek(in_file, -sizeof(char), SEEK_CUR); // rebobina 1 char

            j = i;
            while((c == '*' || c =='&') && !isEOF(in_file)) {
                buffer[i] = c;
                c = getChar(in_file);
                i++;   
            }
            if (i != j) fseek(in_file, -sizeof(char), SEEK_CUR); // rebobina 1 char

            buffer[i] = '\0';

            tipoKw = buscaTipoKeyword(map, buffer, 0, MAPSIZE);

            if (tipoKw == COMANDO || tipoKw == FUNCAO || isTipoVariavel(tipoKw)) {
                count = ht_get(th, buffer);
                if (count != -1) ht_set(th, buffer, count + 1);
                else ht_set(th, buffer, 1);
            }
            lista = insereToken(lista, criaToken(buffer, tipoKw, NULL));
        }
        else if (isNumero(c)) {
            strcpy(buffer, "");
            i = 0;

            j = i;
            while((isNumero(c) || c == 'b' || c == 'x' || c == '.') && !isEOF(in_file)) {
                buffer[i] = c;
                c = getChar(in_file);
                i++;
            }
            if (i != j) fseek(in_file, -sizeof(char), SEEK_CUR); // rebobina 1 char
            buffer[i] = '\0';
            lista = insereToken(lista,criaToken(buffer, NUMERO, NULL));
            // le enquanto for numero, 'b', 'x' e '.'
        }
        else if (c == '\n') {
            lista = insereToken(lista, criaToken(charToString(c, buffer), NOVA_LINHA, NULL));
        }
        else if (c == '&') {
            strcpy(buffer, "");
            i = 0;
            while((isLetra(c) || isNumero(c) || c == '_' || c == '&') && !isEOF(in_file)) {
                buffer[i] = c;
                c = getChar(in_file);
                i++;
                if(c == '&') {
                    buffer[i] = c;
                    break;
                }
            }
            if(i == 1) {
                buffer[i++] = '\0';
                lista = insereToken(lista, criaToken(buffer, OPERADOR_LOGICO, NULL));
            }
            else {
                buffer[i] = '\0';
                lista = insereToken(lista, criaToken(buffer, KEYWORD, NULL));
            }

        }
        else if (c == '+'|| c == '-' || c == '^') {
                strcpy(buffer, "");
                i = 0;

                buffer[i] = c;
                c = getChar(in_file);
                if ((buffer[i] == '+' && c == '+') ||
                    (buffer[i] == '-' && c == '-') ||
                    (buffer[i] == '-' && c == '>')) {
                    i++;
                    buffer[i] = c;
                }
                else {
                    fseek(in_file, -sizeof(char), SEEK_CUR); // rebobina 1 char
                }
                buffer[i + 1] = '\0';
            lista = insereToken(lista, criaToken(buffer, OPERADOR, NULL));
        }
        else if (c == '|') {
            strcpy(buffer, "");
            i = 0;

            buffer[i] = c;
            c = getChar(in_file);
            if (c == '|') {
                i++;
                buffer[i] = c;
                buffer[i + 1] = '\0';
                lista = insereToken(lista, criaToken(buffer, OPERADOR_LOGICO, NULL));
            }
            else {
                fseek(in_file, -sizeof(char), SEEK_CUR); // rebobina 1 char
                buffer[i + 1] = '\0';
                lista = insereToken(lista, criaToken(buffer, DESCONHECIDO, NULL));
            }
        }
        else if (c == '/') {
            strcpy(buffer, "");
            i = 0;
            buffer[i] = c;
            c = getChar(in_file);
            i++;
            if (c == '/') {
                buffer[i] = c;
                i++;
                do {
                    c = getChar(in_file);
                    buffer[i] = c;
                    i++;
                } while(c != '\n' && !isEOF(in_file));
                buffer[i - 1] = '\0';
                lista = insereToken(lista, criaToken(buffer, COMENTARIO_1L, NULL));
                lista = insereToken(lista, criaToken("\n", NOVA_LINHA, NULL));
                count = ht_get(th, "//");
                if (count == -1) ht_set(th, "//", 1);
                else ht_set(th, "//", count + 1); 
            }
            else if (c == '*') {
                buffer[i] = c;
                i++;
                c = getChar(in_file);
                buffer[i] = c;
                d = getChar(in_file);
                i++;
                do {
                    if (!(c == '*' && d == '/')) {
                        buffer[i] = d;
                        c = d;
                        d = getChar(in_file);
                        i++;
                    }
                    else {
                        buffer[i] = d;
                        buffer[i + 1] = '\0';
                        break;
                    }
                    
                } while(!isEOF(in_file));
                lista = insereToken(lista, criaToken(buffer, COMENTARIO_ML, NULL));
                count = ht_get(th, "*/");
                if (count == -1) ht_set(th, "*/", 1);
                else ht_set(th, "*/", count + 1); 
            }
            else {
                buffer[i] = '\0';
                fseek(in_file, -sizeof(char), SEEK_CUR);
                lista = insereToken(lista, criaToken(buffer, OPERADOR, NULL));
            }
        }
        else if (c == '=') {
            strcpy(buffer, "");
            i = 0;
            buffer[i] = c;
            i++;
            c = getChar(in_file);
            if (c == '=') {
                buffer[i] = c;
                buffer[i++] = '\0';
                lista = insereToken(lista, criaToken(buffer, OPERADOR_LOGICO, NULL));
            }
            else {
                buffer[i] = '\0';
                lista = insereToken(lista, criaToken(buffer, IGUAL, NULL));
            }
        }
        else if (c == '!') {
            strcpy(buffer, "");
            i = 0;
            buffer[i] = c;
            i++;
            c = getChar(in_file);
            while((isLetra(c) || isNumero(c) || c == '_' || c == '(' || c == ')') && !isEOF(in_file)) {
                buffer[i] = c;
                c = getChar(in_file);
                i++;
            }
            if(i == 1 && c == '=') {
                buffer[i] = c;
                buffer[i++] = '\0';
                lista = insereToken(lista, criaToken(buffer, NAO_IGUAL, NULL));
            }
            else {
                buffer[i] = '\0';
                lista = insereToken(lista, criaToken(buffer, KEYWORD, NULL));
            }
        }
        else if (c == '<' || c == '>') {
            strcpy(buffer, "");
            i = 0;
            buffer[i] = c;
            i++;
            c = getChar(in_file);
            if (c == '<' || c == '>' || c == '=') {
                buffer[i] = c;
                buffer[i++] = '\0';
                lista = insereToken(lista, criaToken(buffer, OPERADOR_LOGICO, NULL));
            }
            else {
                buffer[i] = '\0';
                lista = insereToken(lista, criaToken(buffer, DESCONHECIDO, NULL));
            }
        }
        else if (c == '"') {
            strcpy(buffer, "");
            i = 0;
            j = i;
            do {
                buffer[i] = c;
                c = getChar(in_file);
                i++;
            } while(c != '"' && c != '\n' && !isEOF(in_file));
            buffer[i] = c;
            buffer[i + 1] = '\0';
            lista = insereToken(lista, criaToken(buffer, LITERAL_S, NULL));
        }
        else if (c == '\'') {
            strcpy(buffer, "");
            i = 0;
            j = i;
            do {
                buffer[i] = c;
                c = getChar(in_file);
                i++;
            } while(c != '\'' && c != '\n' && !isEOF(in_file));
            buffer[i] = c;
            buffer[i + 1] = '\0';
            lista = insereToken(lista, criaToken(buffer, LITERAL_C, NULL));
        }
    }
    marcaVariaveis(lista);

    FILE *out_file  = fopen(nomeArqSaida, "w");

    fprintf(out_file, "%s", HTML_CABECALHO);
    
    imprimeContadoresHTML(out_file, map, th, HTML_TABELA_TIPO, "corT", TIPO_INT, TIPO_DESCONHECIDO);
    imprimeContadoresHTML(out_file, map, th, HTML_TABELA_CMD, "corC", COMANDO, COMANDO);
    imprimeContadoresHTML(out_file, map, th, HTML_TABELA_FUNC, "corF", FUNCAO, FUNCAO);
    imprimeComentariosHTML(out_file, th, HTML_TABELA_COM);
    fprintf(out_file, "%s", HTML_TABELA_VAR);
    imprimeTokensHTML(out_file, lista, "char", VAR_CHAR);
    imprimeTokensHTML(out_file, lista, "int", VAR_INT);
    imprimeTokensHTML(out_file, lista, "long", VAR_LONG);
    imprimeTokensHTML(out_file, lista, "short", VAR_SHORT);
    imprimeTokensHTML(out_file, lista, "float", VAR_FLOAT);
    imprimeTokensHTML(out_file, lista, "double", VAR_DOUBLE);

    fprintf(out_file, "%s", HTML_TABELA_FIM);
    fprintf(out_file,
        "<h2>Codigo fonte:</h2>\
         <div style=\"background: #eee; border: 1px solid #888; margin: 5px; padding: 5px;\">");
    fprintf(out_file, "\n");
    removeComentario(lista);
    trocaNomeVariavel(lista);
    imprimeTodosTokens(out_file, lista);
    fprintf(out_file,"</div>");
    fprintf(out_file, "%s", HTML_RODAPE);
    fclose(out_file);

    return 0;
}