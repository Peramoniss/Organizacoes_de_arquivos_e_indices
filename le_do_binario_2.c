#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <conio.h>
#include <stdbool.h>
#include <time.h>

// Default order
#define ORDER 7

struct nodo_conteudo{
  char nome[100];
  int pos;
  struct nodo_conteudo *prox_conteudo;
};

struct nodo_header{
  char categoria[100];
  struct nodo_header *prox_header;
  struct nodo_conteudo *lista; 
};

struct nodo_header *createHeader(char categoria[100]){
  struct nodo_header *novo;
  novo = malloc(sizeof(struct nodo_header));
  strcpy(novo->categoria, categoria);
  novo->lista = NULL;
  novo->prox_header = NULL;
  return novo;
}

void insertIntoContent(char conteudo[100], int pos, struct nodo_header **origem){
  struct nodo_conteudo *novo, *aux;
  novo = malloc(sizeof(struct nodo_conteudo));
  strcpy(novo->nome, conteudo);
  novo->pos = pos;
  novo->prox_conteudo = NULL;

  aux = (*origem)->lista;

  if(aux == NULL){
    (*origem)->lista = novo;
    return;
  }

  /*while(aux->prox_conteudo != NULL)
    aux = aux->prox_conteudo;

  aux->prox_conteudo = novo;*/

  novo->prox_conteudo = aux;
  (*origem)->lista = novo;
  
  return;
}

void insere(struct nodo_header **raiz, char categoria[100], char conteudo[100], int pos){
  struct nodo_header *aux = *raiz;
  if(aux == NULL){
    *raiz = createHeader(categoria);
    insertIntoContent(conteudo, pos, raiz);
    return;
  }

  while(aux->prox_header != NULL)
  {
    if(strcmp(categoria, aux->categoria) == 0){
      insertIntoContent(conteudo, pos, &aux);
      return;
    }

    aux = aux->prox_header;
  }

  if(strcmp(categoria, aux->categoria) == 0){ //testo com o ultimo valor antes de enviar pra criar
      insertIntoContent(conteudo, pos, &aux);
      return;
  }

  aux->prox_header = createHeader(categoria);
  insertIntoContent(conteudo, pos, &(aux->prox_header));

}

int consulta_categoria(struct nodo_header *raiz, char modo[100], int limit){
  if(raiz == NULL) return 1;
  int w = 0, enter = 0, i = 0;
  if(strcmp(modo, "write") == 0)  w=1;
  struct nodo_header *aux;
  aux = raiz;
  while(aux != NULL){
    if(w || strcmp(modo, aux->categoria) == 0){
      printf("%s: \n", aux->categoria);
      enter = 1;
    } 
    struct nodo_conteudo *auxiliar = aux->lista;

    if(auxiliar == NULL)
      printf("Vazio");
    if(enter){
      while(auxiliar != NULL && i++<limit){
        printf("   %s (%d)\n", auxiliar->nome, (auxiliar->pos) / 350);
        auxiliar = auxiliar->prox_conteudo;
      }
      printf("\n");
      if(!w) break;
    }

    if(enter) break;
    aux = aux->prox_header;
  }

  printf("%d apps existem nesse filtro\n", i-1);

  return 0;
}

int consulta_chave_categoria(struct nodo_header *raiz, char modo[100], char chave[100]){
  if(raiz == NULL) return 1;
  int w = 0, enter = 0, i = 0;
  if(strcmp(modo, "write") == 0)  w=1;
  struct nodo_header *aux;
  aux = raiz;
  while(aux != NULL){
    if(w || strcmp(modo, aux->categoria) == 0){
      printf("%s: \n", aux->categoria);
      enter = 1;
    } 
    struct nodo_conteudo *auxiliar = aux->lista;

    if(auxiliar == NULL)
      printf("Vazio");
    if(enter){
      while(auxiliar != NULL){
        if(strcmp(chave, auxiliar->nome) == 0){
          printf("   %s foi encontrado em (%d)\n", auxiliar->nome, (auxiliar->pos) / 350);
          w = 0;
          break;
        }
        auxiliar = auxiliar->prox_conteudo;
      }
      printf("\n");
      if(!w) break;
    }

    if(enter) break;
    aux = aux->prox_header;
  }

  return 0;
}


//-----------------------------------------------------------------------------------------------------

typedef struct record {
  int value;
  struct nodo_conteudo* pos;
} record;

// Node
typedef struct node {
  void **pointers;
  record *keys;
  struct node *parent;
  bool is_leaf;
  int num_keys;
  struct node *next;
} node;

int order = ORDER;
node *queue = NULL;
bool verbose_output = false;

// Enqueue
void enqueue(node *new_node);

// Dequeue
node *dequeue(void);
int height(node *const root);
int pathToLeaves(node *const root, node *child);
void printLeaves(node *const root);
void printTree(node *const root);
void findAndPrint(node *const root, int key, bool verbose, int limit);
void findAndPrintRange(node *const root, int range1, int range2, bool verbose, int limit);
int findRange(node *const root, int key_start, int key_end, bool verbose,
        int returned_keys[], void *returned_pointers[]);
node *findLeaf(node *const root, int key, bool verbose);
record *find(node *root, int key, bool verbose, node **leaf_out);
int cut(int length);

record *makeRecord(int value, struct nodo_conteudo *pos);
node *makeNode(void);
node *makeLeaf(void);
int getLeftIndex(node *parent, node *left);
node *insertIntoLeaf(node *leaf, int key, struct nodo_conteudo ** pos, record *pointer, node *root);
node *insertIntoLeafAfterSplitting(node *root, node *leaf, int key, struct nodo_conteudo * pos,
                   record *pointer);
node *insertIntoNode(node *root, node *parent,
           int left_index, int key, struct nodo_conteudo * pos, node *right);
node *insertIntoNodeAfterSplitting(node *root, node *parent,
                   int left_index,
                   int key, struct nodo_conteudo * pos, node *right);
node *insertIntoParent(node *root, node *left, int key, struct nodo_conteudo * pos, node *right);
node *insertIntoNewRoot(node *left, int key, struct nodo_conteudo *pos, node *right);
node *startNewTree(int key, struct nodo_conteudo *pos, record *pointer);
node *insert(node *root, int key, int value, int pos);

// Enqueue
void enqueue(node *new_node) {
  node *c;
  if (queue == NULL) {
    queue = new_node;
    queue->next = NULL;
  } else {
    c = queue;
    while (c->next != NULL) {
      c = c->next;
    }
    c->next = new_node;
    new_node->next = NULL;
  }
}

// Dequeue
node *dequeue(void) {
  node *n = queue;
  queue = queue->next;
  n->next = NULL;
  return n;
}

