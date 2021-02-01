#include <stdio.h>
#include <stdlib.h>

#define min(a,b) (a<b ? a:b)

struct Graph{
    int numheads;
    struct store** heads;
};

struct store{
    int storeNumber; 
    int scc_id;
    int low;
    int instack;           
    int discoveryTime;    
    struct edge* adjVertex;
    int root;
    int min;
    int* connectionsSCC;
};

struct Stack{
    int number;
    int top;
    int size;
    int *elements;
};

struct edge{
    int vNumber;
    int flag;
    struct edge* nextEdge;
};


struct connection{
    int source;
    int destiny;
};

struct Stack *stack_aux;
static struct Graph *graph;
struct connection* newConnection(int source,int destiny);

/* ==================================================================== */

int isEmpty();
void initStack(int capacity);
void push(int store);
int pop();

void initGraph(int n);
void insertEdge(int source,int destiny);
struct store* newStore(int number);
void printGraph();

void SCC_Tarjan(int size);
void Tarjan_Visit(int n);

void IdentifyConnections();
void MakeConnectionsArray();

void mergesortSource(int i,int j);
void mergesortDestiny(int i,int j);
void mergeSource(int i1,int j1,int i2,int j2);
void mergeDestiny(int i1,int j1,int i2,int j2);


int time; /*Variável para o tempo de descoberta*/
int nScc;
int idAtual;
int min;

int connections;
struct connection** connectionsArray;

/* ==================================================================== */


/*************************************************************************
== STACK ============================================================== */

int isEmpty(){   
    return stack_aux->number == 0;  
}

void initStack(int capacity){
    stack_aux = (struct Stack*) malloc(sizeof(struct Stack));
    stack_aux->top =-1;
    stack_aux->number = 0;
    stack_aux->size = capacity;
    stack_aux->elements=(int*)malloc(sizeof(int)*capacity);
}

void push(int store){
    stack_aux->top = store;
    stack_aux->elements[stack_aux->number]=store;
    stack_aux->number++;
}

int pop(){

    int valueStoreNumber;

    if (isEmpty()) {
        printf("isEMPTY!\n");
        return EXIT_FAILURE;
    }

    valueStoreNumber = graph->heads[stack_aux->top - 1]->storeNumber;

    if (stack_aux->number-2 >= 0)
        stack_aux->top=stack_aux->elements[stack_aux->number-2]; 
    stack_aux->number--;
    return valueStoreNumber;
}

/**************************************************************************/

/*************************************************************************
== GRAPH ============================================================== */

void initGraph(int n){
    int i;
    graph=(struct Graph*)malloc(sizeof(struct Graph));
    graph->heads=(struct store**)malloc(sizeof(struct store*)*n);

    for(i = 0; i < n; i++){
        graph->heads[i]=newStore(i+1);
    }
}

struct connection* newConnection(int source, int destiny){
    struct connection* new=(struct connection*)malloc(sizeof(struct connection));
    new->source=source;
    new->destiny=destiny;
    return new;
}

struct store* newStore(int number){
    struct store* new = (struct store*)malloc(sizeof(struct store));
    new->storeNumber = number;
    new->scc_id = -1;
    new->adjVertex = NULL;
    new->discoveryTime = -1;
    new->instack = 0;
    new->low = -1;
    new->root = -1;
    new->min = -1;
    return new;
}

struct edge* newEdge(int number){
    struct edge* new = (struct edge*)malloc(sizeof(struct edge));
    new->vNumber = number;
    new->nextEdge = NULL;
    new->flag = 0;

    return new;
}

void insertEdge(int source,int destiny){
    struct edge* newNode = newEdge(destiny);
    newNode->nextEdge = graph->heads[source-1]->adjVertex;
    graph->heads[source-1]->adjVertex = newNode;
}

void SCC_Tarjan(int size){
    int i,j,m;
    
    for(i = 0; i < size; i++){
        if(graph->heads[i]->discoveryTime == -1){
            Tarjan_Visit(i);
        }
    }

    for(j=0;j<graph->numheads;j++) {
        if(graph->heads[j]->storeNumber == graph->heads[j]->root){
            graph->heads[j]->connectionsSCC = malloc(nScc*sizeof*graph->heads[j]->connectionsSCC);
            for(m = 0; m < nScc; m++) {
                graph->heads[j]->connectionsSCC[m]=0;
            }
        }
    }
}

