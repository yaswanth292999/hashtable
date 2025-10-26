#include <stdio.h>
#include <stdlib.h>
#include <string.h>



    
/* TO DO

CONVERT THESE TO INT 8 16 32 64 RESPECTIVE WIDTHS
CONVERT INT TO DOUBLE ETC FOR CAPACITY NO OF ELEMENTS ETC
Make accessing elements as cpu friendly as possible

*/



//Basic hash function which will sum ascii values of the character


/*
you create a struct with key and value;
array will have pointers with this type;
for now let it point to this directly instead of linked list or any kind of optimisation
now add/update should double copy and delte the previous array
remove should do the same
*/

typedef struct kvPair {
    char *key;
    char *value;
} kvPair;



int capacity=10;
int noOfElements=0;
float resizeTreshold=0.7;


float loadFactor(){
    return noOfElements/capacity
}

int hash(char *s){
    int sumAscii=0;
    for(int i=0;i<strlen(s);i++){
        int charAscii=(int)s[i];
        sumAscii+=charAscii;   
    }
    return sumAscii;
}




kvPair ** resizeHashmap(kvPair **oldHashmap){

    //double the capacity of the hashmap
    int newCapacity=capacity*2;

    kvPair **newHashmap = malloc(newCapacity * sizeof(kvPair*) );

    for(int index=0;index<capacity;index++){
        kvPair *pair=oldHashmap[index];
        addKey(pair->key,newHashmap);
    }

    capacity=newCapacity;
    free(oldHashmap);

    return newHashmap

    //copy the old elements into new hashmap and resize
}


int getSlotLinearProbing(char *key,kvPair **hashmap){
    int computedHash=hash(key);
    int index=computedHash%capacity;
    int initalIndex=index;

    do{
        index++;
    } while(hashmap[index]!=NULL&&initalIndex!=index){
        index++;
    }

    if(hashmap[index]!=NULL){
        return -1;
    }

    return index;
}

int getIndexByKey(char *key,kvPair **hashmap){

    int computedHash=hash(key);
    int index=computedHash%capacity;

    while(strcmp(hashmap[index]->key,key)!==0 && index!=NULL){
        index++;
    }

    if(index==NULL) return -1;

    return index;

}


int addKey(char *key, char *value,kvPair **hashtable){

    float loadFactor=loadFactor();

    //trigger a resize when loadfactor is greater than treshold

    if(loadFactor>=resizeTreshold){
        hashtable=resizeHashmap(hashtable);

    } 

    int index=getSlotLinearProbing(key,hashtable)

    if(index==-1){
        printf("Add failed as there are no slots left in hashmap via Linear probing");
        return -1
    }

    kvPair *pair=malloc(sizeof(kvPair));
    pair->key=key;
    pair->value=value;
    hashtable[index]=pair;
    noOfElements++;

    return index;

}

int removeKey(char *key,kvPair **hashtable){
    int index=getIndexByKey(key,hashtable)

    if(index!=-1){
        free(hashtable[index]);
        hashtable[index]=NULL;   
    }
    else{
        printf("Key %s is not present to be removed",key);
    }

    return 1;
}

char * getValue(char *key, kvPair **hashtable){
    int index=getIndexByKey(key);

    if(index==-1) return NULL;

    return hashtable[index]->value;

}

int main(){
   kvPair **hashtable;

   hashtable = malloc(capacity * sizeof(kvPair*));
    char *keys[]={"Yaswanth","Gandhi","Nagalakshmi","Skyfall","Broo","SAF","ADS","ADSASD"};
    char *values[]={"ezrawolf","mom","dad","ads","ads","asdeaw","adsasd","adsdsa","ewaweq"};
    for(int i=0;i<8;i++){
        int index=addKey(keys[i],values[i],hashtable);
    }

  
    for(int index=0;index<capacity;index++){
        if(hashtable[index]!=NULL){
            printf("The index out there is %s\n",hashtable[index]->key);
        }
    }

        for(int i=0;i<8;i++){
        int index=removeKey(keys[i],hashtable);
    }

        for(int index=0;index<capacity;index++){
        if(hashtable[index]!=NULL){
            printf("The index out there is %s\n",hashtable[index]->key);
        }
        if(hashtable[index]==NULL){
            printf("The index is null %d \n",index);
        }
    }
     
}