// Print the leaves
void printLeaves(node *const root) {
  if (root == NULL) {
    printf("Empty tree.\n");
    return;
  }
  int i;
  node *c = root;
  while (!c->is_leaf)
    c = c->pointers[0];
  while (true) {
    for (i = 0; i < c->num_keys; i++) {
      if (verbose_output)
        printf("%p ", c->pointers[i]);
      printf("%d ", c->keys[i].value);
    }
    if (verbose_output)
      printf("%p ", c->pointers[order - 1]);
    if (c->pointers[order - 1] != NULL) {
      printf(" | ");
      c = c->pointers[order - 1];
    } else
      break;
  }
  printf("\n");
}

// Calculate height
int height(node *const root) {
  int h = 0;
  node *c = root;
  while (!c->is_leaf) {
    c = c->pointers[0];
    h++;
  }
  return h;
}

// Get path to root
int pathToLeaves(node *const root, node *child) {
  int length = 0;
  node *c = child;
  while (c != root) {
    c = c->parent;
    length++;
  }
  return length;
}

// Print the tree
void printTree(node *const root) {
  node *n = NULL;
  int i = 0;
  int rank = 0;
  int new_rank = 0;

  if (root == NULL) {
    printf("Empty tree.\n");
    return;
  }
  queue = NULL;
  enqueue(root);
  while (queue != NULL) {
    n = dequeue();
    if (n->parent != NULL && n == n->parent->pointers[0]) {
      new_rank = pathToLeaves(root, n);
      if (new_rank != rank) {
        rank = new_rank;
        printf("\n");
      }
    }
    if (verbose_output)
      printf("(%p)", n);
    for (i = 0; i < n->num_keys; i++) {
      if (verbose_output)
        printf("%p ", n->pointers[i]);
      printf("%d [%d]", n->keys[i].value, n->keys[i].pos);
    }
    if (!n->is_leaf)
      for (i = 0; i <= n->num_keys; i++)
        enqueue(n->pointers[i]);
    if (verbose_output) {
      if (n->is_leaf)
        printf("%p ", n->pointers[order - 1]);
      else
        printf("%p ", n->pointers[n->num_keys]);
    }
    printf("| ");
  }
  printf("\n");
}

// Find the node and print it
void findAndPrint(node *const root, int key, bool verbose, int limit) {
  node *leaf = NULL;
  record *r = find(root, key, verbose, NULL);
  int i=0;
  if (r == NULL)
    printf("Record not found under key %d.\n", key);
  else{
    struct nodo_conteudo *aux = r->pos;
    while(aux != NULL && i<limit){
      printf("Record at %p -- %d time, key %d, value %d, position %d.\n",
         r, i++, key, r->value, (aux->pos + 350) / 350);
      aux = aux->prox_conteudo;
    }

  }
}

// Find and print the range
void findAndPrintRange(node *const root, int key_start, int key_end,
             bool verbose, int limit) {
  int i, j=0;
  int array_size = key_end - key_start + 1;
  int returned_keys[array_size];
  void *returned_pointers[array_size];
  int num_found = findRange(root, key_start, key_end, verbose,
                returned_keys, returned_pointers);
  if (!num_found)
    printf("None found.\n");
  else {
    for (i = 0; i < num_found; i++){
      //if(i>limit) break;
      j=0;
      struct nodo_conteudo *aux = ((record *) returned_pointers[i])->pos;
      while(aux != NULL && j<limit){
        printf("Record at %p -- %d time, key %d, value %d, position %d.\n",
          returned_pointers[i], j++, returned_keys[i], ((record *) returned_pointers[i])->value, (aux->pos + 350) / 350);
        aux = aux->prox_conteudo;
      }
    }
  }
}

// Find the range
int findRange(node *const root, int key_start, int key_end, bool verbose,
        int returned_keys[], void *returned_pointers[]) {
  int i, num_found;
  num_found = 0;
  node *n = findLeaf(root, key_start, verbose);
  if (n == NULL)
    return 0;
  for (i = 0; i < n->num_keys && n->keys[i].value < key_start; i++)
    ;
  if (i == n->num_keys)
    return 0;
  while (n != NULL) {
    for (; i < n->num_keys && n->keys[i].value <= key_end; i++) {
      returned_keys[num_found] = n->keys[i].value;
      returned_pointers[num_found] = n->pointers[i];
      num_found++;
    }
    n = n->pointers[order - 1];
    i = 0;
  }
  return num_found;
}

// Find the leaf
node *findLeaf(node *const root, int key, bool verbose) {
  if (root == NULL) {
    if (verbose)
      printf("Empty tree.\n");
    return root;
  }
  int i = 0;
  node *c = root;
  while (!c->is_leaf) {
    if (verbose) {
      printf("[");
      for (i = 0; i < c->num_keys - 1; i++)
        printf("%d ", c->keys[i]);
      printf("%d] ", c->keys[i]);
    }
    i = 0;
    while (i < c->num_keys) {
      if (key >= c->keys[i].value)
        i++;
      else
        break;
    }
    if (verbose)
      printf("%d ->\n", i);
    c = (node *)c->pointers[i];
  }
  if (verbose) {
    printf("Leaf [");
    for (i = 0; i < c->num_keys - 1; i++)
      printf("%d ", c->keys[i]);
    printf("%d] ->\n", c->keys[i]);
  }
  return c;
}

record *find(node *root, int key, bool verbose, node **leaf_out) {
  if (root == NULL) {
    if (leaf_out != NULL) {
      *leaf_out = NULL;
    }
    return NULL;
  }

  int i = 0;
  node *leaf = NULL;

  leaf = findLeaf(root, key, verbose);

  for (i = 0; i < leaf->num_keys; i++)
    if (leaf->keys[i].value == key)
      break;
  if (leaf_out != NULL) {
    *leaf_out = leaf;
  }
  if (i == leaf->num_keys)
    return NULL;
  else
    return (record *)leaf->pointers[i];
}

int cut(int length) {
  if (length % 2 == 0)
    return length / 2;
  else
    return length / 2 + 1;
}

record *makeRecord(int value, struct nodo_conteudo *pos) {
  record *new_record = (record *)malloc(sizeof(record));
  if (new_record == NULL) {
    perror("Record creation.");
    exit(EXIT_FAILURE);
  } else {
    new_record->value = value;
    new_record->pos = pos;
  }
  return new_record;
}

