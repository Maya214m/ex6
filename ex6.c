#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------
void trimWhitespace(char *str) {
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0) {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src) {
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest) {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}
int readIntSafe(const char *prompt) {
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success) {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }
        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0) {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        if (*endptr != '\0') {
            printf("Invalid input.\n");
        } else {
            // We got a valid integer
           success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);
    return input;
}
// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice)
    {
    case 1:
        displayBFS(owner->pokedexRoot);
        break;
    case 2:
        preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
        inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
        postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
        displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}
// Queue that expends as needed
Queue *createQueue(int initialCapacity) {
    Queue *queue = malloc(sizeof(Queue));
    if (!queue) {
        fprintf(stderr, "Queue allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    queue->data = malloc(initialCapacity * sizeof(PokemonNode *));
    if (!queue->data) {
        fprintf(stderr, "Queue data allocation failed.\n");
        free(queue);
        exit(EXIT_FAILURE);
    }
    queue->front = 0;
    queue->rear = 0;
    queue->capacity = initialCapacity;
    return queue;
}

void enqueue(Queue *queue, PokemonNode *node) {
    if (queue->rear >= queue->capacity) {
        queue->capacity *= 2;
        queue->data = realloc(queue->data, queue->capacity * sizeof(PokemonNode *));
        if (!queue->data) {
            fprintf(stderr, "Queue resizing failed.\n");
            exit(EXIT_FAILURE);
        }
    }
    queue->data[queue->rear++] = node;
}

PokemonNode *dequeue(Queue *queue) {
    if (queue->front == queue->rear) {
        return NULL; // Queue is empty
    }
    return queue->data[queue->front++];
}

int isQueueEmpty(Queue *queue) {
    return queue->front == queue->rear;
}

void freeQueue(Queue *queue) {
    free(queue->data);
    free(queue);
}
// Function BFSGeneric
void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (!root) return;
    // Create a dynamic queue
    Queue *queue = createQueue(10);
    enqueue(queue, root);
    while (!isQueueEmpty(queue)) {
        PokemonNode *current = dequeue(queue);
        visit(current);
        if (current->left) {
            enqueue(queue, current->left);
        }
        if (current->right) {
            enqueue(queue, current->right);
        }
    }
    freeQueue(queue);
}
// Function preOrderGeneric
void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (!root) return;
    visit(root);
    preOrderGeneric(root->left, visit);
    preOrderGeneric(root->right, visit);
}
// Function inOrderGeneric
void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (!root) return;
    inOrderGeneric(root->left, visit);
    visit(root);
    inOrderGeneric(root->right, visit);
}
// Function postOrderGeneric
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (!root) return;
    postOrderGeneric(root->left, visit);
    postOrderGeneric(root->right, visit);
    visit(root);
}
// Function displayBFS
void displayBFS(PokemonNode *root) {
    BFSGeneric(root, printPokemonNode);
}
// Function preOrderTraversal
void preOrderTraversal(PokemonNode *root) {
    if (!root) {
        printf("Pokedex is empty.\n");
        return;
    }
    preOrderGeneric(root, printPokemonNode);
}
// Function inOrderTraversal
void inOrderTraversal(PokemonNode *root) {
    if (!root) {
        printf("Pokedex is empty.\n");
        return;
    }
    inOrderGeneric(root, printPokemonNode);
}
// Function postOrderTraversal
void postOrderTraversal(PokemonNode *root) {
    if (!root) {
        printf("Pokedex is empty.\n");
        return;
    }
    postOrderGeneric(root, printPokemonNode);
}
// Initilizes the NodeArray structure
void initNodeArray(NodeArray *na, int cap) {
    na->nodes = malloc(cap * sizeof(PokemonNode *));
    na->size = 0;
    na->capacity = cap;
}
// Function displayAlphabetical
void displayAlphabetical(PokemonNode *root) {
    NodeArray na;
    initNodeArray(&na, 10);
    collectAll(root, &na);
    qsort(na.nodes, na.size, sizeof(PokemonNode *), compareByNameNode);
    for (int i = 0; i < na.size; i++) {
        printPokemonNode(na.nodes[i]);
    }
    free(na.nodes);
}
// Function collectAll
void collectAll(PokemonNode *root, NodeArray *na) {
    if (!root) return;
    if (na->size >= na->capacity) {
        na->capacity *= 2;
        na->nodes = realloc(na->nodes, na->capacity * sizeof(PokemonNode *));
    }
    na->nodes[na->size++] = root;
    collectAll(root->left, na);
    collectAll(root->right, na);
}
// Function compareByNameNode
int compareByNameNode(const void *a, const void *b) {
    PokemonNode *nodeA = *(PokemonNode **)a;
    PokemonNode *nodeB = *(PokemonNode **)b;
    return strcmp(nodeA->data->name, nodeB->data->name);
}
// Function createPokemonNode
PokemonNode *createPokemonNode(const PokemonData *data) {
    PokemonNode *node = malloc(sizeof(PokemonNode));
    if (!node) return NULL;
    node->data = malloc(sizeof(PokemonData));
    if (!node->data) {
        free(node);
        return NULL;
    }
    // Copy the data
    *node->data = *data;
    node->left = node->right = NULL;
    return node;
}
// Function insertPokemon
PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    if (!root) return newNode;
    if (newNode->data->id < root->data->id)
        root->left = insertPokemonNode(root->left, newNode);
    else if (newNode->data->id > root->data->id)
        root->right = insertPokemonNode(root->right, newNode);
    return root;
}
// Remove pokemon by Id
PokemonNode *removePokemonById(PokemonNode *root, int id) {
    if (!root) return NULL;
    PokemonNode *targetNode = searchPokemonBFS(root,id);
    if (!targetNode) {
        printf("Pokemon with ID %d not found.\n", id);
        return root;
    }
    root = removeNodeBST(root, id);
    return root;
}
// Free the entire Bst
void freePokemonTree(PokemonNode *root) {
    if (!root) return;
    freePokemonTree(root->left);
    freePokemonTree(root->right);
    freePokemonNode(root);
}
// Link owner in circular list
void linkOwnerInCircularList(OwnerNode *newOwner) {
    if (!ownerHead) {
        ownerHead = newOwner;
        newOwner->next = newOwner->prev = newOwner;
        return;
    }
    OwnerNode *tail = ownerHead->prev;
    tail->next = newOwner;
    newOwner->prev = tail;
    newOwner->next = ownerHead;
    ownerHead->prev = newOwner;
}
// Find owner by name
OwnerNode *findOwnerByName(const char *name) {
    if (!ownerHead) return NULL;
    OwnerNode *current = ownerHead;
    do {
        if (strcmp(current->ownerName, name) == 0)
            return current;
        current = current->next;
    } while (current != ownerHead);
    return NULL;
}
// Free all owners
void freeAllOwners(void) {
    if (!ownerHead) return;
    OwnerNode *current = ownerHead;
    do {
        OwnerNode *next = current->next;
        freeOwnerNode(current);
        current = next;
    } while (current != ownerHead);
    ownerHead = NULL;
}
// Remove Node BST
PokemonNode *removeNodeBST(PokemonNode *root, int id) {
    if (!root) {
        return NULL;
    }
    if (id < root->data->id) {
        root->left = removeNodeBST(root->left, id);
    } else if (id > root->data->id) {
        root->right = removeNodeBST(root->right, id);
    } else {
        if (!root->left) {
            PokemonNode *temp = root->right;
            freePokemonNode(root);
            return temp;
        } else if (!root->right) {
            PokemonNode *temp = root->left;
            freePokemonNode(root);
            return temp;
        }
        // Find the minimum node in the right subtree
        PokemonNode *temp = root->right;
        while (temp && temp->left) {
            temp = temp->left;
        }
        root->data = temp->data;
        root->right = removeNodeBST(root->right, temp->data->id);
    }
    return root;
}
// Free Pokemon node
void freePokemonNode(PokemonNode *node) {
    if (!node) return;
    free(node->data);
    free(node);
}
// Free owner node
void freeOwnerNode(OwnerNode *owner) {
    if (!owner) return;
    freePokemonTree(owner->pokedexRoot);
    free(owner->ownerName);
    free(owner);
}
// Create owner
OwnerNode *createOwner(char *ownerName, PokemonNode *starter) {
    if (!ownerName || !starter) return NULL;
    OwnerNode *newOwner = (OwnerNode *)malloc(sizeof(OwnerNode));
    if (!newOwner) {
        printf("Memory allocation error.\n");
        return NULL;
    }
    newOwner->ownerName = ownerName;
    newOwner->pokedexRoot = starter;
    newOwner->next = newOwner->prev = NULL;
    return newOwner;
}
// Search Pokemon BFS
PokemonNode *searchPokemonBFS(PokemonNode *root, int id) {
    if (!root) return NULL;
    Queue *queue = createQueue(10);
    enqueue(queue, root);
    PokemonNode *found = NULL;
    while (!isQueueEmpty(queue)) {
        PokemonNode *current = dequeue(queue);
        if (current->data->id == id) {
            found = current;
            break;
        }
        if (current->left) enqueue(queue, current->left);
        if (current->right) enqueue(queue, current->right);
    }
    freeQueue(queue);
    return found;
}
// Remove owner from circular list
void removeOwnerFromCircularList(OwnerNode *target) {
    if (!ownerHead || !target) return;
    if (target->next == target) {
        ownerHead = NULL;
    } else {
        target->prev->next = target->next;
        target->next->prev = target->prev;
        if (ownerHead == target) {
            ownerHead = target->next;
        }
    }
    freeOwnerNode(target);
}
// Swap Owner Data
void swapOwnerData(OwnerNode *a, OwnerNode *b) {
    if (!a || !b) return;
    char *tempName = a->ownerName;
    a->ownerName = b->ownerName;
    b->ownerName = tempName;
    PokemonNode *tempPokedex = a->pokedexRoot;
    a->pokedexRoot = b->pokedexRoot;
    b->pokedexRoot = tempPokedex;
}
// Function for adding new Pokedex
void openPokedexMenu() {
    printf("Your name: ");
    char *ownerName = getDynamicInput();
    if (!ownerName) {
        printf("Failed to allocate memory for owner name.\n");
        return;
    }
    // Check for duplicate owner names
    if (findOwnerByName(ownerName)) {
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n", ownerName);
        free(ownerName);
        return;
    }
    // Display starter Pokemon options
    printf("Choose Starter:\n");
    printf("1. Bulbasaur\n");
    printf("2. Charmander\n");
    printf("3. Squirtle\n");
    int starterChoice = readIntSafe("Your choice: ");
    if (starterChoice < 1 || starterChoice > 3) {
        printf("Invalid choice. Please select between 1 and 3.\n");
        free(ownerName);
        return;
    }
    // Map the starter choice to the respective Pokemon ID
    int starterID;
    switch (starterChoice) {
        case 1:
            starterID = 1; // Bulbasaur
        break;
        case 2:
            starterID = 4; // Charmander
        break;
        case 3:
            starterID = 7; // Squirtle
        break;
    }
    // Create the starter Pokemon node
    PokemonNode *starter = createPokemonNode(&pokedex[starterID - 1]);
    if (!starter) {
        printf("Failed to create starter Pokemon.\n");
        free(ownerName);
        return;
    }
    // Create a new OwnerNode
    OwnerNode *newOwner = createOwner(ownerName, starter);
    if (!newOwner) {
        printf("Failed to create owner.\n");
        freePokemonTree(starter);
        free(ownerName);
        return;
    }
    // Add the new owner to the circular linked list
    linkOwnerInCircularList(newOwner);
    printf("New Pokedex created for %s with starter %s.\n", ownerName, starter->data->name);
}
// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu() {
    if (!ownerHead) {
        printf("No existing Pokedexes.\n");
        return;
    }
    // Display all existing Pokedex owners
    printf("\nExisting Pokedexes:\n");
    OwnerNode *cur = ownerHead;
    int count = 0;

    do {
        printf("%d. %s\n", ++count, cur->ownerName);
        cur = cur->next;
    } while (cur != ownerHead);

    printf("Choose a Pokedex by number: ");
    int choice = readIntSafe("");

    if (choice < 1 || choice > count) {
        printf("Invalid choice.\n");
        return;
    }
    // Navigate to the selected owner
    cur = ownerHead;
    for (int i = 1; i < choice; ++i) {
        cur = cur->next;
    }
    // Manage the selected owner's Pokedex
    printf("\nEntering %s's Pokedex...\n", cur->ownerName);
    int subChoice;
    do {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main Menu\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice) {
            case 1:
                addPokemon(cur);
            break;
            case 2:
                displayMenu(cur);
            break;
            case 3:
                freePokemon(cur);
            break;
            case 4:
                pokemonFight(cur);
            break;
            case 5:
                evolvePokemon(cur);
            break;
            case 6:
                printf("Back to main menu.\n");
            break;
            default:
                printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}
