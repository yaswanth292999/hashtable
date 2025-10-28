#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TO DO

refactor the code better. add key shouldnt return hashmap address. very bad;
CONVERT THESE TO INT 8 16 32 64 RESPECTIVE WIDTHS
CONVERT INT TO DOUBLE ETC FOR CAPACITY NO OF ELEMENTS ETC
Make accessing elements as cpu friendly as possible
ALL you need to do is free pointers not again allocate memory

*/

// Basic hash function which will sum ascii values of the character

/*
you create a struct with key and value;
array will have pointers with this type;
for now let it point to this directly instead of linked list or any kind of optimisation
now add/update should double copy and delte the previous array
remove should do the same
*/

typedef struct kvPair
{
    char *key;
    char *value;
} kvPair;

kvPair **hashtable;
kvPair **addKey(char *key, char *value, kvPair **hashtable);
kvPair **resizeHashmap(kvPair **oldHashmap);

int capacity = 4;
int noOfElements = 0;
float resizeTreshold = 0.7;

float loadFactor()
{
    float loadFactorValue = (float)noOfElements / capacity;
    printf("No of elements is %d, capacity is %d, loadfactor is %f \n", noOfElements, capacity, loadFactorValue);
    return loadFactorValue;
}

int hash(char *s)
{
    int sumAscii = 0;
    for (int i = 0; i < strlen(s); i++)
    {
        int charAscii = (int)s[i];
        sumAscii += charAscii;
    }
    return sumAscii;
}

int getSlotLinearProbing(char *key, kvPair **hashmap)
{
    int computedHash = hash(key);
    int index = computedHash % capacity;
    int initalIndex = index;

    if (hashmap[initalIndex] == NULL)
        return initalIndex;

    do
    {
        index++;
        if (index >= capacity)
        {
            index = 0;
        }
    } while (hashmap[index] != NULL && initalIndex != index);

    if (hashmap[index] != NULL)
    {
        printf("Slot not found during linear probing");
        return -1;
    }

    return index;
}

int getIndexByKey(char *key, kvPair **hashmap)
{

    int computedHash = hash(key);
    int index = computedHash % capacity;

    if (hashmap[index] == NULL)
    {
        return -1;
    }
    else if (strcmp(hashmap[index]->key, key) == 0)
    {
        return index;
    }

    int initalIndex = index;

    do
    {
        index++;
        if (index >= capacity)
        {
            index = 0;
        }
    } while (strcmp(hashmap[index]->key, key) != 0 && hashmap[index] != NULL);

    if (hashmap[index] == NULL || strcmp(hashmap[index]->key, key) != 0)
    {
        return -1;
    }

    return index;
}

kvPair **resizeHashmap(kvPair **oldHashmap)
{
    // double the capacity of the hashmap
    int oldCapacity = capacity;
    capacity = capacity * 2;

    printf("Triggering a resize with new size %d when no of elements are %d \n", capacity, noOfElements);

    kvPair **newHashmap = malloc(capacity * sizeof(kvPair *));

    for (int index = 0; index < oldCapacity; index++)
    {
        if (oldHashmap[index] == NULL)
        {
            continue;
        }
        else
        {
            kvPair *pair = oldHashmap[index];
            int newIndex = getSlotLinearProbing(pair->key, newHashmap);
            newHashmap[newIndex] = pair;
        }
    }

    free(oldHashmap);

    return newHashmap;
}

kvPair **addKey(char *key, char *value, kvPair **hashtable)
{

    float loadFactorVal = loadFactor();

    // trigger a resize when loadfactor is greater than treshold
    if (loadFactorVal >= resizeTreshold)
    {
        hashtable = resizeHashmap(hashtable);
    }

    int index = getSlotLinearProbing(key, hashtable);

    if (index == -1)
    {
        printf("Add failed as there are no slots left in hashmap via Linear probing");
    }
    else
    {
        printf("Adding key %s at index %d \n", key, index);
        kvPair *pair = malloc(sizeof(kvPair));
        pair->key = key;
        pair->value = value;
        hashtable[index] = pair;
        noOfElements++;
    }

    return hashtable;
}

int removeKey(char *key, kvPair **hashtable)
{
       
        
}

char *getValue(char *key, kvPair **hashtable)
{
    int index = getIndexByKey(key, hashtable);

    if (index == -1)
        return NULL;

    return hashtable[index]->value;
}

int main()
{

    hashtable = malloc(capacity * sizeof(kvPair *));
    char *keys[] = {"Yaswanth", "Gandhi", "Nagalakshmi", "Skyfall", "Broo", "SAF", "ADS", "ADSASD"};
    char *values[] = {"ezrawolf", "mom", "dad", "ads", "ads", "asdeaw", "adsasd", "adsdsa"};
    for (int i = 0; i < 8; i++)
    {
        // printf("hashtable address is %p for adding string %s \n", hashtable, keys[i]);
        hashtable = addKey(keys[i], values[i], hashtable);
    }

    for (int index = 0; index < capacity; index++)
    {
        if (hashtable[index] != NULL)
        {
            printf("The element there is %s at index %i and the value is %s \n", hashtable[index]->key, index, hashtable[index]->value);
        }
    }

    char *myValue = getValue("ADSASD", hashtable);
    printf("My Value is %s \n", myValue);
}