node *makeNode(void) {
  node *new_node;
  new_node = malloc(sizeof(node));
  if (new_node == NULL) {
    perror("Node creation.");
    exit(EXIT_FAILURE);
  }
  new_node->keys = malloc((order - 1) * sizeof(record));
  if (new_node->keys == NULL) {
    perror("New node keys array.");
    exit(EXIT_FAILURE);
  }
  new_node->pointers = malloc(order * sizeof(void *));
  if (new_node->pointers == NULL) {
    perror("New node pointers array.");
    exit(EXIT_FAILURE);
  }
  new_node->is_leaf = false;
  new_node->num_keys = 0;
  new_node->parent = NULL;
  new_node->next = NULL;
  return new_node;
}

node *makeLeaf(void) {
  node *leaf = makeNode();
  leaf->is_leaf = true;
  return leaf;
}

int getLeftIndex(node *parent, node *left) {
  int left_index = 0;
  while (left_index <= parent->num_keys &&
       parent->pointers[left_index] != left)
    left_index++;
  return left_index;
}

node *insertIntoLeaf(node *leaf, int key, struct nodo_conteudo ** pos, record *pointer, node *root) {
  int i, insertion_point;
   //printf("Chegando em insertIntoLeaf com key: %d e pos: %d\n", key, pos);

  insertion_point = 0;

  while (insertion_point < leaf->num_keys && leaf->keys[insertion_point].value < key)
    insertion_point++;

  for (i = leaf->num_keys; i > insertion_point; i--) {
    leaf->keys[i] = leaf->keys[i - 1];
    leaf->pointers[i] = leaf->pointers[i - 1];
  }
  leaf->keys[insertion_point].value = key;
  leaf->keys[insertion_point].pos = *pos;
  leaf->pointers[insertion_point] = pointer;
  leaf->num_keys++;
  return leaf;
}

node *insertIntoLeafAfterSplitting(node *root, node *leaf, int key, struct nodo_conteudo * pos, record *pointer) {
  node *new_leaf;
  record *temp_keys;
  void **temp_pointers;
  int insertion_index, split, new_key, i, j;
  struct nodo_conteudo *new_pos;

   //printf("Chegando em insertIntoLeafAfterSplitting com key: %d e pos: %d\n", key, pos);

  new_leaf = makeLeaf();

  temp_keys = malloc(order * sizeof(record));
  if (temp_keys == NULL) {
    perror("Temporary keys array.");
    exit(EXIT_FAILURE);
  }

  temp_pointers = malloc(order * sizeof(void *));
  if (temp_pointers == NULL) {
    perror("Temporary pointers array.");
    exit(EXIT_FAILURE);
  }

  insertion_index = 0;
  while (insertion_index < order - 1 && leaf->keys[insertion_index].value < key)
    insertion_index++;

  for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
    if (j == insertion_index)
      j++;
    temp_keys[j] = leaf->keys[i];
    temp_pointers[j] = leaf->pointers[i];
  }

  temp_keys[insertion_index].value = key;
  temp_keys[insertion_index].pos = pos;
  temp_pointers[insertion_index] = pointer;

  leaf->num_keys = 0;

  split = cut(order - 1);

  for (i = 0; i < split; i++) {
    leaf->pointers[i] = temp_pointers[i];
    leaf->keys[i] = temp_keys[i];
    leaf->num_keys++;
  }

  for (i = split, j = 0; i < order; i++, j++) {
    new_leaf->pointers[j] = temp_pointers[i];
    new_leaf->keys[j] = temp_keys[i];
    new_leaf->num_keys++;
  }

  free(temp_pointers);
  free(temp_keys);

  new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
  leaf->pointers[order - 1] = new_leaf;

  for (i = leaf->num_keys; i < order - 1; i++)
    leaf->pointers[i] = NULL;
  for (i = new_leaf->num_keys; i < order - 1; i++)
    new_leaf->pointers[i] = NULL;

  new_leaf->parent = leaf->parent;
  new_key = new_leaf->keys[0].value;
  new_pos = new_leaf->keys[0].pos;

  return insertIntoParent(root, leaf, new_key, new_pos, new_leaf);
}

node *insertIntoNode(node *root, node *n,
           int left_index, int key, struct nodo_conteudo * pos, node *right) {
  //printf("Chegando em insertIntoNode com key: %d e pos: %d\n", key, pos);

  int i;

  for (i = n->num_keys; i > left_index; i--) {
    n->pointers[i + 1] = n->pointers[i];
    n->keys[i] = n->keys[i - 1];
  }
  n->pointers[left_index + 1] = right;
  n->keys[left_index].value = key;
  n->keys[left_index].pos = pos;
  n->num_keys++;
  return root;
}

node *insertIntoNodeAfterSplitting(node *root, node *old_node, int left_index,
                   int key, struct nodo_conteudo * pos, node *right) {
  //printf("Chegando em insertIntoNodeAfterSplitting com key: %d e pos: %d\n", key, pos);
  int i, j, split, k_prime;
  node *new_node, *child;
  record *temp_keys;
  node **temp_pointers;
  struct nodo_conteudo *k_prime_pos;

  temp_pointers = malloc((order + 1) * sizeof(node *));
  if (temp_pointers == NULL) {
    exit(EXIT_FAILURE);
  }
  temp_keys = malloc(order * sizeof(record));
  if (temp_keys == NULL) {
    exit(EXIT_FAILURE);
  }

  for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++) {
    if (j == left_index + 1)
      j++;
    temp_pointers[j] = old_node->pointers[i];
  }

  for (i = 0, j = 0; i < old_node->num_keys; i++, j++) {
    if (j == left_index)
      j++;
    temp_keys[j] = old_node->keys[i];
  }

  temp_pointers[left_index + 1] = right;
  temp_keys[left_index].value = key;
  temp_keys[left_index].pos = pos;

  split = cut(order);
  new_node = makeNode();
  old_node->num_keys = 0;
  for (i = 0; i < split - 1; i++) {
    old_node->pointers[i] = temp_pointers[i];
    old_node->keys[i] = temp_keys[i];
    old_node->num_keys++;
  }
  old_node->pointers[i] = temp_pointers[i];
  k_prime = temp_keys[split - 1].value;
  k_prime_pos = temp_keys[split - 1].pos;

  for (++i, j = 0; i < order; i++, j++) {
    new_node->pointers[j] = temp_pointers[i];
    new_node->keys[j] = temp_keys[i];
    new_node->num_keys++;
  }
  new_node->pointers[j] = temp_pointers[i];
  free(temp_pointers);
  free(temp_keys);
  new_node->parent = old_node->parent;
  for (i = 0; i <= new_node->num_keys; i++) {
    child = new_node->pointers[i];
    child->parent = new_node;
  }

  return insertIntoParent(root, old_node, k_prime, k_prime_pos, new_node);
}