void Tarjan_Visit(int n){

    int popValue;

    graph->heads[n]->discoveryTime = time;
    graph->heads[n]->low = time;
    time++;
    push(graph->heads[n]->storeNumber);
    graph->heads[n]->instack = 1;
    
    struct edge* ptr = graph->heads[n]->adjVertex;

    while(ptr != NULL){
        
        int number = ptr->vNumber;

        if(graph->heads[number-1]->discoveryTime == -1 || graph->heads[number-1]->instack == 1){
            if(graph->heads[number-1]->discoveryTime == -1 ){
                Tarjan_Visit(number-1);
            }
            graph->heads[n]->low = min(graph->heads[n]->low,graph->heads[number- 1]->low);
        }

        ptr = ptr->nextEdge;
    }
    

    if(graph->heads[n]->discoveryTime == graph->heads[n]->low){

        nScc++;
        int minTemp = graph->heads[n]->storeNumber;

        do {
            graph->heads[stack_aux->top-1]->instack = 0;
            popValue = pop();
            if(popValue < minTemp){
                minTemp = popValue;
            }
            graph->heads[popValue- 1]->scc_id = idAtual;
            graph->heads[popValue- 1]->root = graph->heads[n]->storeNumber;
        }
        while (popValue != graph->heads[n]->storeNumber);
        
        graph->heads[n]->min = minTemp;
        idAtual++;
    }

}

void IdentifyConnections(){
    int i;
    struct edge*ptr;
    for(i = 0; i < graph->numheads; i++){
        ptr=graph->heads[i]->adjVertex;

        while (ptr != NULL){

            if (graph->heads[ptr->vNumber-1]->scc_id != graph->heads[i]->scc_id &&
            graph->heads[graph->heads[i]->root-1]->connectionsSCC[graph->heads[ptr->vNumber-1]->scc_id] != 1) {

                ptr->flag=1;
                graph->heads[graph->heads[i]->root-1]->connectionsSCC[graph->heads[ptr->vNumber-1]->scc_id]=1;
                connections++;
            }

            ptr=ptr->nextEdge;

        }
    }
}

void MakeConnectionsArray(){
    int i,j,indiceArray=0;
    struct edge* ptr;

    connectionsArray=(struct connection**)malloc(connections*sizeof(struct connection*));
    for(j = 0; j < connections; j++){
        connectionsArray[j]=NULL;
    }

    for(i = 0; i < graph->numheads; i++){
        ptr = graph->heads[i]->adjVertex;
        while (ptr != NULL){
            if(ptr->flag == 1){
                connectionsArray[indiceArray] = newConnection(graph->heads[graph->heads[i]->root-1]->min,graph->heads[graph->heads[ptr->vNumber-1]->root-1]->min);
                indiceArray++;
            }
            ptr = ptr->nextEdge;
        }
    }

    mergesortDestiny(0,connections-1);
    mergesortSource(0,connections-1);
}


/* ==================================================================== */
void mergesortSource(int i,int j)
{
    int mid;
        
    if(i < j)
    {
        mid = (i+j)/2;
        mergesortSource(i,mid);
        mergesortSource(mid+1,j);
        mergeSource(i,mid,mid+1,j);
    }
}
 
void mergeSource(int i1,int j1,int i2,int j2)
{
    struct connection* temp[connections];
    int i,j,k;
    i=i1;    
    j=i2;    
    k=0;
    
    while(i<=j1 && j<=j2)
    {
        if(connectionsArray[i]->source<connectionsArray[j]->source)
            temp[k++]=connectionsArray[i++];
        else
            temp[k++]=connectionsArray[j++];
    }
    
    while(i<=j1)
        temp[k++]=connectionsArray[i++];
        
    while(j<=j2)
        temp[k++]=connectionsArray[j++];
        
    for(i=i1,j=0;i<=j2;i++,j++)
        connectionsArray[i]=temp[j];
}

void mergesortDestiny(int i,int j)
{
    int mid;
        
    if(i < j)
    {
        mid=(i+j)/2;
        mergesortDestiny(i,mid); 
        mergesortDestiny(mid+1,j);
        mergeDestiny(i,mid,mid+1,j);
    }
}
 
void mergeDestiny(int i1,int j1,int i2,int j2)
{
    struct connection* temp[connections];
    int i,j,k;
    i=i1;    
    j=i2;
    k=0;
    
    while(i<=j1 && j<=j2)
    {
        if(connectionsArray[i]->destiny>connectionsArray[j]->destiny)
            temp[k++]=connectionsArray[i++];
        else
            temp[k++]=connectionsArray[j++];
    }
    
    while(i<=j1)
        temp[k++]=connectionsArray[i++];
        
    while(j<=j2)
        temp[k++]=connectionsArray[j++];
        
    for(i=i1,j=0;i<=j2;i++,j++)
        connectionsArray[i]=temp[j];
}


/* ==================================================================== */
int main(){

    int nStores,nEdges,i,source,destiny;
    nScc=0,time=0,connections=0,min=0;
    scanf("%d\n",&nStores);
    scanf("%d\n",&nEdges);
    initGraph(nStores);
    initStack(nStores);

    graph->numheads=nStores;

    for(i = 0; i < nEdges; i++){
        scanf("%d %d", &source, &destiny);
        insertEdge(source, destiny);
    }

    SCC_Tarjan(nStores);

    IdentifyConnections();
    MakeConnectionsArray();

    printf("%d\n",nScc);
    printf("%d\n",connections);

    for(i = 0; i < connections; i++){
        printf("%d %d\n",connectionsArray[i]->source,connectionsArray[i]->destiny);
    }

    return EXIT_SUCCESS;
}