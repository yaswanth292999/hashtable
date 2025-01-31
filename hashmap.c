#include <stdio.h>
#include <string.h>
#include <stdlib.h>  

/*
In function parameters, arrays automatically decay into pointers.
This means char key[] is actually interpreted as char *key.
The compiler sees char key[] as char *key, so using char[] inside function parameters is not allowed.
*/
struct KVPair{

 char *key;
 char *value;

};

struct HashTable{
    
    int size;
   struct KVPair *items;

};

int computeHash(char *key)
{

    int strLen = strlen(key);
    int sumOfAscii = 0;

    for (int i = 0; i < strLen; i++)
    {
        int asciiValue = (int)key[i];
        sumOfAscii += asciiValue;
    }

    return sumOfAscii;
}


int insert(struct HashTable* hashmap,char *key, char* value){

    //compute hash
    int hashValue=computeHash(key);
    int hashIndex=(hashValue)%(hashmap->size);
    printf("\nInserted at value %d \n",hashIndex);
    //create <Key,Value> pairs
    struct KVPair kvPair={key,value};
    hashmap->items[hashIndex]=kvPair;

    return 1;
}

int get(struct HashTable* hashTable,char *key){

    //compute hash and hashIndex
    printf("\nGetting the value...for %s\n",key);
    int hashValue=computeHash(key);
    int hashIndex=(hashValue)%hashTable->size;
    if(strcmp(hashTable->items[hashIndex].key,key)==0){
      printf("Value for %s is %s",key,hashTable->items[hashIndex].value); 
      return 1;
    }

    return 0;

}

int main()
{

    char myName[] = "yaswanth";
    char myName2[]="Honeymoon";
    char myValue[]="coder";

   struct HashTable hashTable;

   hashTable.size=10;

   hashTable.items= malloc(sizeof(struct KVPair)*(hashTable.size));
    
    insert(&hashTable,myName,myValue);

    printf("%d",get(&hashTable,myName2));



}
