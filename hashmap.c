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

typedef enum
{
    EMPTY,
    OCCUPIED,
    TOMBSTONE
} marker;

typedef struct item
{
    void *key;
    void *value;
    marker itemState;

} item;

typedef enum
{
    SHRINK,
    INCREASE,
} reshrinkStrat;

item *hashtable;
int addKey(void *key, void *value, item **hashtable);
void resizeHashmap(item **hashtable, reshrinkStrat stratergy);

int capacity = 4;
int noOfElements = 0;
float resizeTreshold = 0.7;
float shrinkTreshold = 0.25;

float loadFactor()
{
    float loadFactorValue = (float)noOfElements / capacity;
    printf("No of elements is %d, capacity is %d, loadfactor is %f \n", noOfElements, capacity, loadFactorValue);
    return loadFactorValue;
}

int hash(char *s)
{
    int randomNumber = rand();
    return randomNumber;
}

int getSlotLinearProbing(void *key, item **hashtable)
{
    int computedHash = hash(key);
    int index = computedHash % capacity;

    if ((*hashtable)[index].itemState == EMPTY)
        return index;

    for (int probe = (index + 1) % capacity; probe != index; probe = (probe + 1) % capacity)
    {
        int firstTombstone = -1;
        int idx = (index + probe) % capacity;
        marker state = (*hashtable)[idx].itemState;

        if (state == EMPTY)
        {
            /* empty slot: key is not present; reuse first tombstone if seen */
            if (firstTombstone != -1)
                return firstTombstone;
            return idx;
        }

        if (state == OCCUPIED)
        {
            /* check for equality — using pointer equality here.
               Replace with your equals() function if you use content comparison. */
            if ((*hashtable)[idx].key == key)
                return idx;
            /* else continue probing */
        }
        else /* TOMBSTONE */
        {
            /* remember first tombstone encountered for possible reuse */
            if (firstTombstone == -1)
                firstTombstone = idx;
            /* continue probing to ensure key isn't further in the cluster */
        }
    }

    return -1;
}

int getIndexByKey(void *key, item *hashtable)
{
    int computedHash = hash(key);
    int index = computedHash % capacity;

    if (hashtable[index].itemState == EMPTY)
    {
        return -1;
    }
    else if (hashtable[index].key == key && hashtable[index].itemState == OCCUPIED)
    {
        return index;
    }

    for (int probe = index + 1; probe != index; probe = (probe + 1) % capacity)
    {

        if (hashtable[probe].key == key && hashtable[probe].itemState == OCCUPIED)
            return probe;
    }

    return -1;
}

void resizeHashmap(item **hashtable, reshrinkStrat stratergy)
{
    item *oldHashtable = *hashtable;
    // double the capacity of the hashmap
    int oldCapacity = capacity;

    if (stratergy != SHRINK)
        capacity = capacity * 2;
    else
        capacity = capacity / 2;

    printf("Triggering a resize with new size %d when no of elements are %d \n", capacity, noOfElements);

    item *newHashtable = calloc(capacity, sizeof(item));

    for (int index = 0; index < oldCapacity; index++)
    {
        if (oldHashtable[index].itemState == EMPTY)
        {
            continue;
        }
        else if (oldHashtable[index].itemState == TOMBSTONE)
        {
            // IF DESTROY FUNCTION EXISTS PASS THE DESTROY FUNCTION IN THAT ELSE JUST IGNORE THAT VALUE
            // free(oldHashtable[index].pair);
            continue;
        }
        else
        {
            int newIndex = getSlotLinearProbing(oldHashtable[index].key, &newHashtable);
            newHashtable[newIndex].key = oldHashtable[index].key;
            newHashtable[newIndex].value = oldHashtable[index].value;
            newHashtable[newIndex].itemState = OCCUPIED;
        }
    }

    free(oldHashtable);
    *hashtable = newHashtable;

    return;
}

int addKey(void *key, void *value, item **hashtable)
{

    float loadFactorVal = (float)loadFactor();

    // trigger a resize when loadfactor is greater than treshold

    if (loadFactorVal >= resizeTreshold)
    {
        resizeHashmap(hashtable, INCREASE);
    }

    int index = getSlotLinearProbing(key, hashtable);
    printf("Getting indexed at %d \n", index);
    (*hashtable)[index].key = key;
    (*hashtable)[index].value = value;
    (*hashtable)[index].itemState = OCCUPIED;
    noOfElements++;
    return index;
}

