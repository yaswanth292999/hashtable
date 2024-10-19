#include <stdio.h>
#include <string.h>

#define INITIAL_HASHMAP_SIZE 5 // Use #define for fixed size

char *hashmap[INITIAL_HASHMAP_SIZE]; // Declaration of the hashmap

int computeHash(char value[], int SIZE_OF_HASHMAP)
{
    int lengthOfValueString = strlen(value);
    int totalAscii = 0;

    for (int i = 0; i < lengthOfValueString; i++)
    {
        int charAsciiValue = (int)value[i];
        totalAscii += charAsciiValue;
    }

    int hash = totalAscii % SIZE_OF_HASHMAP;

    return hash;
}

int setValue(char value[])
{
    int hash = computeHash(value, INITIAL_HASHMAP_SIZE); // Use corrected constant

    if (hashmap[hash] != NULL)
    {
        printf("Collision Detected for String %s as there is already an existing value %s at index %d.!!Value not set!!!\n", value, hashmap[hash], hash);
        return 0;
    }

    else
    {
        hashmap[hash] = value;
        printf("Set Value for %s at index %d\n", hashmap[hash], hash);
        return 1;
    }
}

int getValue(char value[])
{

    int hash = computeHash(value, INITIAL_HASHMAP_SIZE);

    if (hashmap[hash] == value)
    {
        printf("Got Value at index %d!!\n", hash);
    }
    else
    {
        printf("VALUE NOT FOUND!!!\n");
    }
}

int main()
{
    // Initialize the hashmap to NULL
    char *names[] = {
        "Alice",
        "Bob",
        "Charlie",
        "David",
        "Eve",
        "Frank",
        "Grace",
        "Heidi",
        "Ivan",
        "Judy"};

    // Set values for the sample names
    for (int i = 0; i < 10; i++)
    {
        setValue(names[i]); // Store each name in the hashmap
    }
    for (int i = 0; i < 10; i++)
    {
        getValue(names[i]); // Store each name in the hashmap
    }
    return 0;
}
