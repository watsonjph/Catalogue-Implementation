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
	newNode->parent = parent;
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

	int index = getIndex(product->name[0]);
	if (index == -1) {
		printf("\nInvalid character in product name"); // Invalid character in product name
		return -1;
	}

	if (catalogue->roots[index] == NULL) { // If the root node for the first character doesn't exist, create it
		catalogue->roots[index] = createTrieNode(product->name[0], NULL);
	}
	Node* currentNode = catalogue->roots[index];
	for (int i = 1; product->name[i] != '\0'; i++) { // Traverse the trie to add the product
		index = getIndex(product->name[i]);
		if (index == -1) {
			printf("\nInvalid character in product name");
			return -1;
		}
		if (currentNode->children[index] == NULL) { // If the node for the character doesn't exist, create it
			currentNode->children[index] = createTrieNode(product->name[i], currentNode);
		}
		currentNode = currentNode->children[index];
	}
	currentNode->product = product; // Set the product at the last node

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


int deleteProduct(Catalogue* catalogue, char* target) {
	if (catalogue == NULL || target == NULL) {
		return -1; // Input validation error.
	}

	int index = getIndex(target[0]);
	if (index == -1 || index >= MAX_TRIE_ROOTS) {
		return -1; // Invalid first character handling, means the product name is invalid.
	}

	Node* node = navigateToNode(catalogue, target);
	if (node == NULL || node->product == NULL) {
		return -1; // Product doesn't exist or was already deleted.
	}

	// Free the product and then set the pointer to NULL
	free(node->product);
	node->product = NULL;

	// Traverse up the tree and delete nodes with no products and no children
	while (node != NULL && node->product == NULL) {
		bool hasChildren = false;
		for (int i = 0; i < MAX_TRIE_ROOTS; i++) {
			if (node->children[i] != NULL) {
				hasChildren = true;
				break;
			}
		}

		if (hasChildren == false) {
			Node* parent = node->parent;
			if (parent != NULL) {
				for (int i = 0; i < MAX_TRIE_ROOTS; i++) {
					if (parent->children[i] == node) {
						parent->children[i] = NULL;
						free(node); // Free the node as it's no longer connected
						node = parent; // Move up to the parent
						break;
					}
				}
			}
			else {
				// Node is a root node
				catalogue->roots[index] = NULL;
				free(node);
				break; // Break since there's no parent to move up to.
			}
		}
		else {
			// If the node has children, we stop pruning
			break;
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

	// Check if all roots are NULL, indicating an empty catalogue
	bool isEmpty = true;
	for (int i = 0; i < MAX_TRIE_ROOTS; i++) {
		if (catalogue->roots[i] != NULL) {
			isEmpty = false;
			break;
		}
	}

	if (isEmpty) {
		printf("\nCatalogue is empty\n");
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

Node* navigateToNode(Catalogue* catalogue, char* str) {
	if (catalogue == NULL || str == NULL) {
		return NULL;
	}

	int index = getIndex(str[0]); // Get the index of the first character
	if (index == -1 || index >= MAX_TRIE_ROOTS) {
		return NULL;
	}

	Node* currentNode = catalogue->roots[index]; // Start at the root node for the first character
	if (currentNode == NULL) {
		return NULL;
	}

	for (int i = 1; str[i] != '\0'; i++) {
		index = getIndex(str[i]);
		if (index == -1 || index >= MAX_TRIE_ROOTS) {
			return NULL;
		}
		if (currentNode->children[index] == NULL) {
			return NULL;
		}
		currentNode = currentNode->children[index];
	}

	return currentNode; // Node found, return it
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


int getIndex(char c) {
	if (c == ' ') {
		return 26; // 26 for space
	}
	if (c >= '0' && c <= '9') {
		return 27 + (c - '0'); // Return 27-36 for '0'-'9'
	}
	c = tolower(c);
	if (c >= 'a' && c <= 'z') {
		return c - 'a';  // 0-25 for 'a'-'z'
	}
	return -1;  // Invalid character
}