void* removeKey(void *key, item **hashtable)
{
    int index = getIndexByKey(key, *hashtable);

    if (index == -1)
        return NULL;

    noOfElements--;

    (*hashtable)[index].itemState=TOMBSTONE;
    void *value = (*hashtable)[index].value;

    float loadFactorVal =(float) loadFactor();
    if (loadFactorVal <= shrinkTreshold)
    {
        printf("Triggering shrink when loadFactor is %f and capacity is %d and no of ele is %d \n", loadFactorVal, capacity, noOfElements);
        resizeHashmap(hashtable, SHRINK);
    }
    
    return value;
}

void *getValue(void *key, item *hashtable)
{
    int index = getIndexByKey(key, hashtable);
    
    if (index == -1)
        return NULL;

    return hashtable[index].value;
}

void test_insert_and_resize()
{
    printf("\n--- Test 1: Insert and Resize ---\n");
    char *keys[] = {"Yaswanth", "Gandhi", "Nagalakshmi", "Skyfall", "Broo", "SAF", "ADS", "ADSASD"};
    char *values[] = {"ezrawolf", "mom", "dad", "ads", "ads", "asdeaw", "adsasd", "adsdsa"};

    for (int i = 0; i < 8; i++)
    {
        printf("Inserting %s -> %s\n", keys[i], values[i]);
        addKey(keys[i], values[i], &hashtable);
    }

    printf("\nAfter insertion and possible resize:\n");
    for (int i = 0; i < capacity; i++)
    {
        if (hashtable[i].itemState != EMPTY)
        {
            printf("Index %d: key=%s, value=%s\n", i, (char *)hashtable[i].key, (char *)hashtable[i].value);
        }
    }

    // printf("Value of Yaswanth is %s \n",(char*)getValue(keys[0],hashtable));
  
}

// void test_get_value()
// {
//     printf("\n--- Test 2: Get Value ---\n");
//     char *myValue = getValue("Gandhi", hashtable);
//     if (myValue)
//         printf("Found value for 'Gandhi': %s\n", myValue);
//     else
//         printf("'Gandhi' not found.\n");

//     myValue = getValue("Skyfall", hashtable);
//     if (myValue)
//         printf("Found value for 'Skyfall': %s\n", myValue);
//     else
//         printf("'Skyfall' not found.\n");

//     myValue = getValue("UnknownKey", hashtable);
//     if (!myValue)
//         printf("'UnknownKey' correctly not found.\n");
// }

// void test_remove_and_get()
// {
//     printf("\n--- Test 3: Remove and Get ---\n");

//     hashtable = removeKey("Gandhi", hashtable);
//     char *myValue = getValue("Gandhi", hashtable);
//     if (myValue == NULL)
//         printf("After removal, 'Gandhi' not found ✅\n");
//     else
//         printf("Error: 'Gandhi' still exists ❌\n");

//     hashtable = removeKey("Skyfall", hashtable);
//     myValue = getValue("Skyfall", hashtable);
//     if (myValue == NULL)
//         printf("After removal, 'Skyfall' not found ✅\n");
// }

// void test_reinsertion_after_remove()
// {
//     printf("\n--- Test 4: Reinsertion After Removal ---\n");

//     hashtable = addKey("Gandhi", "new_mom", hashtable);
//     char *myValue = getValue("Gandhi", hashtable);
//     printf("My value gandhi is %s \n", myValue);
//     if (myValue && strcmp(myValue, "new_mom") == 0)
//         printf("Reinsertion of 'Gandhi' successful ✅\n");
//     else
//         printf("Reinsertion failed ❌\n");
// }

// void test_mass_removal_and_shrink()
// {
//     printf("\n--- Test 5: Mass Removal and Shrink ---\n");
//     char *keys[] = {"Yaswanth", "Nagalakshmi", "Broo", "SAF", "ADS", "ADSASD"};
//     for (int i = 0; i < 6; i++)
//     {
//         printf("Removing %s\n", keys[i]);
//         hashtable = removeKey(keys[i], hashtable);
//     }

//     printf("\nAfter mass removal:\n");
//     for (int i = 0; i < capacity; i++)
//     {
//         if (hashtable[i])
//             printf("Index %d -> key: %s value: %s is deleted %d \n", i, hashtable[i]->key, hashtable[i]->value, hashtable[i]->isDeleted);
//         else
//             printf("Index %d is empty\n", i);
//     }
// }

int main()
{
    hashtable = calloc(capacity, sizeof(item));
    test_insert_and_resize();
    
    // int addedIndex = addKey("Yaswanth", "ezrawolf", &hashtable);
    // printf("Added at key %s and value is %s \n", (char *)hashtable[addedIndex].key, (char *)hashtable[addedIndex].value);
    // test_insert_and_resize();
    // test_get_value();
    // test_remove_and_get();
    // test_reinsertion_after_remove();
    // test_mass_removal_and_shrink();
}