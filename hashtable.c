#include "hashtable.h"

#define ULONG_MAX 429496729UL

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

    unsigned long int hashval;
    int i = 0;

    /* Converte nossa string num inteiro */
    while( i < strlen(chave)) {
        // 00000000 00000000 00000000 00001111
        // 00000000 00000000 00001111 00000000
        hashval = hashval << 8;
        // printf("\t\"%s\"-> chave[%d] = %d\n", chave, i, 0b0000000011111111 & chave[i]);

        hashval += 0b0000000011111111 & chave[i];
        i++;
    }
    // printf("ht_hash(%s) = %lu\n", chave, hashval);

    return hashval % tabelaHash->tamanho;
}

/* Cria um par chave-valor. */
linha_t *novopar_th( char *chave, int valor, unsigned long int gaveta ) {
    linha_t *novopar;

    if( ( novopar = malloc( sizeof( linha_t ) ) ) == NULL ) {
        return NULL;
    }

    if( ( novopar->chave = strdupNosso( chave ) ) == NULL ) {
        return NULL;
    }

    novopar->valor = valor;
    novopar->gaveta = gaveta;
        
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
        novopar = novopar_th( chave, valor, gaveta );

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

linha_t *ht_get_struct( tabelaHash_t *tabelaHash, char *chave ) {
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
        return NULL;
    } else {
        return par;
    }
}
