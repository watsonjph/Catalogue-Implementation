#include "Catalogue.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

void test_add_product(Catalogue* catalogue, const char* ProductName, Component* components, int numComponents) {
    printf("\n--- Test Add Product: %s ---\n", ProductName);
    char* productName = (char*)malloc(strlen(ProductName) + 1);
    strcpy(productName, ProductName);

    Component fullComponents[COMPONENT_LIMIT] = { 0 };
    for (int i = 0; i < numComponents; i++) {
        fullComponents[i] = components[i];
    }

    Product* product = createProduct(productName, fullComponents);
    assert(product != NULL);

    int addResult = addProduct(catalogue, product);
    assert(addResult == 0);


    printf("\nCatalogue after adding '%s':\n", productName);
    printCatalogue(catalogue);

    Product* retrievedProduct = getProduct(catalogue, productName);
    assert(retrievedProduct != NULL);
    assert(strcmp(retrievedProduct->name, productName) == 0);

    free(productName);
}

void test_delete_product(Catalogue* catalogue, const char* constProductName) {
    printf("\n--- Test Delete Product: %s ---\n", constProductName);
    char* productName = (char*)malloc(strlen(constProductName) + 1);
    strcpy(productName, constProductName);

    // Print before deletion
    printf("Catalogue before deleting '%s':\n", productName);
    printCatalogue(catalogue);

    int delResult = deleteProduct(catalogue, productName);
    assert(delResult == 0);

    // Print after deletion
    printf("\nCatalogue after deleting '%s':\n", productName);
    printCatalogue(catalogue);

    Product* retrievedProduct = getProduct(catalogue, productName);
    assert(retrievedProduct == NULL);

    free(productName); // Clean up the mutable copy used for testing
}

int main() {
    Catalogue* catalogue = initCatalogue();
    assert(catalogue != NULL);

    // Components for each product
    Component doorComponents[3] = { {"Wood", 10}, {"Nails", 100}, {"Paint", 5} };
    Component windowComponents[2] = { {"Glass", 4}, {"Frame", 8} };

    test_add_product(catalogue, "Door", doorComponents, 3);

    test_add_product(catalogue, "Window", windowComponents, 2);

    test_delete_product(catalogue, "Window");

    printf("\n\nFinal state of the Catalogue:\n");
    printCatalogue(catalogue);

    deleteAllProducts(catalogue);
    free(catalogue);

    printf("\nAll tests passed.\n");
    return 0;
}