node *insertIntoParent(node *root, node *left, int key, struct nodo_conteudo * pos, node *right) {
  int left_index;
  node *parent;

  //printf("Chegando em insertIntoParent com key: %d e pos: %d\n", key, pos);


  parent = left->parent;

  if (parent == NULL)
    return insertIntoNewRoot(left, key, pos, right);

  left_index = getLeftIndex(parent, left);

  if (parent->num_keys < order - 1)
    return insertIntoNode(root, parent, left_index, key, pos, right);

  return insertIntoNodeAfterSplitting(root, parent, left_index, key, pos, right);
}

node *insertIntoNewRoot(node *left, int key, struct nodo_conteudo *pos, node *right) {
  //printf("Chegando em insertIntoNewRoot com key: %d e pos: %d\n", key, pos);

  node *root = makeNode();
  root->keys[0].value = key;
  root->keys[0].pos = pos;
  root->pointers[0] = left;
  root->pointers[1] = right;
  root->num_keys++;
  root->parent = NULL;
  left->parent = root;
  right->parent = root;
  return root;
}

node *startNewTree(int key, struct nodo_conteudo *pos, record *pointer) {
  //printf("Chegando em startNewTree com key: %d e pos: %d\n", key, pos);

  node *root = makeLeaf();
  root->keys[0].value = key;
  root->keys[0].pos = pos;
  root->pointers[0] = pointer;
  root->pointers[order - 1] = NULL;
  root->parent = NULL;
  root->num_keys++;
  return root;
}

node *insert(node *root, int key, int value, int pos) {
  //printf("Chegando em insert com key: %d e pos: %d\n", key, pos);

  record *record_pointer = NULL;
  node *leaf = NULL;
  struct nodo_conteudo *posit;
  posit = malloc(sizeof(struct nodo_conteudo));
  posit->pos = pos;
  posit->prox_conteudo = NULL;

  record_pointer = find(root, key, false, NULL);

  //if already exists
  if (record_pointer != NULL) {
    record_pointer->value = value;
    struct nodo_conteudo *aux = (record_pointer->pos);
    posit->prox_conteudo = aux;
    record_pointer->pos = posit;
  
    return root;
  }

  record_pointer = makeRecord(value, posit);


  if (root == NULL)
    return startNewTree(key, posit, record_pointer);

  leaf = findLeaf(root, key, false);

  if (leaf->num_keys < order - 1) {
    leaf = insertIntoLeaf(leaf, key, &posit, record_pointer, root);
    return root;
  }

  return insertIntoLeafAfterSplitting(root, leaf, key, posit, record_pointer);
}

//-----------------------------------------------------------------------------------------------------

void gera_indice_lista(struct nodo_header **raiz){
  FILE *read_ptr;
  read_ptr = fopen("teste.bin", "rb");
  char buffer[350], *nome, *categoria;
  int j=0;

  fread(buffer,350,1,read_ptr); //ignora cabecalho
    while ((fread(buffer,350,1,read_ptr) != 0))
    {
        char *conteudo = strtok(buffer, ";"); //ignora nome
        strtok(NULL, ";"); //ignora ID
        char *categoria = strtok(NULL, ";"); //pega categoria
        insere(raiz, categoria, conteudo, j*350);
        j++;
    }

    //printf("Indice em lista gerado com sucesso");
    fclose(read_ptr);
}

int gera_arvore(node **root){
    FILE *read_ptr;
    read_ptr = fopen("Playstore - Copia.csv", "rt");
    char buffer[350];
    int j=0;

    fgets(buffer, 350, read_ptr); //ignora cabecalho
    while ((fgets(buffer, 350, read_ptr) != NULL))
    {
        strtok(buffer, ";"); //ignora nome
        strtok(NULL, ";"); //ignora ID
        strtok(NULL, ";"); //ignora categoria
        char *indice = strtok(NULL, ";"); //pega max_installs
        int installs = atoi(indice);
        *root = insert(*root, installs, installs, (j+1)*350);
        j++;
    }

    //printf("Arvore gerada com sucesso");
    fclose(read_ptr);
}

int gera_arquivo_binario(){

    FILE *write_ptr, *read_ptr;
    write_ptr = fopen("teste.bin","w+");  // w for write, b for binary
    read_ptr = fopen("Playstore - Copia.csv", "rt");

    if(write_ptr == NULL || read_ptr == NULL)
        return 1;

    char buffer[350], *nome, *id, *categoria, *max_installs;
    int j=0, pos, escreve;

    while(fgets(buffer, 350, read_ptr) != NULL) //lê o dado do arquivo original
    {
        for(int i = strlen(buffer)+1; i<350; i++) //completa com espaços em branco para ter tamanho fixo dos registros
            buffer[i] = ' ';

        pos = j*350; //calcula posição para escrever o próximo dado
          fseek(write_ptr, pos, SEEK_SET); //vai até a aposição calculada
          fwrite(buffer,350,1,write_ptr); //escreve no arquivo binário o valor lido
          j++; //incrementa indíce para o calculo
        //}
    }

    fclose(write_ptr);
    fclose(read_ptr);

    return 0;
}

int gera_indice()
{
    FILE *read_ptr, *write_ptr;
    read_ptr = fopen("teste.bin", "rb");
    write_ptr = fopen("indice_parcial_teste.bin", "w+");

    if(write_ptr == NULL || read_ptr == NULL)
        return 1;

    char buffer[350], *indice, pos_str[25];
    int pos, pos_read, j=0;

    fread(buffer, 350, 1, read_ptr); //ignora primeira linha com os headers

    while(fread(buffer, 350, 1, read_ptr) != 0)  //lê do arquivo
    {        
        indice = strtok(buffer, ";"); //pega o nome do App
        
        pos = j*350; //pos calcula a posicao da escrita no arquivo de índice parcial
        fseek(write_ptr, pos, SEEK_SET); //vai até a posição calculada 
        
        pos_read = (pos+350)*100; //pos_read indica a posição de leitura. Tem de somar 350 para ignorar a primeira linha

        sprintf(pos_str, "%s;%d;", indice, pos_read); //cria string com a posição do registro limite ao qual esse índice representa (representa os que são maiores ou iguais a ele e os que são menores ou iguais ao que essa posição aponta)
        for(int i=strlen(pos_str); i<350; i++)
          pos_str[i] = ' ';
        fwrite(pos_str, 350,1,write_ptr); //escreve no arquivo de índice
        j++; //aumenta posição
        fseek(read_ptr, pos_read, SEEK_SET); //posiciona o arquivo para a próxima leitura
    }

    fseek(read_ptr, 0, SEEK_END);
    int fim = ftell(read_ptr);

    if((j-1)*350*100 != fim-350){
      fseek(read_ptr, ((j-1)*350)*100+350, SEEK_SET); //posiciona o arquivo para a próxima leitura válida (um depois da tentativa anterior)
      fread(buffer, 350, 1, read_ptr); //lê o registro seguinte
      
      indice = strtok(buffer, ";");
      sprintf(pos_str, "%s;%d;*", indice, fim-350); //cria string com a posição do registro limite ao qual esse índice representa (representa os que são maiores ou iguais a ele e os que são menores ou iguais ao que essa posição aponta)
      for(int i=strlen(pos_str); i<350; i++)
        pos_str[i] = ' ';
      fwrite(pos_str, 350, 1, write_ptr); //escreve no arquivo de índice a ultima posicao

    }

    fclose(write_ptr);
    fclose(read_ptr);

    return 0;
}

