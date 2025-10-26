#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// TO DO :
    // CONVERT THESE TO INT 8 16 32 64 RESPECTIVE WIDTHS


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



int CAPACITY=10;
int SIZE=0;

int hash(char *s){
    int sumAscii=0;
    for(int i=0;i<strlen(s);i++){
        int charAscii=(int)s[i];
        sumAscii+=charAscii;   
    }
    return sumAscii;
}

int addKey(char *key, char *value,kvPair **hashtable){
    int computedHash=hash(key);
    int index=computedHash%CAPACITY;

    if(hashtable[index]!=NULL){
        printf("Collison detected opinon rejected for %s\n",key);
        return -1;
    }

    kvPair *pair=malloc(sizeof(kvPair));
    pair->key=key;
    pair->value=value;
    hashtable[index]=pair;
    SIZE++;

    return index;

}

int removeKey(char *key,kvPair **hashtable){
    int computedHash=hash(key);
    int index=computedHash%CAPACITY;
    if(hashtable[index]==NULL){
        printf("The following key %s is not present in the hashtable\n",key);
        return -1;
    }
    if(strcmp(hashtable[index]->key,key)==0){
        free(hashtable[index]);
        hashtable[index]=NULL;
       
    }
    return 1;
}

int main(){
   kvPair **hashtable;

   hashtable = malloc(CAPACITY * sizeof(kvPair*));
    char *keys[]={"Yaswanth","Gandhi","Nagalakshmi","Skyfall","Broo","SAF","ADS","ADSASD"};
    char *values[]={"ezrawolf","mom","dad","ads","ads","asdeaw","adsasd","adsdsa","ewaweq"};
    for(int i=0;i<8;i++){
        int index=addKey(keys[i],values[i],hashtable);
    }

  
    for(int index=0;index<CAPACITY;index++){
        if(hashtable[index]!=NULL){
            printf("The index out there is %s\n",hashtable[index]->key);
        }
    }

        for(int i=0;i<8;i++){
        int index=removeKey(keys[i],hashtable);
    }

        for(int index=0;index<CAPACITY;index++){
        if(hashtable[index]!=NULL){
            printf("The index out there is %s\n",hashtable[index]->key);
        }
        if(hashtable[index]==NULL){
            printf("The index is null %d \n",index);
        }
    }
     
}