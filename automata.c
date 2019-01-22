#include "automata.h"

estado_t *criaEstado(char *estado, enum fase tipo, estado_t *abaixo, estado_t *ao_lado) {
    estado_t *ret;
    ret = malloc(sizeof(estado_t));
    
    ret->estado = estado;
    ret->tipo = tipo;
    ret->abaixo = abaixo;
    ret->ao_lado = ao_lado;
    
    return ret;
}

estado_t *insereEstadoOrdenado(estado_t *raiz, estado_t *alvo, estado_t *ultimo) {
    if(raiz == NULL) raiz = alvo;
    else if(strcmp(alvo->estado, raiz->estado) == 0) free(alvo);
    else if(strcmp(alvo->estado, raiz->estado) < 0 && ultimo != NULL) {
        ultimo->ao_lado = alvo;
        alvo->ao_lado = raiz;
    }
    else {
        if(strComecaCom(alvo->estado, raiz->estado)) 
            raiz->abaixo = insereEstadoOrdenado(raiz->abaixo, alvo, raiz);             
        else 
            raiz->ao_lado = insereEstadoOrdenado(raiz->ao_lado, alvo, raiz);
    }
    return raiz;
}

void imprimeAutomato(estado_t *raiz, int nivel) {
    if (raiz != NULL) {
        for(int i = 0; i < nivel; i++) printf("->");
        printf("[%s]\n", raiz->estado);
        imprimeAutomato(raiz->abaixo, nivel + 1);
        imprimeAutomato(raiz->ao_lado, nivel);
    }
}

estado_t *montaAutomato(char *palavras[], int n_palavras) {
    estado_t  *automato = NULL;
    int i, j;
    char **vet_str;

    for(i = 0; i < n_palavras; i++) {
        vet_str = particionaString(palavras[i]);
        for (j = 0; j < strlen(palavras[i]); j++) {
            estado_t* atual = criaEstado(vet_str[j], INTERMEDIARIO, NULL, NULL);
            automato = insereEstadoOrdenado(automato, atual, NULL);
        }
    }
    return automato;
}

int encontraEstado(estado_t *raiz, char* padrao, int nivel) {
    if(raiz == NULL) {
        return INEXISTENTE;
    }
    else if(strcmp(padrao, raiz->estado) == 0) {
        if (raiz->abaixo == NULL) return FINAL;
        else if(nivel > 0) return INTERMEDIARIO;
        else return INICIAL;
    }
    else if(strcmp(padrao, raiz->estado) < 0) {
        return INEXISTENTE;
    }
    else {
        if(strComecaCom(padrao, raiz->estado)) 
            return encontraEstado(raiz->abaixo, padrao, nivel + 1);             
        else 
            return encontraEstado(raiz->ao_lado, padrao, nivel);             
    }
}
