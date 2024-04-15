#include "Catalogue.h"


Catalogue* initCatalogue() {
	Catalogue* newCatalogue = (Catalogue*)malloc(sizeof(Catalogue));
	if (newCatalogue == NULL) {
		printf("\nMalloc failure in Initializing Catalogue, Exiting Program");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < MAX_TRIE_ROOTS; i++) {
		newCatalogue->roots[i] = NULL; // roots to NULL
	}

	return newCatalogue;
}


Product* createProduct(char* name, Component* arrayOfComponents) { // Creates a new product with the given name and components
	Product* newProduct = (Product*)malloc(sizeof(Product));
	if (newProduct == NULL) {
		printf("\nMalloc failure in Creating Product, Exiting Program");
		exit(EXIT_FAILURE);
	}

	strncpy(newProduct->name, name, NAME_LIMIT - 1);
	newProduct->name[NAME_LIMIT - 1] = '\0'; // Ensure null-terminated string

	for (int i = 0; i < COMPONENT_LIMIT; i++) {
		newProduct->requiredComponents[i] = arrayOfComponents[i]; // Copy the array of components
	}

	return newProduct;
}


Node* createTrieNode(char character) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	if (newNode == NULL) {
		printf("\nMalloc failure in Creating Trie Node, Exiting Program");
		exit(EXIT_FAILURE);
	}

	newNode->character = character;
	newNode->product = NULL;
	for (int i = 0; i < MAX_TRIE_ROOTS; i++) { // Set all children to NULL
		newNode->children[i] = NULL;
	}

	return newNode;
}


int addProduct(Catalogue* catalogue, Product* product) {
	if (catalogue == NULL || product == NULL) {  // You can change this, so that the caller handles the printing of the error message to follow Single Responsibility Principle
		printf("\nCatalogue or Product is NULL");
		return -1;            // Either this or exit(EXIT_FAILURE) depending on how you want to handle the error
	}

	int index = tolower(product->name[0]) - 'a'; // Get the index of the root node
	if (catalogue->roots[index] == NULL) { // If the root node doesn't exist, create it
		catalogue->roots[index] = createTrieNode(product->name[0]);
	}

	Node* currentNode = catalogue->roots[index]; // Start at the root node
	for (int i = 1; i < strlen(product->name); i++) { // For each character in the product name
		int childIndex = tolower(product->name[i]) - 'a'; // Get the index of the child node
		if (currentNode->children[childIndex] == NULL) { // If the child node doesn't exist, create it
			currentNode->children[childIndex] = createTrieNode(product->name[i]);
		}
		currentNode = currentNode->children[childIndex]; // Move to the child node
	}

	currentNode->product = product; // Set the product at the last node

	return 0; // 0 indicates success in this implementation, can change later for a more uniform format
}


Product* getProduct(Catalogue* catalogue, char* target) {
	Node* node = navigateToNode(catalogue, target);
	if (node != NULL) {
		return node->product;
	}
	else {
		return NULL; // Return NULL if the product doesn't exist
	}

}


int deleteProduct(Catalogue* catalogue, char* target) {
	if (catalogue == NULL || target == NULL) {
		return -1;
	}
	Node* node = navigateToNode(catalogue, target);
	if (node == NULL || node->product == NULL) {
		return -1;
	}
	free(node->product);  // Free the product
	node->product = NULL; // Set to NULL after freeing

	// Optional - Further pruning logic would be necessary here if we need to remove empty branches
	return 0;
}


int deleteAllProducts(Catalogue* catalogue) {
	if (catalogue == NULL) {
		return -1;
	}
	for (int i = 0; i < MAX_TRIE_ROOTS; i++) { // Free all the roots and their children
		freeTrieNode(catalogue->roots[i]);
		catalogue->roots[i] = NULL;
	}
	return 0;
}


void printSearchResults(Catalogue* catalogue, char* prefix) {
	Node* node = navigateToNode(catalogue, prefix);
	if (node == NULL) {
		printf("\nNo matching prefix found in printSearchResults"); // You can change this no not print anything if you prefer, I just added it for debugging
		return;
	}
	printFromNode(node, prefix);
}


void printCatalogue(Catalogue* catalogue) {
	if (catalogue == NULL) {
		printf("\nCatalogue is NULL");
		return;
	}
	printf("Catalogue Products:\n");
	char initialPrefix[NAME_LIMIT] = ""; // Initialize with an empty string so that the product name is printed correctly
	for (int i = 0; i < MAX_TRIE_ROOTS; i++) {
		if (catalogue->roots[i] != NULL) {
			printFromNode(catalogue->roots[i], initialPrefix);
		}
	}
}

// Helper functions to prevent code duplication that I implemented, you can choose to use them or not, I just prefer to keep the code shorter for readability

Node* navigateToNode(Catalogue* catalogue, char* str) { // Basically navigate to the node that corresponds to the given string
	if (catalogue == NULL || str == NULL) {
		return NULL;
	}

	Node* currentNode = catalogue->roots[tolower(str[0]) - 'a'];
	for (int i = 1; currentNode != NULL && str[i] != '\0'; i++) {
		currentNode = currentNode->children[tolower(str[i]) - 'a'];
	}
	return currentNode;
}


void freeTrieNode(Node* node) { // Recursively free all the children of a node
	if (node == NULL) { // base case
		return;
	}
	for (int i = 0; i < MAX_TRIE_ROOTS; i++) {
		freeTrieNode(node->children[i]);
	}
	free(node->product);  // Ensure to free the product if it exists
	free(node);
}


void printFromNode(Node* node, char* prefix) { // Recursively print all the products from a node
	if (node == NULL) {
		return;
	}

	char newPrefix[NAME_LIMIT]; // Create a new prefix to pass to the children
	snprintf(newPrefix, sizeof(newPrefix), "%s%c", prefix, node->character); // Append the character to the prefix, maybe better than strncat

	// Check if this node has a product and print details
	if (node->product != NULL) { 
		printf("%s: ", newPrefix); // Print the complete product name built so far
		for (int i = 0; i < COMPONENT_LIMIT; i++) {
			// Make sure the component has a valid name and quantity before printing
			if (node->product->requiredComponents[i].name[0] != '\0' && node->product->requiredComponents[i].qty > 0) { // Check if the component is valid
				printf("%s (%d units), ", node->product->requiredComponents[i].name, node->product->requiredComponents[i].qty);
			}
		}
		printf("\n");
	}

	// Recursively print children nodes with the updated prefix
	for (int i = 0; i < MAX_TRIE_ROOTS; i++) {
		if (node->children[i] != NULL) {
			printFromNode(node->children[i], newPrefix); // Use newPrefix to continue the path
		}
	}
}

// ???????????????????????