// Function addPokemon
void addPokemon(OwnerNode *owner) {
    if (!owner) {
        printf("Invalid owner.\n");
        return;
    }
    printf("Enter ID to add: ");
    int id = readIntSafe("");
    // Check if ID exists in the global pokedex
    const PokemonData *pokemon = NULL;
    for (int i = 0; i < (int)(sizeof(pokedex) / sizeof(pokedex[0])); i++) {
        if (pokedex[i].id == id) {
            pokemon = &pokedex[i];
            break;
        }
    }
    if (!pokemon) {
        printf("Pokemon with ID %d does not exist in the global pokedex.\n", id);
        return;
    }
    // Check for duplicates in the owner's pokedex
    if (searchPokemonBFS(owner->pokedexRoot, id)) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", id);
        return;
    }
    // Add the Pokemon to the owner's pokedex
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, createPokemonNode(pokemon));
    printf("Pokemon %s (ID %d) added.\n", pokemon->name, pokemon->id);
}
// Function freePokemon
void freePokemon(OwnerNode *owner) {
    if (!owner || !owner->pokedexRoot) {
        printf("No existing Pokemon to release.\n");
        return;
    }
    printf("Enter Pokemon ID to release: ");
    int id = readIntSafe("");
    // Use removePokemonByID to handle removal
    PokemonNode *targetPokemon = searchPokemonBFS(owner->pokedexRoot, id);
    if (!targetPokemon) {
        printf("No Pokemon with ID %d found.\n", id);
        return;
    }
        printf("Removing Pokemon %s (ID %d).\n", targetPokemon->data->name, targetPokemon->data->id);
        owner->pokedexRoot = removePokemonById(owner->pokedexRoot, id);
    }