/* Swaps position of strings in array (char**) */
/*void swap(char *a[], char *b[]) {
	char *temp = *a;
	*a = *b;
	*b = temp;
}

void quicksort(char *arr[], unsigned int length) {
	unsigned int i, piv = 0;
	if (length <= 1) 
		return;
	
	for (i = 0; i < length; i++) {
		// if curr str < pivot str, move curr into lower array and  lower++(pvt)
		if (strcmp(arr[i], arr[length -1]) < 0) 	//use string in last index as pivot
			swap(arr + i, arr + piv++);		
	}
	//move pivot to "middle"
	swap(arr + piv, arr + length - 1);

	//recursively sort upper and lower
	quicksort(arr, piv++);			//set length to current pvt and increase for next call
	quicksort(arr + piv, length - piv);
}*/

// function for comparing two strings. This function 
// is passed as a parameter to _quickSort() when we 
// want to sort  
/*int cmpstr(void* v1, void* v2) 
{ 
    // casting v1 to char** and then assigning it to 
    // pointer to v1 as v1 is array of characters i.e 
    // strings. 
    char *a1 = *(char**)v1; 
    char *a2 = *(char**)v2; 
    return strcmp(a1, a2); 
} */
int cmpstr(const void* v1, const void* v2) {
    const char* a1 = *(const char**)v1;
    const char* a2 = *(const char**)v2;

    // Create temporary copies of a1 and a2 for comparison
    char a1_copy[strlen(a1) + 1];
    char a2_copy[strlen(a2) + 1];
    strcpy(a1_copy, a1);
    strcpy(a2_copy, a2);

    // Use strtok on the temporary copies
    char* token1 = strtok(a1_copy, ";");
    char* token2 = strtok(a2_copy, ";");

    // Compare the tokens
    while (token1 && token2) {
        int cmp = strcmp(token1, token2);
        if (cmp != 0) {
            return cmp;
        }
        token1 = strtok(NULL, ";");
        token2 = strtok(NULL, ";");
    }

    // If one string has more tokens, consider it greater
    if (token1) {
        return 1;
    } else if (token2) {
        return -1;
    } else {
        return 0; // Both strings are equal
    }
}

/* you can also write compare function for floats, 
    chars, double similarly as integer. */
// function for swap two elements 
void swap(void* v1, void* v2, int size) 
{ 
    // buffer is array of characters which will  
    // store element byte by byte 
    char buffer[size]; 

  
    // memcpy will copy the contents from starting 
    // address of v1 to length of size in buffer  
    // byte by byte. 
    memcpy(buffer, v1, size); 
    memcpy(v1, v2, size); 
    memcpy(v2, buffer, size); 
} 


// v is an array of elements to sort. 
// size is the number of elements in array 
// left and right is start and end of array 
//(*comp)(void*, void*) is a pointer to a function 
// which accepts two void* as its parameter 
void _qsort(void* v, int size, int left, int right, 
                      int (*comp)(void*, void*)) 
{ 
    void *vt, *v3; 
    int i, last, mid = (left + right) / 2; 
    if (left >= right) 
        return; 
  
    // casting void* to char* so that operations  
    // can be done. 
    void* vl = (char*)(v + (left * size)); 
    void* vr = (char*)(v + (mid * size)); 
    swap(vl, vr, size); 
    last = left; 
    for (i = left + 1; i <= right; i++) { 
  
        // vl and vt will have the starting address  
        // of the elements which will be passed to  
        // comp function. 
        vt = (char*)(v + (i * size)); 
        if ((*comp)(vl, vt) > 0) { 
            ++last; 
            v3 = (char*)(v + (last * size)); 
            swap(vt, v3, size); 
        } 
    } 
    v3 = (char*)(v + (last * size)); 
    swap(vl, v3, size); 
    _qsort(v, size, left, last - 1, comp); 
    _qsort(v, size, last + 1, right, comp); 
} 

int faz_em_memoria(int contador){
  FILE *read_ptr, *write_ptr;
  write_ptr = fopen("indice_exaustivo_teste.bin","w+");  // w for write, b for binary
    read_ptr = fopen("teste.bin", "rb");

  if(write_ptr == NULL || read_ptr == NULL)
        return 1;

  char bufferi[350];
  //char *lista[350];
  char **lista;

  lista = malloc(sizeof(char*)*contador);
  for(int i=0; i<contador; i++)
    lista[i] = malloc(sizeof(char)*350);

  int j=0;


  while(fread(bufferi, 350, 1, read_ptr)){
    //printf("%d  ", j);
    lista[j] = (char*)malloc(strlen(bufferi)*sizeof(char));
    strtok(bufferi, ";"); //ignora nome
    char *indice = strtok(NULL, ";"); //pega id
    //printf("%s", indice);
    snprintf(lista[j], 350, "%s;%d;", indice,ftell(read_ptr)-350);
    j++;

    /*for (int i = strlen(lista[j]); i < 350; i++)
    {
      lista[j][i] = tolower(lista[j][i]);
    }*/
  }

  //quicksort(lista, contador);
  _qsort(lista, sizeof(char*), 0, contador-1, (int (*)(void*, void*))(cmpstr)); 
  
  for(int i=0; i<contador; i++){
    //printf("%s\n", lista[i]);
    fwrite(lista[i], strlen(lista[i]), 1, write_ptr);
    fseek(write_ptr, i*350, SEEK_SET);
  }

  fclose(read_ptr);
  fclose(write_ptr);

  return 0;
}

int gera_csv_id(){
  FILE *write_ptr, *read_ptr;
  read_ptr = fopen("indice_exaustivo_teste.bin","rb");  // w for write, b for binary
  write_ptr = fopen("id.csv", "w+");
  char buffer[350];

  while(fread(buffer, 350, 1, read_ptr)){
    fprintf(write_ptr, "%s\n", buffer);
  }

  return 0;
}

