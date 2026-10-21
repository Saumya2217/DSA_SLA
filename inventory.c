#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 37
#define MAX_PRODUCTS 18
#define QUEUE_SIZE 10
#define STACK_SIZE 10

typedef struct DLLNode {
int id;
char name[50];
int quantity;
float price;
struct DLLNode *prev, *next;
} DLLNode;

typedef struct BSTNode {
DLLNode *product;
struct BSTNode *left, *right;
} BSTNode;

typedef struct {
int id;
int quantity;
} Request;

/* The DLL is the main inventory. Other structures refer to product IDs
or DLL records, so inventory changes remain consistent. */
DLLNode *dllHead = NULL, *dllTail = NULL;
int productCount = 0;

Request restockQueue[QUEUE_SIZE];
int restockFront = 0, restockRear = -1;

Request checkoutQueue[QUEUE_SIZE];
int checkoutFront = 0, checkoutCount = 0;

int productStack[STACK_SIZE], stackTop = -1;

BSTNode *bstRoot = NULL;
DLLNode *hashTable[HASH_SIZE];

/* ---------- DLL / INVENTORY ---------- */

DLLNode *findProduct(int id) {
DLLNode *p = dllHead;
while (p != NULL) {
if (p->id == id) return p;
p = p->next;
}
return NULL;
}

DLLNode *createProduct(int id, const char *name, int quantity, float price) {
DLLNode *p = (DLLNode *)malloc(sizeof(DLLNode));
if (p == NULL) {
printf("Memory allocation failed.\n");
return NULL;
}
p->id = id;
snprintf(p->name, sizeof(p->name), "%s", name);
p->quantity = quantity;
p->price = price;
p->prev = p->next = NULL;
return p;
}

void appendProduct(DLLNode *p) {
if (dllHead == NULL) {
dllHead = dllTail = p;
} else {
dllTail->next = p;
p->prev = dllTail;
dllTail = p;
}
productCount++;
}

void pushRecent(int id) {
if (stackTop == STACK_SIZE - 1) {
for (int i = 1; i <= stackTop; i++)
productStack[i - 1] = productStack[i];
stackTop--;
}
productStack[++stackTop] = id;
}

/* ---------- BST ---------- */

BSTNode *insertBST(BSTNode *root, DLLNode *p) {
if (root == NULL) {
BSTNode *n = (BSTNode *)malloc(sizeof(BSTNode));
if (n == NULL) {
printf("Memory allocation failed while building BST.\n");
exit(EXIT_FAILURE);
}
n->product = p;
n->left = n->right = NULL;
return n;
}
if (p->id < root->product->id)
root->left = insertBST(root->left, p);
else if (p->id > root->product->id)
root->right = insertBST(root->right, p);
return root;
}

BSTNode *searchBST(BSTNode *root, int id) {
if (root == NULL || root->product->id == id) return root;
if (id < root->product->id) return searchBST(root->left, id);
return searchBST(root->right, id);
}

void displayBST(BSTNode *root) {
if (root == NULL) return;
displayBST(root->left);
printf("ID: %d | Name: %s | Quantity: %d | Price: Rs. %.2f\n",
root->product->id, root->product->name,
root->product->quantity, root->product->price);
displayBST(root->right);
}

void freeBST(BSTNode *root) {
if (root == NULL) return;
freeBST(root->left);
freeBST(root->right);
free(root);
}

/* ---------- HASH TABLE: LINEAR PROBING ---------- */

int hashFunction(int id) {
int index = id % HASH_SIZE;
return index < 0 ? index + HASH_SIZE : index;
}

void clearHash(void) {
for (int i = 0; i < HASH_SIZE; i++) hashTable[i] = NULL;
}

int insertHash(DLLNode *p) {
int index = hashFunction(p->id);
for (int i = 0; i < HASH_SIZE; i++) {
if (hashTable[index] == NULL) {
hashTable[index] = p;
return 1;
}
index = (index + 1) % HASH_SIZE;
}
return 0;
}

DLLNode *searchHash(int id) {
int index = hashFunction(id);
for (int i = 0; i < HASH_SIZE; i++) {
if (hashTable[index] == NULL) return NULL;
if (hashTable[index]->id == id) return hashTable[index];
index = (index + 1) % HASH_SIZE;
}
return NULL;
}

void rebuildIndexes(void) {
freeBST(bstRoot);
bstRoot = NULL;
clearHash();

DLLNode *p = dllHead;
while (p != NULL) {
bstRoot = insertBST(bstRoot, p);
insertHash(p);
p = p->next;
}
}

/* ---------- ADD / INSERT / DELETE / DISPLAY ---------- */

