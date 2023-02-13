//
// AED, November 2022 (Tomás Oliveira e Silva)
//
// Second practical assignement (speed run)
//
// Place your student numbers and names here
//   N.Mec. 107658  Name: Maria Rafaela Alves Abrunhosa
//   N.Mec. 108193  Name: Matilde Moital Portugal Sampaio Teixeira 
//
// Do as much as you can
//   1) MANDATORY: complete the hash table code
//      *) hash_table_create
//      *) hash_table_grow
//      *) hash_table_free
//      *) find_word
//      +) add code to get some statistical data about the hash table (where?)
//   2) HIGHLY RECOMMENDED: build the graph (including union-find data) -- use the similar_words function...
//      *) find_representative
//      *) add_edge
//   3) RECOMMENDED: implement breadth-first search in the graph
//      *) breadh_first_search
//   4) RECOMMENDED: list all words belonginh to a connected component
//      *) breadh_first_search
//      *) list_connected_component
//   5) RECOMMENDED: find the shortest path between two words
//      *) breadh_first_search
//      *) path_finder
//      *) test the smallest path from bem to mal
//         [ 0] bem
//         [ 1] tem
//         [ 2] teu
//         [ 3] meu
//         [ 4] mau
//         [ 5] mal
//      *) find other interesting word ladders

//   6) OPTIONAL: compute the diameter of a connected component and list the longest word chain
//      *) breadh_first_search
//      *) connected_component_diameter
//   7) OPTIONAL: print some s about the graph
//      *) graph_info
//   8) OPTIONAL: test for memory leaks
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//
// static configuration
//

#define _max_word_size_  32


//
// data structures (SUGGESTION --- you may do it in a different way)
//

typedef struct adjacency_node_s  adjacency_node_t;
typedef struct hash_table_node_s hash_table_node_t;
typedef struct hash_table_s      hash_table_t;

struct adjacency_node_s
{
  adjacency_node_t *next;            // link to the next adjacency list node
  hash_table_node_t *vertex;         // the other vertex
};

struct hash_table_node_s
{
  // the hash table data
  char word[_max_word_size_];        // the word - keys
  hash_table_node_t *next;           // next hash table linked list node
  // the vertex data
  adjacency_node_t *head;            // head of the linked list of adjancency edges
  int visited;                       // visited status (while not in use, keep it at 0)
  hash_table_node_t *previous;       // breadth-first search parent
  // the union find data
  hash_table_node_t *representative; // the representative of the connected component this vertex belongs to -> if the representatives of their connected componments are the same, they are connected
  int number_of_vertices;            // number of vertices of the conected component (only correct for the representative of each connected component)
  int number_of_edges;               // number of edges of the conected component (only correct for the representative of each connected component)
};

struct hash_table_s
{
  unsigned int hash_table_size;      // the size of the hash table array
  unsigned int number_of_entries;    // the number of entries in the hash table
  unsigned int number_of_edges;      // number of edges (for information purposes only)
  hash_table_node_t **heads;         // the heads of the linked lists
};


//
// allocation and deallocation of linked list nodes (done)
//