int gera_indice_id(){

    FILE *write_ptr, *read_ptr;
    write_ptr = fopen("indice_exaustivo_teste.bin","w+");  // w for write, b for binary
    read_ptr = fopen("teste.bin", "rb");

    if(write_ptr == NULL || read_ptr == NULL)
        return 1;

    char buffer[350], *indice, menor[350], ultimo[350], pos_str[350];
    int j=0, pos, menor_pos, k;

    strcpy(ultimo, "primeiro");

    fseek(read_ptr, 0, SEEK_END);
    int contador = ftell(read_ptr) / 350;
    if(contador >= 3000){
      //printf("%d", contador);
      return faz_em_memoria(contador);
    }

    printf("Faz por bubble");

    rewind(read_ptr);
    //fgets(buffer, 350, read_ptr); //ignora primeira linha

    for(int i=0; i<contador; i++){
        strcpy(menor, "zzzzzzzzzzzzzzz");
        //fgets(buffer, 350, read_ptr); //ignora primeira linha
        fread(buffer, 350, 1, read_ptr);
        k=1;
        while(fread(buffer, 350, 1, read_ptr) != 0) //começa a ler os demais dados do arquivo
        {
            char teste[350];
            strcpy(teste, buffer);
            strtok(buffer, ";"); //ignora nome
            indice = strtok(NULL, ";"); //pega ID 
            int resultado = strcmp(menor, indice);
            int passado = strcmp(ultimo, indice);
            if(resultado > 0 && (passado < 0 || j == 0)){
                strcpy(menor, indice);
                menor_pos = k * 350;
            }
            k++;
        }

        rewind(read_ptr);

        //int ref = strlen(menor);
        //menor[ref++] = ';';

        pos = j*350; //calcula posição para escrever o próximo dado
        fseek(write_ptr, pos, SEEK_SET); //vai até a aposição calculada
        //fwrite(menor,ref,1,write_ptr); //escreve no arquivo binário o valor lido

        //int original_pos = ftell(read_ptr)-350;
        sprintf(pos_str, "%s;%d;", menor, menor_pos); //cria string com a posição do registro limite ao qual esse índice representa (representa os que são maiores ou iguais a ele e os que são menores ou iguais ao que essa posição aponta)
        for(int i = strlen(pos_str); i<350; i++) //completa com espaços em branco para ter tamanho fixo dos registros
            pos_str[i] = ' ';
        fwrite(pos_str, 350,1,write_ptr); //escreve no arquivo de índice
        
        j++; //incrementa indíce para o calculo
        strcpy(ultimo, menor);
    }

    fclose(write_ptr);
    fclose(read_ptr);

    return 0;
}

int pesquisa_binaria(char comparativo[]){
    printf("Procurando %s...", comparativo);
    FILE *read_ptr;
    read_ptr = fopen("teste.bin", "rb");
    char buffer[350], *indice, backup[350], carac, comparacao[350];
    int i;
    char ultimo[350];
    //int ultimo;

    strcpy(comparacao,comparativo); //temp

    /*for(i = 0; i<strlen(comparativo); i++){ //coloca em minúsculas (tolower()) para efeito de comaparação
        comparacao[i] = tolower(comparativo[i]);
    }
    comparacao[i] = '\0'; //encerra novo string com minusculas*/

    fseek(read_ptr, 0, SEEK_END);
    int fim = ftell(read_ptr); //indica o tamanho do arquivo
    printf("%d", fim/350);
    int inicio = 0;
    rewind(read_ptr); //volta arquivo para o comeco

    while(1)
    {
        int offset = (fim + inicio)/2; //calcula a posicao do meio do arquivo (principio da busca binaria)
        while(offset%350 != 0){ //procura por uma posição que inicie um registro (seja divisível pelo tamanho do registro) 
            offset--;
        }

        //printf("_ %d _", offset/350);
        
        fseek(read_ptr, offset, SEEK_SET); //procura a partir do começo a posição
        fread(buffer, 350, 1, read_ptr); //lê o registro
        strcpy(backup, buffer); //copia o que foi lido para backup para preservar a leitura após modificações no buffer

        
        //if(ultimo == offset || inicio == fim)
        if(strcmp(ultimo, backup) == 0 || inicio == fim)
            return 1;
        
        indice = strtok(buffer, ";"); //pega so o nome do app        

        /*for(int i = 0; i<strlen(indice); i++){ //deixa em minúsculas
            if(indice[i]>='A' && indice[i]<='Z')
                indice[i] = tolower(indice[i]);
        }*/

        int resultado = strcmp(indice, comparacao); //compara
        //printf("%s VS %s = %d - \n", indice, comparacao, resultado);

        if(resultado == 0){
            fseek(read_ptr, offset, SEEK_SET); //retoma a posição
            //printf("Valor encontrado na posicao %d", (ftell(read_ptr)/350)+1); //divide a posição atual por 350 para ter a ordem do registro
            break; //encontrou
        }
        else if(resultado > 0){
            //valor encontrado é maior que o procurado
            fseek(read_ptr, offset, SEEK_SET); //retoma a posicao que estava
            fim = ftell(read_ptr); //seta a nova posicao como fim
            //printf("Maior\n");
        }else if(resultado < 0){
            //valor encontrado é menor que o procurado
            fseek(read_ptr, offset, SEEK_SET); //retoma a posicao que estava
            inicio = ftell(read_ptr); //seta a nova posicao como fim
            //printf("Menor\n");
        }

        strcpy(ultimo, backup); //salva o registro nao alterado (backup) como o ultimo registro lido. Usado para comparar com o próximo lido e verificar se entrou em loop, assim indicando que não há o valor procurado no índice
        //ultimo = offset;
    }

    return 0;
}