void addProduct(void) {
int id, quantity;
float price;
char name[50];

if (productCount >= MAX_PRODUCTS) {
printf("Inventory is full (maximum %d products).\n", MAX_PRODUCTS);
return;
}

printf("Enter Product ID: ");
if (scanf("%d", &id) != 1) { printf("Invalid ID.\n"); return; }
if (findProduct(id) != NULL) {
printf("Product ID already exists.\n");
return;
}

printf("Enter Product Name: ");
scanf(" %49[^\n]", name);
printf("Enter Quantity: ");
if (scanf("%d", &quantity) != 1 || quantity < 0) {
printf("Quantity must be zero or greater.\n");
return;
}
printf("Enter Unit Price: ");
if (scanf("%f", &price) != 1 || price < 0) {
printf("Price must be zero or greater.\n");
return;
}

DLLNode *p = createProduct(id, name, quantity, price);
if (p == NULL) return;

appendProduct(p);
pushRecent(id);
rebuildIndexes();
printf("Product added. DLL, Stack, BST and Hash Table updated.\n");
}

void insertAfterProduct(void) {
int targetID, id, quantity;
float price;
char name[50];

if (productCount >= MAX_PRODUCTS) {
printf("Inventory is full.\n");
return;
}
if (dllHead == NULL) {
printf("Inventory is empty. Add a product first.\n");
return;
}

printf("Insert after Product ID: ");
if (scanf("%d", &targetID) != 1) { printf("Invalid ID.\n"); return; }
DLLNode *target = findProduct(targetID);
if (target == NULL) {
printf("Selected product not found.\n");
return;
}

printf("Enter new Product ID: ");
if (scanf("%d", &id) != 1) { printf("Invalid ID.\n"); return; }
if (findProduct(id) != NULL) {
printf("Product ID already exists.\n");
return;
}

printf("Enter Product Name: ");
scanf(" %49[^\n]", name);
printf("Enter Quantity: ");
if (scanf("%d", &quantity) != 1 || quantity < 0) {
printf("Quantity must be zero or greater.\n");
return;
}
printf("Enter Unit Price: ");
if (scanf("%f", &price) != 1 || price < 0) {
printf("Price must be zero or greater.\n");
return;
}

DLLNode *p = createProduct(id, name, quantity, price);
if (p == NULL) return;

p->prev = target;
p->next = target->next;
if (target->next != NULL) target->next->prev = p;
else dllTail = p;
target->next = p;
productCount++;

pushRecent(id);
rebuildIndexes();
printf("Inserted after ID %d. BST and Hash Table synchronized.\n", targetID);
printf("Pending queue order is unchanged.\n");
}

void displayInventory(void) {
DLLNode *p = dllHead;
if (p == NULL) { printf("Inventory is empty.\n"); return; }

printf("\n========== INVENTORY (DLL) ==========\n");
while (p != NULL) {
printf("ID: %d | Name: %s | Quantity: %d | Price: Rs. %.2f\n",
p->id, p->name, p->quantity, p->price);
p = p->next;
}
}

void displayInventoryReverse(void) {
DLLNode *p = dllTail;
if (p == NULL) { printf("Inventory is empty.\n"); return; }

printf("\n========== INVENTORY (REVERSE DLL) ==========\n");
while (p != NULL) {
printf("ID: %d | Name: %s | Quantity: %d | Price: Rs. %.2f\n",
p->id, p->name, p->quantity, p->price);
p = p->prev;
}
}

void deleteProduct(void) {
int id;
printf("Enter Product ID to delete: ");
if (scanf("%d", &id) != 1) { printf("Invalid ID.\n"); return; }

DLLNode *p = findProduct(id);
if (p == NULL) { printf("Product not found.\n"); return; }

if (p->prev != NULL) p->prev->next = p->next;
else dllHead = p->next;
if (p->next != NULL) p->next->prev = p->prev;
else dllTail = p->prev;

free(p);
productCount--;
rebuildIndexes();
printf("Product deleted. BST and Hash Table updated.\n");
printf("Queued requests for this ID will be rejected if processed later.\n");
}

/* ---------- STACK ---------- */

void displayRecentProducts(void) {
if (stackTop == -1) { printf("Recent-products stack is empty.\n"); return; }

printf("\n========== RECENTLY ADDED PRODUCTS (LIFO) ==========\n");
for (int i = stackTop; i >= 0; i--) {
DLLNode *p = findProduct(productStack[i]);
if (p != NULL) printf("ID: %d | Name: %s\n", p->id, p->name);
else printf("ID: %d | Product has been deleted\n", productStack[i]);
}
}

void popRecentProduct(void) {
if (stackTop == -1) { printf("Stack is empty.\n"); return; }
printf("Removed ID %d from recent-products history only.\n",
productStack[stackTop--]);
printf("Inventory record was not deleted.\n");
}