// Function pokemonFight
void pokemonFight(OwnerNode *owner) {
    if (!owner || !owner->pokedexRoot) {
        printf("Pokedex is empty.\n");
        return;
    }
    printf("Enter ID of the first Pokemon: \n");
    int id1 = readIntSafe("");
    printf("Enter ID of the second Pokemon: \n");
    int id2 = readIntSafe("");
    // Check if both Pokemon exist in the owner's pokedex
    PokemonNode *pokemon1 = searchPokemonBFS(owner->pokedexRoot, id1);
    PokemonNode *pokemon2 = searchPokemonBFS(owner->pokedexRoot, id2);
    if (!pokemon1 || !pokemon2) {
        printf("One or both Pokemon IDs not found.\n");
        return;
    }
    // Calculate fight scores
    double score1 = pokemon1->data->attack * 1.5 + pokemon1->data->hp * 1.2;
    double score2 = pokemon2->data->attack * 1.5 + pokemon2->data->hp * 1.2;
    printf("Pokemon 1: %s (Score = %.2f)\n", pokemon1->data->name, score1);
    printf("Pokemon 2: %s (Score = %.2f)\n", pokemon2->data->name, score2);
    // Determine winner
    if (score1 > score2) {
        printf("%s wins!\n", pokemon1->data->name);
    } else if (score1 < score2) {
        printf("%s wins!\n", pokemon2->data->name);
    } else {
        printf("It's a tie!\n");
    }
}
// Function evolvePokemon
void evolvePokemon(OwnerNode *owner) {
    if (!owner->pokedexRoot) {
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }
    int id = readIntSafe("Enter ID of Pokemon to evolve: \n");
    PokemonNode *node = searchPokemonBFS(owner->pokedexRoot, id);
    if (!node) {
        printf("No Pokemon with ID %d found.\n", id);
        return;
    }
    if (node->data->CAN_EVOLVE == CANNOT_EVOLVE) {
        printf("Pokemon with ID %d cannot evolve.\n", id);
        return;
    }
    // Check if evolved form exists
    int newID = id + 1;
    PokemonNode *evolvedNode = searchPokemonBFS(owner->pokedexRoot, newID);
    if (evolvedNode) {
        owner->pokedexRoot = removeNodeBST(owner->pokedexRoot, newID);
    }
    // Create new node for the evolved Pokemon
    PokemonNode *newNode = createPokemonNode(&pokedex[newID - 1]);
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newNode);
    printf("Removing Pokemon %s (ID %d).\n", node->data->name, id);

    printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n",
           node->data->name, id, newNode->data->name, newID);
    // Remove the old Pokemon
    owner->pokedexRoot = removeNodeBST(owner->pokedexRoot, id);
}
// Function to delete a Pokedex
void deletePokedex() {
    if (!ownerHead) {
        printf("No existing Pokedexes to delete.\n");
        return;
    }

    // Display the list of owners
    printf("\n=== Delete a Pokedex ===\n");
    OwnerNode *current = ownerHead;
    int count = 0;

    do {
        printf("%d. %s\n", ++count, current->ownerName);
        current = current->next;
    } while (current != ownerHead);
    // Ask the user to choose a Pokedex to delete
    printf("\nChoose a Pokedex to delete by number: ");
    int choice = readIntSafe("");

    if (choice < 1 || choice > count) {
        printf("Invalid choice.\n");
        return;
    }
    // Navigate to the selected owner
    current = ownerHead;
    for (int i = 1; i < choice; ++i) {
        current = current->next;
    }
    // Delete the selected Pokedex
    printf("Deleting %s's entire Pokedex...\n", current->ownerName);
    removeOwnerFromCircularList(current);
    printf("Pokedex deleted.\n");
}
// Function to merge Pokedex
void mergePokedexMenu() {
    if (!ownerHead || ownerHead->next == ownerHead) {
        printf("Not enough owners to merge.\n");
        return;
    }
    printf("\n=== Merge Pokedexes ===\n");
    // Prompt for the names of the two owners
    printf("Enter name of first owner: ");
    char *firstOwnerName = getDynamicInput();
    printf("Enter name of second owner: ");
    char *secondOwnerName = getDynamicInput();
    // Find the owners in the circular list
    OwnerNode *firstOwner = findOwnerByName(firstOwnerName);
    OwnerNode *secondOwner = findOwnerByName(secondOwnerName);

    if (!firstOwner || !secondOwner || firstOwner == secondOwner) {
        printf("Invalid owners specified.\n");
        free(firstOwnerName);
        free(secondOwnerName);
        return;
    }
    // Merge secondOwner's Pokedex into firstOwner's Pokedex
    printf("Merging %s and %s...\n", firstOwner->ownerName, secondOwner->ownerName);
    firstOwner->pokedexRoot = mergeBST(firstOwner->pokedexRoot, secondOwner->pokedexRoot);
    // Remove secondOwner from the circular linked list
    removeOwnerFromCircularList(secondOwner);

    printf("Merge completed.\n");
    printf("Owner '%s' has been removed after merging.\n", secondOwnerName);
    // Clean up dynamic memory for owner names
    free(firstOwnerName);
    free(secondOwnerName);
}
// Helper function for merging two BST
PokemonNode *mergeBST(PokemonNode *root1, PokemonNode *root2) {
    if (!root1) return root2;
    if (!root2) return root1;
    // Insert root2 into root1 recursively
    root1 = insertPokemonNode(root1, createPokemonNode(root2->data));
    // Merge left and right subtrees
    root1 = mergeBST(root1, root2->left);
    root1 = mergeBST(root1, root2->right);
    return root1;
}
// Function to sort owners by name
void sortOwners() {
    if (!ownerHead || ownerHead->next == ownerHead) {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }
    int swapped;
    do {
        swapped = 0;
        OwnerNode *current = ownerHead;

        do {
            OwnerNode *next = current->next;
            if (strcmp(current->ownerName, next->ownerName) > 0) {
                swapOwnerData(current, next);
                swapped = 1;
            }
            current = next;
        } while (current != ownerHead);
    } while (swapped);

    printf("Owners sorted successfully.\n");
}
// Function for Print Owners in a direction X times
void printOwnersCircular() {
    if (!ownerHead) {
        printf("No owners.\n");
        return;
    }
    // Prompt user for direction
    printf("Enter direction (F or B): ");
    char *directionInput = getDynamicInput();
    if (!directionInput) {
        printf("Failed to read direction input.\n");
        return;
    }
    // Validate and process direction input
    char direction = directionInput[0]; // Check only the first character
    if (direction != 'f' && direction != 'F' && direction != 'b' && direction != 'B') {
        printf("Invalid direction. Enter F for forward or B for backward.\n");
        return;
    }
    if (directionInput[0] == 'f' || directionInput[0] == 'F') direction = 'F';
    if (directionInput[0] == 'b' || directionInput[0] == 'B') direction = 'B';
    free(directionInput);
    // Prompt user for number of prints
    printf("How many prints? ");
    int prints = readIntSafe("");
    if (prints <= 0) {
        printf("Number of prints must be positive.\n");
        return;
    }
    // Traverse and print owners in the specified direction
    OwnerNode *current = ownerHead;
    printf("\n");
    for (int i = 1; i <= prints; i++) {
        printf("[%d] %s\n", i, current->ownerName);
        current = (direction == 'F') ? current->next : current->prev; // Move forward or backward
    }
}
// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");
        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            enterExistingPokedexMenu();
            break;
        case 3:
            deletePokedex();
            break;
        case 4:
            mergePokedexMenu();
            break;
        case 5:
            sortOwners();
            break;
        case 6:
            printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    mainMenu();
    freeAllOwners();
    return 0;
}
