#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define INFINITE 10000000


struct Pixel{
   int lp;                   /*peso de primeiro plano*/
   int cp;                   /*peso de cenário*/
   struct Edge* parent;      /*ponteiro para o antecessor*/
   struct Edge* adjPixels;   /*vertices adjacentes*/
};

struct Edge{
    int origin;              
    int destiny;
    int crValue;            /*capacidade residual do arco*/
    struct Edge* next;
};

struct Queue{
    int front, rear, size;
    int capacity;
    int* list;
};

struct Pixel **pixels; /*grafo*/
int dimTotal;          /*numero total de pixeis*/
struct Queue* queue;
int *visited;
int max_flow=0;

void getLp(int m, int n);
void getCp(int m, int n);
void initPixel(int indice);
void initAll();
void initializeGraph(int m, int n);
void getHorizontalConnections(int m, int n);
void getVerticalConnections(int m, int n);
struct Edge* initEdge(int origin, int destiny);
void InsertEdge(int u, int v,int capacity);
struct Queue* createQueue(int capacity);
void resetQueue();
int isEmpty();
int isFull();
int removePixelQ();
void insertQ(int pixelNumber);
struct Edge* getEdge(int u, int v);
void updateCr(int u, int v, int value);
int BFS();
int EdmondsKarp();

int BFS(){
    struct Edge* x;
    int u,v;
    memset(visited,0,dimTotal*sizeof(int));
    
    resetQueue();

    insertQ(0);

    while(isEmpty()==0){
    
        u = removePixelQ();
        x=pixels[u]->adjPixels;

        while(x!=NULL){
            v=x->destiny;

            if(visited[v]==0 && x->crValue>0){
                insertQ(v);
                pixels[v]->parent=x;
                visited[v]=1;

                if(v == dimTotal-1){  /* v = t */
                    return 1;
                }

            }

            x=x->next;
            
        }

    }
    return 0;   
}

int EdmondsKarp(){
    struct Edge* x;
    int v=dimTotal-1;

    queue=createQueue(dimTotal);
    visited = (int*)malloc(sizeof(int)*dimTotal);
    
    while (BFS()==1){
        int pathFlow=INFINITE;
  
       for(v=dimTotal-1; v!=0;v=x->origin){
           x=pixels[v]->parent;
           pathFlow=MIN(pathFlow,x->crValue);
       }

       for(v=dimTotal-1; v!=0;v=x->origin){
           x=pixels[v]->parent;
           x->crValue-=pathFlow;
            x=pixels[v]->parent;
       }

        max_flow += pathFlow;

    }

    return max_flow;
}
 
struct Queue* createQueue(int capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0; 
    queue->rear = - 1;  
    queue->list = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}

void resetQueue(){
    queue->front=0;
    queue->rear=-1;
    queue->size=0;
}

int isEmpty(){
    return queue->size==0;
}

int isFull(){
    return queue->size==queue->capacity;
}

int removePixelQ() {
  
   int pixelNumber=queue->list[queue->front++];

   if(queue->front==queue->capacity){
       queue->front=0;
   }

   queue->size = queue->size - 1;
   return pixelNumber;
}

void insertQ(int pixelNumber) {
    if(isFull()==1){
        printf("Queue cheia\n");
    }

    if(queue->rear == queue->capacity-1){
        queue->rear=-1;
    }

    queue->list[++queue->rear]=pixelNumber;
    queue->size=queue->size+1;   
}

void InsertEdge(int u, int v,int capacity){
    struct Edge* edge;

    edge=initEdge(u,v);
    edge->crValue=capacity;
    edge->next=pixels[u]->adjPixels;
    pixels[u]->adjPixels=edge;
}

void initPixel(int indice){
    struct Pixel* new = (struct Pixel*)malloc(sizeof(struct Pixel));
    new->lp= -1;
    new->cp=-1;
    new->parent=NULL;
    new->adjPixels=NULL;
    pixels[indice]=new;
}

struct Edge* initEdge(int origin, int destiny){
    struct Edge* new=(struct Edge*)malloc(sizeof(struct Edge));
    new->origin=origin;
    new->destiny=destiny;
    new->next=NULL;
    return new;
}

void initAll(){
    int i;
    for(i=0; i<dimTotal;i++){
        initPixel(i);
    }
}

void getCp(int m, int n){
    int i,j,value, indice=1;

    for(i=0;i<m;i++){
        for(j=0;j<n;j++){
            scanf("%d ",&value);
            pixels[indice]->cp=value;
            indice++;
        }
    }
}

void getLp(int m, int n){
    int i,j,value, indice=1;

    for(i=0;i<m;i++){
        for(j=0;j<n;j++){
            scanf("%d ",&value);
            pixels[indice]->lp=value;
            indice++;
        }
    }
}

void getHorizontalConnections(int m, int n){ 
    int i,j,value;
    int u=1, v=2;

    for(i=0;i<m;i++){
        for(j=0;j<n-1;j++){
            scanf("%d ",&value);
            
            if(value!=0){
                InsertEdge(u,v,value);
                InsertEdge(v,u,value);
            }

            u++;
            v++;
        }
        u++;
        v++;
    }
}



void getVerticalConnections(int m, int n){ 
    int i,j,value;
    int u=1, v=n+1;

    for(i=0;i<m-1;i++){
        for(j=0;j<n;j++){
            scanf("%d ",&value);

            if(value!=0){
                InsertEdge(u,v,value);
                InsertEdge(v,u,value);
            }

            u++;
            v++;
        }
    }
}

void initializeGraph(int m, int n){ 
    int j;
    int value;

    for(j=1; j<dimTotal-1;j++){   /*adicionar arcos de s para j e arcos de j para t (j==pixel)*/

        value=MIN(pixels[j]->lp,pixels[j]->cp);
        max_flow += value;

        if(pixels[j]->lp - value!=0){
            InsertEdge(0,j,pixels[j]->lp - value);
        }
        
        if(pixels[j]->cp - value!=0){
            InsertEdge(j,dimTotal-1,pixels[j]->cp - value);  /*adiciona adjacencia t ao pixel j*/
        }
    }
}

struct Edge* getEdge(int u, int v){
    struct Edge* x;
    x=pixels[u]->adjPixels;
    while(x!=NULL){
        if(x->destiny==v){
            break;
        }
        x=x->next;
    }

    return x;
}

void updateCr(int u, int v, int value){
    struct Edge* x;
    x=pixels[u]->adjPixels;
    while(x!=NULL){
        if(x->destiny==v){
            x->crValue-=value;
        }
        x=x->next;
    }
}


/******************************************************************/
int main(){
    int m,n,flow;
    int mul=0;
    int indice;
    scanf("%d %d\n",&m,&n);
    scanf("\n");
    dimTotal=m*n+2;

    pixels=(struct Pixel **)malloc(dimTotal*sizeof(struct Pixel*));
    initAll();

    getLp(m, n);
    getCp(m, n);

    initializeGraph(m, n);
    
    scanf("\n");

    getHorizontalConnections(m,n);

    scanf("\n");

    getVerticalConnections(m,n);

    flow=EdmondsKarp();

    printf("%d\n\n",flow);

    for(indice=1;indice<=dimTotal-2;indice++){

        if(visited[indice]!=0){
            
            if(mul==n){
                printf("\n");
                mul=0;
            }
            printf("C ");

            mul++;
        }

        else{
            
            if(mul==n){
                printf("\n");
                mul=0;
            }
            printf("P ");

            mul++;
            
        }
    }
    printf("\n");

    return 0;
}