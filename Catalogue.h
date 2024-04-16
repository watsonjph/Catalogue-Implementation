#pragma once
#include "Inventory.h"


/*
Product Catalogue (Trie)
*/


#define COMPONENT_LIMIT 5 // Max components per product
#define MAX_TRIE_ROOTS 128 // ASCII characters
#define NAME_LIMIT 100

typedef struct Product {

    char name[NAME_LIMIT]; //Full name - E.g. AR3
    Component requiredComponents[COMPONENT_LIMIT]; //NOTE: DOESN'T use dynamically allocated components.
    //We'll get the array of componenets via the menu & pass it in when creating.

}Product;

typedef struct Node {

    char character;
    Product* product; // If this isn't the end of a name, leave NULL
    struct Node* parent;
    Node* children[MAX_TRIE_ROOTS];

}Node;

typedef struct Catalogue {

    Node* roots[MAX_TRIE_ROOTS];

}Catalogue;

Catalogue* initCatalogue();


Product* createProduct(char* name, Component* arrayOfComponents); //
Node* createTrieNode(char character, Node* parent); // If no product at this node, product = NULL;

//Calls createTrieNode for each char (if needed), stores product in last one.
int addProduct(Catalogue* catalogue, Product* product);
//This ones gonna be a bit tricky. Has to find target, and delete all nodes leading to it UNLESS they're used by another product.
int deleteProduct(Catalogue* catalogue, char* target);
int deleteAllProducts(Catalogue* catalogue); // Used to de-allocate memory upon exit

Product* getProduct(Catalogue* catalogue, char* target);//Used by createWorkOrder()

void printSearchResults(Catalogue* catalogue, char* str);//User partial search - I.e. All products after "Sho"
void printCatalogue(Catalogue* catalogue); //Print ALL products in Trie


// Helper functions to prevent code duplication
Node* navigateToNode(Catalogue* catalogue, char* prefix);
void freeTrieNode(Node* node); //Recursively free all nodes in a trie
void printFromNode(Node* node, char* prefix); // Recursively print all products in a trie
int getIndex(char c);