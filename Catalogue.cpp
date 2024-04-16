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


Node* createTrieNode(char character, Node* parent) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	if (newNode == NULL) {
		printf("\nMalloc failure in Creating Trie Node, Exiting Program");
		exit(EXIT_FAILURE);
	}
	newNode->character = character;
	newNode->parent = parent;  // Set the parent
	newNode->product = NULL;
	for (int i = 0; i < MAX_TRIE_ROOTS; i++) {
		newNode->children[i] = NULL;
	}
	return newNode;
}


int addProduct(Catalogue* catalogue, Product* product) {
	if (catalogue == NULL || product == NULL) {
		printf("\nCatalogue or Product is NULL");
		return -1;
	}

	int index = tolower(product->name[0]) - 'a';
	if (catalogue->roots[index] == NULL) {
		catalogue->roots[index] = createTrieNode(product->name[0], NULL);  // Root nodes have no parent
	}
	Node* currentNode = catalogue->roots[index];
	for (int i = 1; i < strlen(product->name); i++) {
		int childIndex = tolower(product->name[i]) - 'a';
		if (currentNode->children[childIndex] == NULL) {
			currentNode->children[childIndex] = createTrieNode(product->name[i], currentNode);
		}
		currentNode = currentNode->children[childIndex];
	}
	currentNode->product = product;

	return 0;
}


Product* getProduct(Catalogue* catalogue, char* target) { // Returns the product if it exists, NULL otherwise
	Node* node = navigateToNode(catalogue, target);
	if (node != NULL) {
		return node->product;
	}
	else {
		return NULL; // Return NULL if the product doesn't exist
	}

}


int deleteProduct(Catalogue* catalogue, char* target) { // Deletes the product if it exists, DOES NOT use freeTrieNode because we only want to delete the product. and freeTrieNode deletes the whole tree
	if (catalogue == NULL || target == NULL) {
		return -1;
	}
	Node* node = navigateToNode(catalogue, target);
	if (node == NULL || node->product == NULL) {
		return -1;
	}
	free(node->product); // Free the product
	node->product = NULL; // Set the product to NULL


	while (node != NULL && node->product == NULL) { // Traverse up the tree and delete nodes with no products
		bool hasChildren = false;
		for (int i = 0; i < MAX_TRIE_ROOTS; i++) { // Check if the node has any children, if it does, break
			if (node->children[i] != NULL) {
				hasChildren = true;
				break;
			}
		}
		if (hasChildren == false) { // If the node has no children, delete it
			Node* parent = node->parent;
			if (parent != NULL) {
				for (int i = 0; i < MAX_TRIE_ROOTS; i++) {
					if (parent->children[i] == node) {
						parent->children[i] = NULL;
						break;
					}
				}
			}
			else {

				catalogue->roots[tolower(target[0]) - 'a'] = NULL; // If the node is a root, set it to NULL
			}
			free(node);
			node = parent; // Move up the tree
		}
		else {
			break; // If the node has children, stop deleting
		}
	}
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


void printSearchResults(Catalogue* catalogue, char* searchPrefix) {
	Node* node = navigateToNode(catalogue, searchPrefix);
	if (node == NULL) {
		printf("\nNo matching prefix found for '%s'\n", searchPrefix);
		return;
	}

	// The prefix used here should include all characters leading up to but not including the node where the search stops. Mendokusai :))))))))))))).
	// As navigateToNode may stop at the exact node matching the end of searchPrefix, we need to ensure we don't double-print the last char.
	int len = strlen(searchPrefix);
	char prefix[NAME_LIMIT];
	if (len > 0) {
		strncpy(prefix, searchPrefix, len - 1); // Copy all but the last character
		prefix[len - 1] = '\0'; // Null-terminate early to avoid including last character
	}
	else {
		prefix[0] = '\0'; // In case of an empty searchPrefix, start with an empty prefix
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


void freeTrieNode(Node* node) { // Recursively free all the children of a node, and the node itself
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


// Things to do:

// 1.) Test for Bugs
// 2.) Test if Catalogue is working as expected
// 3.) Test if the Trie is working as expected
// 4.) Test if the Products are working as expected
// 5.) Integrate the Catalogue with the rest of the program