static adjacency_node_t *allocate_adjacency_node(void)
{
  adjacency_node_t *node;

  node = (adjacency_node_t *)malloc(sizeof(adjacency_node_t));
  if(node == NULL)
  {
    fprintf(stderr,"allocate_adjacency_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_adjacency_node(adjacency_node_t *node)
{
  free(node);
}

static hash_table_node_t *allocate_hash_table_node(void)
{
  hash_table_node_t *node;

  node = (hash_table_node_t *)malloc(sizeof(hash_table_node_t));
  if(node == NULL)
  {
    fprintf(stderr,"allocate_hash_table_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_hash_table_node(hash_table_node_t *node)
{
  free(node);
}


//
// hash table stuff (mostly to be done)
//

static void print_table(hash_table_t *hash_table){

  hash_table->hash_table_size = 103;
  hash_table->number_of_entries = 0;
  hash_table->number_of_edges = 0;

  printf("Starting hash table\n");
  for (int i = 0; i < hash_table->hash_table_size; i++){
    if ( hash_table->heads[i] == NULL){
      printf("\t%i\t---\n", i);
    } else {
      printf("\t%i\t%s\n", i, hash_table->heads[i]->word);
    }
  }
}

unsigned int crc32(const char *str)
{
  static unsigned int table[256];
  unsigned int crc;
  if(table[1] == 0u) // do we need to initialize the table[] array?
  {
    unsigned int i,j;

    for(i = 0u;i < 256u;i++)
      for(table[i] = i,j = 0u;j < 8u;j++)
        if(table[i] & 1u)
          table[i] = (table[i] >> 1) ^ 0xAED00022u; // "magic" constant
        else
          table[i] >>= 1;
  }
  crc = 0xAED02022u; // initial value (chosen arbitrarily)
  while(*str != '\0')
    crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
  return crc;
}

static hash_table_t *hash_table_create(void)
{
  hash_table_t *hash_table;
  unsigned int i;

  hash_table = (hash_table_t *)malloc(sizeof(hash_table_t)); // malloc is used to dynamically allocate a single
  // large block of memory with the specified size
  if(hash_table == NULL)
  {
    fprintf(stderr,"create_hash_table: out of memory\n");
    exit(1);
  }
  //
  // complete this
  //
  hash_table->hash_table_size = 103;
  hash_table->number_of_entries = 0;
  hash_table->number_of_edges = 0;

  hash_table->heads = (hash_table_node_t **)malloc(sizeof(hash_table_node_t *)*hash_table->hash_table_size);
  if(hash_table->heads == NULL)
  {
    fprintf(stderr,"create_hash_table: out of memory\n");
    exit(1);
  }

  for (i = 0; i < hash_table->hash_table_size; i++) {
    hash_table->heads[i] = NULL;
  }
  return hash_table;
}

static void hash_table_grow(hash_table_t *hash_table) // hash_table grow what does that mean?
{
  hash_table_node_t **old_hash_table, *head_one, *next;
  unsigned int old_size, i, index;
  //
  // complete this
  //

  old_hash_table = hash_table->heads;
  old_size = hash_table->hash_table_size;

  hash_table->hash_table_size = hash_table->hash_table_size*2+1; // double the size of the hash table

  hash_table->number_of_entries = 0;
  hash_table->number_of_edges = 0;

  hash_table->heads = (hash_table_node_t **)malloc(sizeof(hash_table_node_t *)*hash_table->hash_table_size);
  if(hash_table->heads == NULL)
  {
    fprintf(stderr,"hash_table_grow: out of memory\n");
    exit(1);
  }

  // check for out of memory
  for (i = 0; i < hash_table->hash_table_size; i++) {
    hash_table->heads[i] = NULL;
  }

  for (i = 0; i < old_size; i++){
    //
    // complete this
    //
    head_one = old_hash_table[i];
    while(head_one != NULL){
      //
      // complete this
      //
      index = crc32(head_one->word) % hash_table->hash_table_size;
      next = head_one->next;
      // ...
      head_one->next = hash_table->heads[index];
      hash_table->heads[index] = head_one;
      head_one = next;
   }
  }
  free(old_hash_table);
  //o Bernardo mete aqui para definir as novas heads-> mas acho desnecessário, porque guardamos tudo na antiga
}

static void hash_table_free(hash_table_t *hash_table)
{
  hash_table_node_t *head_one, *next;
  unsigned int i;
  //
  // complete this
  //

  //Não percebi esta parte (TT)
  hash_table->hash_table_size = 103;
  hash_table->number_of_entries = 0;
  hash_table->number_of_edges = 0;

  for (i = 0; i < hash_table->hash_table_size; i++){
    //
    // complete this
    //
    head_one = hash_table->heads[i];
    while(head_one != NULL){
      adjacency_node_t *nodeadj = head_one->head, *nextAdjNode;
      while(nodeadj != NULL){
      // complete this
      nextAdjNode = nodeadj->next;
      free_adjacency_node(nodeadj);
      nodeadj=nextAdjNode;
      }
      next = head_one->next;
      free_hash_table_node(head_one);
      head_one = next;
   }
  }
  free(hash_table->heads);
  free(hash_table);
}

// find a word in the hash table
static hash_table_node_t *find_word(hash_table_t *hash_table, const char *word, int insert_if_not_found)
{
  hash_table_node_t *node;
  unsigned int index;

  index = crc32(word) % hash_table->hash_table_size;
  //
  // complete this
  //
  node = hash_table->heads[index];
  /* table_size = hash_table->hash_table_size; */

  while(node!=NULL) {
  if(strcmp(node->word, word)==0)
    return node;
  
  node=node->next;
  }
if(insert_if_not_found && strlen(word) < _max_word_size_)
  {
    node = allocate_hash_table_node();
    strncpy(node->word,word,_max_word_size_);
    node->representative = node;
    node->next = hash_table->heads[index];
    node->previous = NULL;
    node->number_of_edges = 0;
    node->number_of_vertices = 1;
    node-> visited = 0;
    node->head = NULL;
    hash_table->heads[index] = node;
    hash_table->number_of_entries++;
    if(hash_table->number_of_entries > hash_table->hash_table_size)
      hash_table_grow(hash_table);
    return node;
  }
return NULL;

}


//
// add edges to the word ladder graph (mostly do be done)
//

static hash_table_node_t *find_representative(hash_table_node_t *node)
{
  hash_table_node_t *representative2,*next_node;

  //
  // complete this
  //
  representative2 = node;
  while(representative2 != representative2->representative){
  // Loop through the linked list, looking for a node with the same ID == belongs to graph
  next_node = node->next;
  while (next_node != NULL)
  {
    if (next_node == representative2)
    {
      representative2 = next_node;
    }
    next_node = next_node->next;
  }
  representative2 = representative2->representative;
  }

  
  return representative2;
}

static void add_edge(hash_table_t *hash_table,hash_table_node_t *from,const char *word)
{
  hash_table_node_t *to_node,*representative1,*representative2;
  adjacency_node_t *link1, *link2;

  representative1 = find_representative(from);
  to_node = find_word(hash_table,word,0);
  
  if(to_node == NULL || to_node == from) 
    return;
  
  
  representative2 = find_representative(to_node);
  if (representative1 == representative2){
    representative1->number_of_edges++;
  } else {
    if (representative1->number_of_vertices < representative2->number_of_vertices)
    {
      representative1->representative = representative2;
      representative2->number_of_vertices += representative1->number_of_vertices;
      representative2->number_of_edges += (representative1->number_of_edges)+1;
      representative1->number_of_edges = 0;
      representative1->number_of_vertices = 0;
    }
    else
    {
      representative2->representative = representative1;
      representative1->number_of_vertices += representative2->number_of_vertices;
      representative1->number_of_edges += (representative2->number_of_edges)+1;
      representative2->number_of_edges = 0;
      representative1->number_of_vertices = 0;
    }
  }
  

  link1 = allocate_adjacency_node();
  link2 = allocate_adjacency_node();

  if(link1 == NULL || link2 == NULL)
  {
    fprintf(stderr,"Out of memory\n");
    exit(1);
  }
  
  link1->vertex = to_node;
  link2->next = from->head;
  from->head = link1;

  link2->vertex = from;
  link2->next = to_node->head;
  to_node->head = link2;
  
  hash_table->number_of_edges++;
  return;
}

//
// generates a list of similar words and calls the function add_edge for each one (done)
//
// man utf8 for details on the uft8 encoding
//

static void break_utf8_string(const char *word,int *individual_characters)
{
  int byte0,byte1;

  while(*word != '\0')
  {
    byte0 = (int)(*(word++)) & 0xFF;
    if(byte0 < 0x80)
      *(individual_characters++) = byte0; // plain ASCII character
    else
    {
      byte1 = (int)(*(word++)) & 0xFF;
      if((byte0 & 0b11100000) != 0b11000000 || (byte1 & 0b11000000) != 0b10000000)
      {
        fprintf(stderr,"break_utf8_string: unexpected UFT-8 character\n");
        exit(1);
      }
      *(individual_characters++) = ((byte0 & 0b00011111) << 6) | (byte1 & 0b00111111); // utf8 -> unicode
    }
  }
  *individual_characters = 0; // mark the end!
}

static void make_utf8_string(const int *individual_characters,char word[_max_word_size_])
{
  int code;

  while(*individual_characters != 0)
  {
    code = *(individual_characters++);
    if(code < 0x80)
      *(word++) = (char)code;
    else if(code < (1 << 11))
    { // unicode -> utf8
      *(word++) = 0b11000000 | (code >> 6);
      *(word++) = 0b10000000 | (code & 0b00111111);
    }
    else
    {
      fprintf(stderr,"make_utf8_string: unexpected UFT-8 character\n");
      exit(1);
    }
  }
  *word = '\0';  // mark the end
}

static void similar_words(hash_table_t *hash_table,hash_table_node_t *from)
{
  static const int valid_characters[] =
  { // unicode!
    0x2D,                                                                       // -
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,           // A B C D E F G H I J K L M
    0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,           // N O P Q R S T U V W X Y Z
    0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,           // a b c d e f g h i j k l m
    0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,           // n o p q r s t u v w x y z
    0xC1,0xC2,0xC9,0xCD,0xD3,0xDA,                                              // Á Â É Í Ó Ú
    0xE0,0xE1,0xE2,0xE3,0xE7,0xE8,0xE9,0xEA,0xED,0xEE,0xF3,0xF4,0xF5,0xFA,0xFC, // à á â ã ç è é ê í î ó ô õ ú ü
    0
  };
  int i,j,k,individual_characters[_max_word_size_];
  char new_word[2 * _max_word_size_];

  break_utf8_string(from->word,individual_characters);
  for(i = 0;individual_characters[i] != 0;i++)
  {
    k = individual_characters[i];
    for(j = 0;valid_characters[j] != 0;j++)
    {
      individual_characters[i] = valid_characters[j];
      make_utf8_string(individual_characters,new_word);
      // avoid duplicate cases
      if(strcmp(new_word,from->word) > 0)
        add_edge(hash_table,from,new_word);
    }
    individual_characters[i] = k;
  }
}


//
// breadth-first search (to be done)
//
// returns the number of vertices visited; if the last one is goal, following the previous links gives the shortest path between goal and origin
//

static int breadh_first_search(int maximum_number_of_vertices,hash_table_node_t *list_of_vertices[],hash_table_node_t *origin,hash_table_node_t *goal)
{  //
  // complete this
  //
  list_of_vertices[0]=origin;
  origin->visited = 1;
  int n=0;

  int frente = 0;
  while(n<maximum_number_of_vertices && list_of_vertices[n] != goal) {
    /* hash_table_node_t node2 = list_of_vertices[n]; */
    for(adjacency_node_t *adjency_node = list_of_vertices[frente]->head; adjency_node != NULL; adjency_node = adjency_node->next ){
      if(adjency_node->vertex->visited==1){
        continue;
      }
      frente ++;
      adjency_node->vertex->visited=1;
      list_of_vertices[frente]=adjency_node->vertex;
      adjency_node->vertex->previous=list_of_vertices[frente];
    }
    n++;
  }
  return -1;
}


//
// list all vertices belonging to a connected component (complete this)
//

static void list_connected_component(hash_table_t *hash_table,const char *word)
{
  //
  // complete this
  //
  hash_table_node_t *find_node, *rep_node;
  int n, max_vertices_number;
  hash_table_node_t **list_of_vertices;

  find_node = find_word(hash_table,word,0);
  
  if(find_node == NULL)
  {
    printf("list_connected_component: word not found\n");
    return;
  }
  
  rep_node = find_representative(find_node);
  max_vertices_number = rep_node->number_of_vertices;
  list_of_vertices = (hash_table_node_t **)malloc(max_vertices_number * sizeof(hash_table_node_t *));
  
  if(list_of_vertices == NULL)
  {
    fprintf(stderr,"list_connected_component: out of memory\n");
    exit(1);
  }

  n = breadh_first_search(max_vertices_number,list_of_vertices,find_node,NULL);
  
  for(int i = 0;i < n;i++){
    printf("%d: %s\n",list_of_vertices[i]->word);
  }
  free(list_of_vertices);
}


//
// compute the diameter of a connected component (optional)
//

static int largest_diameter;
static hash_table_node_t **largest_diameter_example;

/* static int connected_component_diameter(hash_table_node_t *node)
{
  int diameter;

  //
  // complete this
  //
  return diameter;
} */


//
// find the shortest path from a given word to another given word (to be done)
//

static void path_finder(hash_table_t *hash_table,const char *from_word,const char *to_word)
{
  //
  // complete this
  //
  hash_table_node_t *nodeinit, *repinit, *nodefin, *repfin;
  nodeinit = find_word(hash_table, from_word, 0);
  nodefin = find_word(hash_table, to_word, 0);
  
  if(nodeinit == NULL || nodefin == NULL){
    printf("Word(s) not in dictionary\n");
    return;
  }

  repinit = find_representative(nodeinit);
  repfin = find_representative(nodefin);

  if(repinit != repfin){
    printf("Words not in the same connected component (No path between them)\n");
    return;
  }

  hash_table_node_t **list_of_vertices = malloc(sizeof(hash_table_node_t *) * repinit->number_of_vertices);

  if (list_of_vertices == NULL)
  {
    fprintf(stderr, "path_finder: malloc failed\n");
    exit(1);
  }

  int final_index = breadh_first_search(repinit->number_of_vertices, list_of_vertices, nodefin, nodeinit);
  int final_index2=final_index-1; //array começa na posição 0
  hash_table_node_t *pointer = list_of_vertices[final_index2];
  
  while (pointer != NULL)
  {
    printf("%d: %s \n", pointer->word);
    pointer = pointer->previous;
  }
  
  free(list_of_vertices);
}


//
// some graph information (optional)
//

 /* static void graph_info(hash_table_t *hash_table)
{
  //
  // complete this
  //
} */

static void print_list(hash_table_t *hash_table){
  for (size_t i = 0; i < hash_table->hash_table_size; i++)
  {
    hash_table_node_t *p = hash_table->heads[i];
    if(p == NULL)
      continue;
    while (p != NULL)
    {
      printf("%s ", p->word);
      p = p->next;
    }
    printf("\n");
  }
  return;
}

//
// main program
//

int main(int argc,char **argv)
{
  char word[100],from[100],to[100];
  hash_table_t *hash_table;
  hash_table_node_t *node;
  unsigned int i;
  int command;
  FILE *fp;

  // initialize hash table
  hash_table = hash_table_create();
  // read words
  fp = fopen((argc < 2) ? "wordlist-big-latest.txt" : argv[1],"rb");
  if(fp == NULL)
  {
    fprintf(stderr,"main: unable to open the words file\n");
    exit(1);
  }
  while(fscanf(fp,"%99s",word) == 1)
    (void)find_word(hash_table,word,1);
    
  fclose(fp);
  /* print_table(hash_table); */
  // find all similar words
  for(i = 0u;i < hash_table->hash_table_size;i++)
    for(node = hash_table->heads[i];node != NULL;node = node->next)
      similar_words(hash_table,node);
/*   graph_info(hash_table); */
  // ask what to do
    print_list(hash_table);
  for(;;)
  {
    fprintf(stderr,"Your wish is my command:\n");
    fprintf(stderr,"  1 WORD       (list the connected component WORD belongs to)\n");
    fprintf(stderr,"  2 FROM TO    (list the shortest path from FROM to TO)\n");
    fprintf(stderr,"  3            (terminate)\n");
    fprintf(stderr,"> ");
    if(scanf("%99s",word) != 1)
      break;
    command = atoi(word);
    if(command == 1)
    {
      if(scanf("%99s",word) != 1)
        break;
      
      list_connected_component(hash_table,word);
    }
    else if(command == 2)
    {
      if(scanf("%99s",from) != 1)
        break;
      if(scanf("%99s",to) != 1)
        break;
      path_finder(hash_table,from,to);
    }
    else if(command == 3)
      break;
  }
  // clean up
  hash_table_free(hash_table);
  return 0;
}