/* ---------- LINEAR QUEUE: RESTOCK ---------- */

int restockEmpty(void) {
return restockRear < restockFront;
}

void enqueueRestock(void) {
int id, quantity;

if (restockRear == QUEUE_SIZE - 1 && restockFront > 0) {
int count = restockRear - restockFront + 1;
for (int i = 0; i < count; i++)
restockQueue[i] = restockQueue[restockFront + i];
restockFront = 0;
restockRear = count - 1;
}
if (restockRear == QUEUE_SIZE - 1) {
printf("Restock queue is full.\n");
return;
}

printf("Enter Product ID to restock: ");
if (scanf("%d", &id) != 1) { printf("Invalid ID.\n"); return; }
if (findProduct(id) == NULL) {
printf("Product not found. Add it to inventory first.\n");
return;
}

printf("Enter quantity to restock: ");
if (scanf("%d", &quantity) != 1 || quantity <= 0) {
printf("Quantity must be positive.\n");
return;
}

restockQueue[++restockRear].id = id;
restockQueue[restockRear].quantity = quantity;
printf("Restock request added to Linear Queue.\n");
}

void processRestock(void) {
if (restockEmpty()) {
printf("No restock requests pending.\n");
restockFront = 0;
restockRear = -1;
return;
}

Request r = restockQueue[restockFront++];
DLLNode *p = findProduct(r.id);
if (p == NULL) {
printf("Request rejected: product ID %d no longer exists.\n", r.id);
} else {
p->quantity += r.quantity;
printf("Restock processed for %s. Added %d; new stock: %d.\n",
p->name, r.quantity, p->quantity);
}

if (restockEmpty()) {
restockFront = 0;
restockRear = -1;
}
}

void displayRestockQueue(void) {
if (restockEmpty()) { printf("Restock queue is empty.\n"); return; }

printf("\n========== RESTOCK QUEUE (FIFO) ==========\n");
for (int i = restockFront; i <= restockRear; i++) {
DLLNode *p = findProduct(restockQueue[i].id);
printf("%d. ID: %d | Name: %s | Quantity to add: %d\n",
i - restockFront + 1, restockQueue[i].id,
p != NULL ? p->name : "(deleted product)",
restockQueue[i].quantity);
}
}

/* ---------- CIRCULAR QUEUE: CHECKOUT ---------- */

void enqueueCheckout(void) {
int id, quantity;

if (checkoutCount == QUEUE_SIZE) {
printf("Checkout queue is full.\n");
return;
}

printf("Enter Product ID to purchase: ");
if (scanf("%d", &id) != 1) { printf("Invalid ID.\n"); return; }
DLLNode *p = findProduct(id);
if (p == NULL) { printf("Product not found.\n"); return; }

printf("Enter quantity to buy: ");
if (scanf("%d", &quantity) != 1 || quantity <= 0) {
printf("Quantity must be positive.\n");
return;
}

int rear = (checkoutFront + checkoutCount) % QUEUE_SIZE;
checkoutQueue[rear].id = id;
checkoutQueue[rear].quantity = quantity;
checkoutCount++;
printf("Checkout request added to Circular Queue.\n");
}

void processCheckout(void) {
if (checkoutCount == 0) {
printf("No checkout requests pending.\n");
return;
}

Request r = checkoutQueue[checkoutFront];
checkoutFront = (checkoutFront + 1) % QUEUE_SIZE;
checkoutCount--;

DLLNode *p = findProduct(r.id);
if (p == NULL) {
printf("Checkout rejected: product ID %d no longer exists.\n", r.id);
return;
}
if (p->quantity < r.quantity) {
printf("Checkout rejected: insufficient stock. Available: %d, requested: %d.\n",
p->quantity, r.quantity);
return;
}

p->quantity -= r.quantity;
printf("Checkout successful: %s | Qty: %d | Total: Rs. %.2f | Remaining: %d\n",
p->name, r.quantity, p->price * r.quantity, p->quantity);
}

void displayCheckoutQueue(void) {
if (checkoutCount == 0) { printf("Checkout queue is empty.\n"); return; }

printf("\n========== CHECKOUT QUEUE (FIFO) ==========\n");
for (int i = 0; i < checkoutCount; i++) {
int index = (checkoutFront + i) % QUEUE_SIZE;
DLLNode *p = findProduct(checkoutQueue[index].id);
printf("%d. ID: %d | Name: %s | Quantity requested: %d\n",
i + 1, checkoutQueue[index].id,
p != NULL ? p->name : "(deleted product)",
checkoutQueue[index].quantity);
}
}

/* ---------- HASH TABLE / BST SEARCH ---------- */