int pesquisa_binaria_indice_parcial(char comparativo[]){
    printf("Procurando %s...\n", comparativo);
    FILE *read_ptr;
    read_ptr = fopen("indice_parcial_teste.bin", "rb"); //abre índice
    char buffer[350], *indice, backup[350], carac, comparacao[350];
    int i, ultimo;

    strcpy(comparacao, comparativo);

    /*for(i = 0; i<strlen(comparativo); i++){ //coloca em minúsculas (tolower()) para efeito de comaparação
        comparacao[i] = tolower(comparativo[i]);
    }
    comparacao[i] = '\0'; //encerra novo string com minusculas*/

    fseek(read_ptr, 0, SEEK_END); 
    int fim = ftell(read_ptr); //indica o tamanho do arquivo
    int inicio = 0;
    rewind(read_ptr); //volta arquivo para o comeco

    while(1)
    {
        int offset = (fim + inicio)/2; //calcula a posicao do meio do arquivo (principio da busca binaria)
        while(offset%350 != 0){ //procura por uma posição que inicie um registro (seja divisível pelo tamanho do registro) 
            offset--;
        }

        //printf("_ %d _", offset/350);
        
        fseek(read_ptr, offset, SEEK_SET); //procura a posição do registro calculada
        fread(buffer, 350, 1, read_ptr); //lê o registro
        //printf("Buffer: %s", buffer);
        strcpy(backup, buffer); //copia o que foi lido para backup para preservar a leitura após modificações no buffer
        //printf("Backup: %s", backup);

        if(ultimo == offset || inicio == fim){ //indica que registro procurado nao foi encontrado. Vai procurar no arquivo de dados original entao          
            FILE *read_ptr2;
            read_ptr2 = fopen("teste.bin", "rb");

            char *teste = strtok(backup, ";"); //ignora nome do app
            offset = atoi(strtok(NULL, ";")); //pega a posição onde há o valor limite            
            printf("%s (%d)", teste, offset);

            while(1)
            {
                fseek(read_ptr2, offset, SEEK_SET); //vai até a posição do valor limite
                fread(buffer, 350, 1, read_ptr2); //lê o app limite
                indice = strtok(buffer, ";"); //separa só o nome
                printf("%s\n", indice);

                /*for(int j = 0; j<strlen(indice); j++){ //converte para minusculas
                    if(indice[j]>='A' && indice[j]<='Z')
                        indice[j] = tolower(indice[j]);
                }*/

                int resultado = strcmp(indice, comparacao); //compara
                if(resultado == 0){
                    printf("Valor encontrado na posicao %d", ((ftell(read_ptr2)/350)));
                    return 0;
                }
                else if(resultado < 0){ //se ler um valor menor do que o que está sendo procurado, significa que o valor não está no arquivo
                    return 1;
                }
                else offset-=350; //se o valor lido é maior do que o procurado, lê o anterior (que é menor)
            }
      }
        
        indice = strtok(backup, ";"); //separa só o nome

        /*for(int i = 0; i<strlen(indice); i++){ //converte para minusculas
            if(indice[i]>='A' && indice[i]<='Z')
                indice[i] = tolower(indice[i]);
        }*/

        int resultado = strcmp(indice, comparacao); //compara
        printf("\n%s VS %s = %d - %d VS %d -", indice, comparacao, resultado, indice[0], comparacao[0]);

        if(resultado == 0){
            fseek(read_ptr, offset, SEEK_SET); //retoma a posição calculada (a leitura fez com que estivesse na posição seguinte)
            int pos_found = (offset/350*100) + (offset == 0 ? 2 : 1); //pega valor limite
            strtok(NULL, ";");
            char *sinal = strtok(NULL, ";");
            sinal[1] = '\0';
            if(strcmp(sinal, "*") == 0){
              printf("Valor encontrado na posicao %d", pos_found-100+1); //divide por 350 para dar a ordem do registro e reduz 9 posicoes para chegar na posição que o índice está representando
            }else
              printf("Valor encontrado na posicao %d", pos_found); //divide por 350 para dar a ordem do registro e reduz 9 posicoes para chegar na posição que o índice está representando
            break; //encontrou
        }
        else if(resultado > 0){ //valor encontrado é maior que o procurado
            fseek(read_ptr, offset, SEEK_SET); //retoma a posicao que estava
            fim = ftell(read_ptr); //seta a nova posicao como fim
            //printf("Maior\n");
        }else if(resultado < 0){//valor encontrado é menor que o procurado
            fseek(read_ptr, offset, SEEK_SET); //retoma a posicao que estava
            inicio = ftell(read_ptr); //seta a nova posicao como fim
            //printf("Menor\n");
        }

        ultimo = offset;
        //strcpy(ultimo, backup); //salva o registro nao alterado (backup) como o ultimo registro lido. Usado para comparar com o próximo lido e verificar se entrou em loop, assim indicando que não há o valor procurado no índice
    }

    return 0;
}

int pesquisa_binaria_indice_exaustivo(char comparativo[]){
    printf("Procurando %s...\n", comparativo);
    FILE *read_ptr;
    read_ptr = fopen("indice_exaustivo_teste.bin", "rb"); //abre índice
    char buffer[350], *indice, ultime[350], backup[350], carac, comparacao[350];
    int i, ultimo;

    strcpy(comparacao, comparativo);

    /*for(i = 0; i<strlen(comparativo); i++){ //coloca em minúsculas (tolower()) para efeito de comaparação
        comparacao[i] = tolower(comparativo[i]);
    }
    comparacao[i] = '\0'; //encerra novo string com minusculas*/

    fseek(read_ptr, 0, SEEK_END); 
    int fim = ftell(read_ptr); //indica o tamanho do arquivo
    int inicio = 0;
    rewind(read_ptr); //volta arquivo para o comeco

    while(1)
    {
        int offset = (fim + inicio)/2; //calcula a posicao do meio do arquivo (principio da busca binaria)
        while(offset%350 != 0){ //procura por uma posição que inicie um registro (seja divisível pelo tamanho do registro) 
            offset--;
        }

        //printf("\n_ %d _", offset/350);
        
        fseek(read_ptr, offset, SEEK_SET); //procura a posição do registro calculada
        fread(buffer, 350, 1, read_ptr); //lê o registro
        strcpy(backup, buffer); //copia o que foi lido para backup para preservar a leitura após modificações no buffer

        if(ultimo == offset || inicio == fim){ //indica que registro procurado nao foi encontrado. Vai procurar no arquivo de dados original entao          
            return 1;
        }
        
        indice = strtok(buffer, ";"); //separa só o ID

        /*for(int i = 0; i<strlen(indice); i++){ //converte para minusculas
            if(indice[i]>='A' && indice[i]<='Z')
                indice[i] = tolower(indice[i]);
        }*/

        int resultado = strcmp(indice, comparacao); //compara
        //printf("\n%s VS %s = %d - %d VS %d -", indice, comparacao, resultado, indice[0], comparacao[0]);

        if(resultado == 0){
            fseek(read_ptr, offset, SEEK_SET); //retoma a posição calculada (a leitura fez com que estivesse na posição seguinte)
            int pos_found = atoi(strtok(NULL, ";")); //pega valor limite
            printf("Valor encontrado na posicao %d", (pos_found/350)+1); //divide por 350 para dar a ordem do registro e reduz 9 posicoes para chegar na posição que o índice está representando
            break; //encontrou
        }
        else if(resultado > 0){ //valor encontrado é maior que o procurado
            fseek(read_ptr, offset, SEEK_SET); //retoma a posicao que estava
            fim = ftell(read_ptr); //seta a nova posicao como fim
            //printf("Maior\n");
        }else if(resultado < 0){//valor encontrado é menor que o procurado
            fseek(read_ptr, offset, SEEK_SET); //retoma a posicao que estava
            inicio = ftell(read_ptr); //seta a nova posicao como fim
            //printf("Menor\n");
        }

        //strcpy(ultime, backup); //salva o registro nao alterado (backup) como o ultimo registro lido. Usado para comparar com o próximo lido e verificar se entrou em loop, assim indicando que não há o valor procurado no índice
        ultimo = offset;
    }

    return 0;
}

