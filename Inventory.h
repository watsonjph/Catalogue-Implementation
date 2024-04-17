#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>



#pragma warning(disable: 4996)

#define NAME_LIMIT 50
#define INVENTORY_SIZE 25


typedef struct Component {

    char name[NAME_LIMIT];
    int  qty;
    struct Component* next;

}Component;

typedef struct Inventory {
    Component* items[INVENTORY_SIZE];
}Inventory;


Inventory* initInventory();
Component* createComponent(char* newName, int newQty);
int generateHash(char* name);

//Add/delete from hash table
int addComponent(Inventory* inventory, Component* newComponent);
int deleteComponent(Inventory* inventory, char* name);
int deleteAllComponents(Inventory* inventory); // Used to de-allocate memory upon exit

int getQty(Inventory* inventory, char* name);//Used by createWorkOrder to check availability
int modifyQty(Inventory* inventory, char* name, int qtyToAdd); //E.g. qtyToAdd = 50 -> adds 50
//    qtyToAdd = -10 -> subtracts 10

void printComponent(Component* component);//Used inside printInventory for loop
void printInventory(Inventory* inventory);