void searchProductHash(void) {
int id;
printf("Enter Product ID to search using Hash Table: ");
if (scanf("%d", &id) != 1) { printf("Invalid ID.\n"); return; }

DLLNode *p = searchHash(id);
if (p == NULL) { printf("Product not found.\n"); return; }
printf("Found: ID: %d | Name: %s | Quantity: %d | Price: Rs. %.2f\n",
p->id, p->name, p->quantity, p->price);
}

void displayHashTable(void) {
printf("\n========== HASH TABLE (LINEAR PROBING) ==========\n");
for (int i = 0; i < HASH_SIZE; i++) {
if (hashTable[i] != NULL)
printf("Index %d -> ID: %d | Name: %s | Quantity: %d\n",
i, hashTable[i]->id, hashTable[i]->name,
hashTable[i]->quantity);
else
printf("Index %d -> Empty\n", i);
}
}


/* ---------- SINGLE MENU WITH STAGE LABELS ---------- */

int main(void) {
int choice;

do {
printf("\n=======================================================\n");
printf("             INVENTORY MANAGEMENT SYSTEM\n");
printf("=======================================================\n");

printf("\n--- STAGE 1: STACK AND QUEUES ---\n");
printf(" 1. Add Restock Request (Linear Queue)\n");
printf(" 2. Process Restock Request\n");
printf(" 3. Display Restock Queue\n");
printf(" 4. Add Checkout Request (Circular Queue)\n");
printf(" 5. Process Checkout Request\n");
printf(" 6. Display Checkout Queue\n");
printf(" 7. Display Recently Added Products (Stack)\n");
printf(" 8. Pop Recent-Products Stack\n");

printf("\n--- STAGE 2: DOUBLY LINKED LIST AND BST ---\n");
printf(" 9. Add Product\n");
printf("10. Insert Product After Selected Product\n");
printf("11. Display Inventory (Forward DLL)\n");
printf("12. Display Inventory (Reverse DLL)\n");
printf("13. Delete Product\n");
printf("14. Display Inventory Sorted by ID (BST)\n");
printf("15. Search Product in BST\n");

printf("\n--- STAGE 3: HASH TABLE ---\n");
printf("16. Search Product using Hash Table\n");
printf("17. Display Hash Table\n");

printf("\n 0. Exit\n");
printf("Enter choice: ");

if (scanf("%d", &choice) != 1) {
printf("Invalid input. Exiting.\n");
break;
}

switch (choice) {
/* STAGE 1 */
case 1: enqueueRestock(); break;
case 2: processRestock(); break;
case 3: displayRestockQueue(); break;
case 4: enqueueCheckout(); break;
case 5: processCheckout(); break;
case 6: displayCheckoutQueue(); break;
case 7: displayRecentProducts(); break;
case 8: popRecentProduct(); break;

/* STAGE 2 */
case 9: addProduct(); break;
case 10: insertAfterProduct(); break;
case 11: displayInventory(); break;
case 12: displayInventoryReverse(); break;
case 13: deleteProduct(); break;
case 14:
if (bstRoot == NULL)
printf("BST is empty. Add products first.\n");
else
displayBST(bstRoot);
break;
case 15: {
int id;
printf("Enter Product ID to search in BST: ");
if (scanf("%d", &id) != 1) {
printf("Invalid ID.\n");
while (getchar() != '\n');
break;
}
BSTNode *result = searchBST(bstRoot, id);
if (result != NULL) {
DLLNode *product = result->product;
printf("Found: ID: %d | Name: %s | Quantity: %d | Price: Rs. %.2f\n",
product->id, product->name,
product->quantity, product->price);
} else {
printf("Product not found in BST.\n");
}
break;
}

/* STAGE 3 */
case 16: searchProductHash(); break;
case 17: displayHashTable(); break;

case 0:
printf("Exiting Inventory Management System.\n");
break;
default:
printf("Invalid choice. Try again.\n");
}
} while (choice != 0);

freeBST(bstRoot);
DLLNode *p = dllHead;
while (p != NULL) {
DLLNode *temp = p;
p = p->next;
free(temp);
}
return 0;
}

/*
FUNCTIONAL MAPPING
------------------
Linear Queue: stores restock requests; processing increases stock in the DLL.
Circular Queue: stores checkout requests; processing validates stock and
decreases inventory only if sufficient stock exists.
Stack: records recently added product IDs. Popping history does not delete
the product from inventory.
Doubly Linked List: main inventory; supports forward/reverse display,
insert-after, and deletion.
BST: indexes the DLL records by ID and displays them in ascending ID order.
Hash Table: modulo hashing with linear probing; indexes the same DLL records.

Inserting after a product changes the DLL order, but it does not change the
order of pending requests in either queue. BST and hash indexes are rebuilt
after additions, insertions, and deletions to keep them synchronized.
*/