int main(){
    setlocale(LC_ALL, "");

    char escolha;
    time_t inicio, fim, total;
    printf("Bem vindo! Gostaria de iniciar a criacao do arquivo binario (S - sim, N - nao)? ");
    escolha = getchar();
    if (escolha == 'S'){
      printf("Aguarde enquanto o arquivo binario esta sendo gerado...\n");
      inicio = time(NULL);
      gera_arquivo_binario();
      fim = time(NULL);
      total = fim - inicio;
      printf("Arquivo gerado em %ld segundos\n", total);
    }
    else return 1;

    int partial_index = 0, ID_index = 0, lista_index = 0, arvore_index = 0, get = 1, key, key2, limit;
    node *root_tree;
    char instruction;
    root_tree = NULL;
    struct nodo_header *raiz_lista = NULL;
    char chave_busca[350], chave_busca2[350];

    while (1)
    {
      system("pause");
      if(get) getchar();
      get = 1;
      system("@cls||clear");
      printf("Escolha a operacao que deseja resolver:\n0 - Sair\n1 - Gerar indice parcial em arquivo\n2 - Gerar indice em arquivo com ordenacao pela coluna ID\n3 - Gerar indice em memoria com B+-Tree para armazenar agrupamentos de instalações\n4 - Gerar indice em memoria com multilista para armazenar agrupamentos de categorias\n5 - Consulta por chave primaria no arquivo bruto\n6 - Consulta por chave primaria no indice\n7 - Consulta por chave secundaria (ID) no indice\n8 - Consulta por numero exato de downloads\n9 - Consulta por faixa de downloads\nA - Consulta por apps de determinada categoria\nB - Consulta por existencia de chave em determinada categoria\n");
      escolha = getchar();
      switch (escolha)
      {
      case '0':
        return 0;
        break;
      case '1':
        printf("Aguarde enquanto o indice parcial esta sendo gerado...\n");
        partial_index = 1;
        inicio = time(NULL);
        gera_indice();
        fim = time(NULL);
        total = fim - inicio;
        printf("Indice parcial gerado em %ld segundos\n", total);
        break;
      case '2':
        printf("Aguarde enquanto a ordenacao por ID esta sendo gerada...\n");
        ID_index = 1;
        inicio = time(NULL);
        gera_indice_id();
        fim = time(NULL);
        total = fim - inicio;
        printf("Indice do ID gerado em %ld segundos\n", total);
        break;
      case '3':
        printf("Aguarde enquanto a B+-Tree por downloads esta sendo gerada...\n");
        arvore_index = 1;
        inicio = time(NULL);
        gera_arvore(&root_tree);
        fim = time(NULL);
        total = fim - inicio;
        printf("B+-Tree gerada em %ld segundos\n", total);
        break;
      case '4':
        printf("Aguarde enquanto a multilista de categorias esta sendo gerada...\n");
        lista_index = 1;
        inicio = time(NULL);
        gera_indice_lista(&raiz_lista);
        fim = time(NULL);
        total = fim - inicio;
        printf("Multilista gerada em %ld segundos\n", total);
        break;
      case '5':
        printf("Qual a chave de busca? ");
        getchar();
        gets(chave_busca);
        if(pesquisa_binaria(chave_busca) == 0)
          printf("\nEncontrado com sucesso\n");
        else
          printf("Puts");

        get = 0;
        break;
      case '6':
        if(!partial_index){
          printf("Gere indice parcial antes de consulta-lo\n");
          break;
        }
        printf("Qual a chave de busca? ");
        getchar();
        gets(chave_busca);
        if(pesquisa_binaria_indice_parcial(chave_busca) == 0)
          printf("\nEncontrado com sucesso\n");
        else
          printf("Puts");
    
        get = 0;
        break;
      case '7':
        if(!ID_index){
          printf("Gere indice por ID antes de consulta-lo\n");
          break;
        }
        printf("Qual a chave de busca? ");
        getchar();
        gets(chave_busca);
        if(pesquisa_binaria_indice_exaustivo(chave_busca) == 0)
          printf("\nEncontrado com sucesso\n");
        else
          printf("Puts");
    
        get = 0;
        break;
      case '8':
        if(!arvore_index){
          printf("Gere a arvore antes de consulta-la\n");
          break;
        }
        printf("Qual a chave (numero de downloads) de busca? ");
        scanf(" %d", &key);
        printf("Qual o limite de registros para a visualizacao? ");
        scanf(" %d", &limit);
        findAndPrint(root_tree, key, instruction = 'a', limit);
        break;
      case '9':
        if(!arvore_index){
          printf("Gere a arvore antes de consulta-la\n");
          break;
        }
        printf("Qual a chave (numero de downloads) minima de busca? ");
        scanf(" %d", &key);
        printf("Qual a chave (numero de downloads) maxima de busca? ");
        scanf(" %d", &key2);
        printf("Qual o limite de registros para a visualizacao? ");
        scanf(" %d", &limit);
        findAndPrintRange(root_tree, key, key2,instruction = 'a', limit);
        break;
      case 'A':
        if(!lista_index){
          printf("Gere a lista antes de consulta-la\n");
          break;
        }
        printf("Qual a chave (categoria) de busca? ");
        getchar();
        gets(chave_busca);
        printf("Qual o limite de registros para a visualizacao? ");
        scanf(" %d", &limit);
        consulta_categoria(raiz_lista, chave_busca, limit); //modo "write" escreve toda a multilista, todas outras palavras são uma busca para ela
          
        break;
      case 'B':
        if(!lista_index){
          printf("Gere a lista antes de consulta-la\n");
          break;
        }
        printf("Qual a chave (categoria) de busca? ");
        getchar();
        gets(chave_busca);
        printf("Qual a chave (app) de busca? ");
        gets(chave_busca2);
        consulta_chave_categoria(raiz_lista, chave_busca, chave_busca2);
        get = 0;
        break;
      default:
        break;
      } 
    }

    return 0;
}
