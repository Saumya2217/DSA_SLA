# Inventory Management System — DSA SLA

A menu-driven C program demonstrating stacks, queues, a doubly linked list, a binary search tree, and a hash table through inventory operations.

## Features

| Data structure | Purpose |
| --- | --- |
| Doubly linked list | Stores inventory; supports addition, insertion after a selected product, deletion, and forward/reverse display. |
| Stack | Tracks recently added product IDs. Popping removes history only. |
| Linear queue | Holds restock requests; processing increases stock. |
| Circular queue | Holds checkout requests; processing checks available stock and deducts purchased quantities. |
| Binary search tree | Searches products by ID and displays inventory in ascending ID order. |
| Hash table | Searches product IDs using modulo hashing and linear probing. |

The linked list is the main inventory. The BST and hash table refer to its product records and are rebuilt after additions, insertions, and deletions.

## Build and run

Requires GCC with C11 support. On Windows, use a GCC installation such as MinGW.

```powershell
git clone https://github.com/Saumya2217/DSA_SLA.git
cd DSA_SLA
gcc -std=c11 -Wall -Wextra -pedantic inventory.c -o inventory.exe
.\inventory.exe
```

On Linux or macOS with GCC installed:

```sh
gcc -std=c11 -Wall -Wextra -pedantic inventory.c -o inventory
./inventory
```

Choose **9** to add a product before creating restock or checkout requests. Choose **0** to exit.

## Example run

A sample run added product `101` (Notebook), with quantity `10` and unit price `Rs. 50.00`, then:

1. Restocked 5 units, increasing stock to 15.
2. Purchased 3 units for Rs. 150.00, leaving 12.
3. Found the product through the hash table with quantity 12.
4. Exited with status 0.

The complete captured output is in [sample-run.txt](sample-run.txt). This sample covers those operations only. The program compiled on Windows with GCC using the flags above and emitted no warnings.

## Limits

- Inventory supports up to 18 products.
- Each request queue holds up to 10 requests; recent-product history holds up to 10 IDs.
- All data is held in memory and is lost when the program exits.
- Interactive numeric input is expected; invalid input can end the session.

## Files

- [inventory.c](inventory.c): source copied unchanged from the supplied `Data Structure SLA.txt`.
- [sample-run.txt](sample-run.txt): captured sample session.
- [.gitignore](.gitignore): excludes local build artifacts.
