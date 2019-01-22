#include<stdio.h>
#include<string.h>
#include<stdlib.h>

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
