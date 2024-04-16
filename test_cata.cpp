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
    assert(product != NULL); // Ensure the product was created successfully

    int addResult = addProduct(catalogue, product);
    assert(addResult == 0); // Ensure the product was added successfully


    printf("\nCatalogue after adding '%s':\n", productName);
    printCatalogue(catalogue);

    Product* retrievedProduct = getProduct(catalogue, productName);
    assert(retrievedProduct != NULL); // Ensure the product was retrieved successfully
    assert(strcmp(retrievedProduct->name, productName) == 0); // Ensure the retrieved product has the correct name

    free(productName);
}


void test_delete_product(Catalogue* catalogue, const char* constProductName) {
    printf("\n--- Test Delete Product: %s ---\n", constProductName);
    char* productName = (char*)malloc(strlen(constProductName) + 1);
    strcpy(productName, constProductName);

    printf("Catalogue before deleting '%s':\n", productName);
    printCatalogue(catalogue);

    int delResult = deleteProduct(catalogue, productName);
    assert(delResult == 0); // Ensure the product was deleted successfully

    printf("\nCatalogue after deleting '%s':\n", productName);
    printCatalogue(catalogue);

    Product* retrievedProduct = getProduct(catalogue, productName);
    assert(retrievedProduct == NULL); // Ensure the product was not found

    free(productName);
}


void test_search_results(Catalogue* catalogue, char* searchQuery) {
    printf("\n--- Test Search Results for '%s' ---\n", searchQuery);
    printSearchResults(catalogue, searchQuery);
}

void test_delete_all_products(Catalogue* catalogue) {
	printf("\n--- Test Delete All Products ---\n");
	deleteAllProducts(catalogue);
	printf("\nCatalogue after deleting all products:\n");
	printCatalogue(catalogue);
}


int main() {
    Catalogue* catalogue = initCatalogue();
    assert(catalogue != NULL); // Ensure the catalogue was created successfully

    // Components for each product
    Component doorComponents[3] = { {"Wood", 10}, {"Nails", 100}, {"Paint", 5} };
    Component windowComponents[2] = { {"Glass", 4}, {"Frame", 8} };
    Component shelfComponents[4] = { {"Wood", 5}, {"Nails", 50}, {"Paint", 2}, {"Screws", 20} };
    Component shellComponents[3] = { {"Metal", 10}, {"Screws", 50}, {"Paint", 3} };

    test_add_product(catalogue, "Door 24", doorComponents, 3);

    test_add_product(catalogue, "Window", windowComponents, 2);

    char searchQuery1[] = "Doo";
    char searchQuery2[] = "Droor";
    char searchQuery3[] = "Wind";
    test_search_results(catalogue, searchQuery1);
    test_search_results(catalogue, searchQuery2);
    test_search_results(catalogue, searchQuery3);
    test_delete_product(catalogue, "Window");

    test_add_product(catalogue, "Shelf", shelfComponents, 4);
    test_add_product(catalogue, "Shell", shellComponents, 3);

    test_delete_product(catalogue, "Shelf");


    
    printf("\n\nFinal state of the Catalogue:\n");
    printCatalogue(catalogue);

    printf("\n\nAfter Deleting all products");
    deleteAllProducts(catalogue);
    printCatalogue(catalogue);
    free(catalogue);

    printf("\nAll tests passed.\n");
    return 0;
}