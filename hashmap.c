#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
    int8_t isDeleted;
} kvPair;

kvPair **hashtable;
kvPair **addKey(char *key, char *value, kvPair **hashtable);
kvPair **resizeHashmap(kvPair **oldHashmap, int shrink);

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
        index = (index + 1) % capacity;

    } while ((hashmap[index] != NULL) && initalIndex != index);

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
        index = (index + 1) % capacity;

    } while (hashmap[index] != NULL && strcmp(hashmap[index]->key, key) != 0);

    if (hashmap[index] == NULL || strcmp(hashmap[index]->key, key) != 0)
    {
        return -1;
    }

    return index;
}

kvPair **resizeHashmap(kvPair **oldHashmap, int shrink)
{
    // double the capacity of the hashmap
    int oldCapacity = capacity;

    if (!shrink)
        capacity = capacity * 2;
    else
        capacity = capacity / 2;

        printf("Triggering a resize with new size %d when no of elements are %d \n", capacity, noOfElements);

    kvPair **newHashmap = malloc(capacity * sizeof(kvPair *));

    for (int index = 0; index < oldCapacity; index++)
    {
        if (oldHashmap[index] == NULL)
        {
            continue;
        }
        else if (oldHashmap[index]->isDeleted == 1)
        {
            free(oldHashmap[index]);
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
        hashtable = resizeHashmap(hashtable, 0);
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
        pair->isDeleted = 0;
        hashtable[index] = pair;
        noOfElements++;
    }

    return hashtable;
}

kvPair **removeKey(char *key, kvPair **hashtable)
{
    int index = getIndexByKey(key, hashtable);
    if (index == -1)
        return hashtable;
    kvPair *pair = hashtable[index];
    pair->isDeleted = 1;
    noOfElements--;
    float loadFactorVal = loadFactor();
    if (loadFactorVal <= 0.25)
    {
        printf("Triggering shrink when loadFactor is %f and capacity is %d and no of ele is %d \n", loadFactorVal,capacity,noOfElements);
        hashtable = resizeHashmap(hashtable, 1);
    }
    return hashtable;
}

char *getValue(char *key, kvPair **hashtable)
{
    int index = getIndexByKey(key, hashtable);

    if (index == -1 || hashtable[index]->isDeleted == 1)
        return NULL;

    return hashtable[index]->value;
}
void test_insert_and_resize()
{
    printf("\n--- Test 1: Insert and Resize ---\n");
    char *keys[] = {"Yaswanth", "Gandhi", "Nagalakshmi", "Skyfall", "Broo", "SAF", "ADS", "ADSASD"};
    char *values[] = {"ezrawolf", "mom", "dad", "ads", "ads", "asdeaw", "adsasd", "adsdsa"};

    for (int i = 0; i < 8; i++)
    {
        printf("Inserting %s -> %s\n", keys[i], values[i]);
        hashtable = addKey(keys[i], values[i], hashtable);
    }

    printf("\nAfter insertion and possible resize:\n");
    for (int i = 0; i < capacity; i++)
    {
        if (hashtable[i] != NULL)
        {
            printf("Index %d: key=%s, value=%s\n", i, hashtable[i]->key, hashtable[i]->value);
        }
    }
}

void test_get_value()
{
    printf("\n--- Test 2: Get Value ---\n");
    char *myValue = getValue("Gandhi", hashtable);
    if (myValue)
        printf("Found value for 'Gandhi': %s\n", myValue);
    else
        printf("'Gandhi' not found.\n");

    myValue = getValue("Skyfall", hashtable);
    if (myValue)
        printf("Found value for 'Skyfall': %s\n", myValue);
    else
        printf("'Skyfall' not found.\n");

    myValue = getValue("UnknownKey", hashtable);
    if (!myValue)
        printf("'UnknownKey' correctly not found.\n");
}

void test_remove_and_get()
{
    printf("\n--- Test 3: Remove and Get ---\n");

    hashtable = removeKey("Gandhi", hashtable);
    char *myValue = getValue("Gandhi", hashtable);
    if (myValue == NULL)
        printf("After removal, 'Gandhi' not found ✅\n");
    else
        printf("Error: 'Gandhi' still exists ❌\n");

    hashtable = removeKey("Skyfall", hashtable);
    myValue = getValue("Skyfall", hashtable);
    if (myValue == NULL)
        printf("After removal, 'Skyfall' not found ✅\n");
}

void test_reinsertion_after_remove()
{
    printf("\n--- Test 4: Reinsertion After Removal ---\n");

    hashtable = addKey("Gandhi", "new_mom", hashtable);
    char *myValue = getValue("Gandhi", hashtable);
    printf("My value gandhi is %s \n",myValue);
    if (myValue && strcmp(myValue, "new_mom") == 0)
        printf("Reinsertion of 'Gandhi' successful ✅\n");
    else
        printf("Reinsertion failed ❌\n");
}

void test_mass_removal_and_shrink()
{
    printf("\n--- Test 5: Mass Removal and Shrink ---\n");
    char *keys[] = {"Yaswanth", "Nagalakshmi", "Broo", "SAF", "ADS", "ADSASD"};
    for (int i = 0; i < 6; i++)
    {
        printf("Removing %s\n", keys[i]);
        hashtable = removeKey(keys[i], hashtable);
    }

    printf("\nAfter mass removal:\n");
    for (int i = 0; i < capacity; i++)
    {
        if (hashtable[i])
            printf("Index %d -> key: %s value: %s is deleted %d \n", i, hashtable[i]->key,hashtable[i]->value,hashtable[i]->isDeleted);
        else
            printf("Index %d is empty\n", i);
    }
}

int main()
{
    hashtable = malloc(capacity * sizeof(kvPair *));
    // char *keys[] = {"Yaswanth", "Gandhi", "Nagalakshmi", "Skyfall", "Broo", "SAF", "ADS", "ADSASD"};
    // char *values[] = {"ezrawolf", "mom", "dad", "ads", "ads", "asdeaw", "adsasd", "adsdsa"};
    // for (int i = 0; i < 8; i++)
    // {
    //     // printf("hashtable address is %p for adding string %s \n", hashtable, keys[i]);
    //     hashtable = addKey(keys[i], values[i], hashtable);
    // }

    // for (int index = 0; index < capacity; index++)
    // {
    //     if (hashtable[index] != NULL)
    //     {
    //         printf("The element there is %s at index %i and the value is %s \n", hashtable[index]->key, index, hashtable[index]->value);
    //     }
    // }

    // char *myValue = getValue("Gandhi", hashtable);
    // printf("My Value is %s \n", myValue);
    // hashtable = removeKey("Gandhi", hashtable);
    // myValue = getValue("Gandhi", hashtable);
    // printf("My Value is %s \n", myValue);

    test_insert_and_resize();
    test_get_value();
    test_remove_and_get();
    test_reinsertion_after_remove();
    test_mass_removal_and_shrink();
}