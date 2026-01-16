#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "library.h"

// Global variables
Book books[MAX_BOOKS];
IssuedBook issuedBooks[MAX_BOOKS];
int bookCount = 0;
int issuedCount = 0;

void initializeFiles() {
    FILE *file;
    
    // Create books file if it doesn't exist
    file = fopen(FILENAME_BOOKS, "rb");
    if (file == NULL) {
        file = fopen(FILENAME_BOOKS, "wb");
        if (file != NULL) {
            fclose(file);
        }
    } else {
        fclose(file);
    }
    
    // Create issued books file if it doesn't exist
    file = fopen(FILENAME_ISSUED, "rb");
    if (file == NULL) {
        file = fopen(FILENAME_ISSUED, "wb");
        if (file != NULL) {
            fclose(file);
        }
    } else {
        fclose(file);
    }
    
    // Create history file if it doesn't exist
    file = fopen(FILENAME_HISTORY, "rb");
    if (file == NULL) {
        file = fopen(FILENAME_HISTORY, "wb");
        if (file != NULL) {
            fclose(file);
        }
    } else {
        fclose(file);
    }
    
    // Load existing data
    loadBooks();
    loadIssuedBooks();
}

void loadBooks() {
    FILE *file = fopen(FILENAME_BOOKS, "rb");
    if (file == NULL) {
        bookCount = 0;
        return;
    }
    
    bookCount = 0;
    while(fread(&books[bookCount], sizeof(Book), 1, file) == 1) {
        bookCount++;
        if (bookCount >= MAX_BOOKS) break;
    }
    
    fclose(file);
}

void loadIssuedBooks() {
    FILE *file = fopen(FILENAME_ISSUED, "rb");
    if (file == NULL) {
        issuedCount = 0;
        return;
    }
    
    issuedCount = 0;
    while(fread(&issuedBooks[issuedCount], sizeof(IssuedBook), 1, file) == 1) {
        issuedCount++;
        if (issuedCount >= MAX_BOOKS) break;
    }
    
    fclose(file);
}

void saveBook(Book book) {
    FILE *file = fopen(FILENAME_BOOKS, "ab");
    if (file == NULL) {
        printf("Error opening file for saving book!\n");
        return;
    }
    
    fwrite(&book, sizeof(Book), 1, file);
    fclose(file);
    
    // Update the array
    books[bookCount] = book;
    bookCount++;
}

void saveIssuedBook(IssuedBook issuedBook) {
    FILE *file = fopen(FILENAME_ISSUED, "ab");
    if (file == NULL) {
        printf("Error opening file for saving issued book!\n");
        return;
    }
    
    fwrite(&issuedBook, sizeof(IssuedBook), 1, file);
    fclose(file);
    
    // Update the array
    issuedBooks[issuedCount] = issuedBook;
    issuedCount++;
}

int getNextBookId() {
    int maxId = 0;
    for (int i = 0; i < bookCount; i++) {
        if (books[i].id > maxId) {
            maxId = books[i].id;
        }
    }
    return maxId + 1;
}

int getNextIssueId() {
    int maxId = 0;
    for (int i = 0; i < issuedCount; i++) {
        if (issuedBooks[i].issueId > maxId) {
            maxId = issuedBooks[i].issueId;
        }
    }
    return maxId + 1;
}

void addBook() {
    Book newBook;
    char choice;
    
    printf("\n========== ADD NEW BOOK ==========\n");
    
    newBook.id = getNextBookId();
    
    printf("Enter Book Title: ");
    fgets(newBook.title, MAX_TITLE, stdin);
    newBook.title[strcspn(newBook.title, "\n")] = 0;
    
    printf("Enter Author: ");
    fgets(newBook.author, MAX_AUTHOR, stdin);
    newBook.author[strcspn(newBook.author, "\n")] = 0;
    
    printf("Enter ISBN: ");
    fgets(newBook.isbn, MAX_ISBN, stdin);
    newBook.isbn[strcspn(newBook.isbn, "\n")] = 0;
    
    printf("Enter Category: ");
    fgets(newBook.category, 50, stdin);
    newBook.category[strcspn(newBook.category, "\n")] = 0;
    
    printf("Enter Publication Year: ");
    scanf("%d", &newBook.year);
    
    printf("Enter Price: ");
    scanf("%f", &newBook.price);
    
    printf("Enter Quantity: ");
    scanf("%d", &newBook.quantity);
    
    newBook.available = newBook.quantity;
    
    // Clear input buffer
    while(getchar() != '\n');
    
    printf("\nBook Details:\n");
    printf("ID: %d\n", newBook.id);
    printf("Title: %s\n", newBook.title);
    printf("Author: %s\n", newBook.author);
    printf("ISBN: %s\n", newBook.isbn);
    printf("Category: %s\n", newBook.category);
    printf("Year: %d\n", newBook.year);
    printf("Price: %.2f\n", newBook.price);
    printf("Quantity: %d\n", newBook.quantity);
    
    printf("\nSave this book? (y/n): ");
    scanf("%c", &choice);
    
    if (tolower(choice) == 'y') {
        saveBook(newBook);
        printf("\nBook added successfully with ID: %d\n", newBook.id);
        
        // Add to history
        char details[200];
        sprintf(details, "Added book: %s (ID: %d)", newBook.title, newBook.id);
        addToHistory("ADD_BOOK", details);
    } else {
        printf("\nBook not saved.\n");
    }
}

void displayAllBooks() {
    printf("\n========== ALL BOOKS IN LIBRARY ==========\n");
    printf("Total Books: %d\n\n", bookCount);
    
    if (bookCount == 0) {
        printf("No books found in the library.\n");
        return;
    }
    
    printf("%-5s %-30s %-20s %-15s %-10s %-10s %-10s\n", 
           "ID", "Title", "Author", "Category", "Year", "Price", "Available");
    printf("---------------------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < bookCount; i++) {
        printf("%-5d %-30s %-20s %-15s %-10d $%-9.2f %-10d\n",
               books[i].id,
               books[i].title,
               books[i].author,
               books[i].category,
               books[i].year,
               books[i].price,
               books[i].available);
    }
}

void searchMenu() {
    int choice;
    
    do {
        printf("\n========== SEARCH BOOKS ==========\n");
        printf("1. Search by Title\n");
        printf("2. Search by Author\n");
        printf("3. Search by ISBN\n");
        printf("4. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        while(getchar() != '\n');
        
        switch(choice) {
            case 1:
                searchByTitle();
                break;
            case 2:
                searchByAuthor();
                break;
            case 3:
                searchByISBN();
                break;
            case 4:
                return;
            default:
                printf("Invalid choice!\n");
        }
    } while(choice != 4);
}

void searchByTitle() {
    char searchTitle[MAX_TITLE];
    int found = 0;
    
    printf("\nEnter title to search: ");
    fgets(searchTitle, MAX_TITLE, stdin);
    searchTitle[strcspn(searchTitle, "\n")] = 0;
    
    printf("\nSearch Results:\n");
    printf("%-5s %-30s %-20s %-15s %-10s %-10s\n", 
           "ID", "Title", "Author", "Category", "Year", "Available");
    printf("-----------------------------------------------------------------------------\n");
    
    for (int i = 0; i < bookCount; i++) {
        if (strstr(books[i].title, searchTitle) != NULL) {
            printf("%-5d %-30s %-20s %-15s %-10d %-10d\n",
                   books[i].id,
                   books[i].title,
                   books[i].author,
                   books[i].category,
                   books[i].year,
                   books[i].available);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No books found with title containing: %s\n", searchTitle);
    }
}

void searchByAuthor() {
    char searchAuthor[MAX_AUTHOR];
    int found = 0;
    
    printf("\nEnter author to search: ");
    fgets(searchAuthor, MAX_AUTHOR, stdin);
    searchAuthor[strcspn(searchAuthor, "\n")] = 0;
    
    printf("\nSearch Results:\n");
    printf("%-5s %-30s %-20s %-15s %-10s %-10s\n", 
           "ID", "Title", "Author", "Category", "Year", "Available");
    printf("-----------------------------------------------------------------------------\n");
    
    for (int i = 0; i < bookCount; i++) {
        if (strstr(books[i].author, searchAuthor) != NULL) {
            printf("%-5d %-30s %-20s %-15s %-10d %-10d\n",
                   books[i].id,
                   books[i].title,
                   books[i].author,
                   books[i].category,
                   books[i].year,
                   books[i].available);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No books found by author: %s\n", searchAuthor);
    }
}

void searchByISBN() {
    char searchISBN[MAX_ISBN];
    int found = 0;
    
    printf("\nEnter ISBN to search: ");
    fgets(searchISBN, MAX_ISBN, stdin);
    searchISBN[strcspn(searchISBN, "\n")] = 0;
    
    printf("\nSearch Results:\n");
    printf("%-5s %-30s %-20s %-15s %-10s %-10s %-10s\n", 
           "ID", "Title", "Author", "ISBN", "Category", "Year", "Available");
    printf("---------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < bookCount; i++) {
        if (strcmp(books[i].isbn, searchISBN) == 0) {
            printf("%-5d %-30s %-20s %-15s %-10s %-10d %-10d\n",
                   books[i].id,
                   books[i].title,
                   books[i].author,
                   books[i].isbn,
                   books[i].category,
                   books[i].year,
                   books[i].available);
            found = 1;
            break; // ISBN should be unique
        }
    }
    
    if (!found) {
        printf("No books found with ISBN: %s\n", searchISBN);
    }
}

int findBookById(int id) {
    for (int i = 0; i < bookCount; i++) {
        if (books[i].id == id) {
            return i;
        }
    }
    return -1;
}

int findBookByISBN(char *isbn) {
    for (int i = 0; i < bookCount; i++) {
        if (strcmp(books[i].isbn, isbn) == 0) {
            return i;
        }
    }
    return -1;
}

void updateBook() {
    int bookId, index;
    char choice;
    
    printf("\n========== UPDATE BOOK INFORMATION ==========\n");
    printf("Enter Book ID to update: ");
    scanf("%d", &bookId);
    while(getchar() != '\n');
    
    index = findBookById(bookId);
    if (index == -1) {
        printf("Book with ID %d not found!\n", bookId);
        return;
    }
    
    printf("\nCurrent Book Information:\n");
    printf("ID: %d\n", books[index].id);
    printf("Title: %s\n", books[index].title);
    printf("Author: %s\n", books[index].author);
    printf("ISBN: %s\n", books[index].isbn);
    printf("Category: %s\n", books[index].category);
    printf("Year: %d\n", books[index].year);
    printf("Price: %.2f\n", books[index].price);
    printf("Quantity: %d\n", books[index].quantity);
    printf("Available: %d\n", books[index].available);
    
    printf("\nWhat do you want to update?\n");
    printf("1. Title\n");
    printf("2. Author\n");
    printf("3. ISBN\n");
    printf("4. Category\n");
    printf("5. Year\n");
    printf("6. Price\n");
    printf("7. Quantity\n");
    printf("8. Cancel\n");
    printf("Enter your choice: ");
    
    scanf("%c", &choice);
    while(getchar() != '\n');
    
    switch(choice) {
        case '1':
            printf("Enter new title: ");
            fgets(books[index].title, MAX_TITLE, stdin);
            books[index].title[strcspn(books[index].title, "\n")] = 0;
            break;
        case '2':
            printf("Enter new author: ");
            fgets(books[index].author, MAX_AUTHOR, stdin);
            books[index].author[strcspn(books[index].author, "\n")] = 0;
            break;
        case '3':
            printf("Enter new ISBN: ");
            fgets(books[index].isbn, MAX_ISBN, stdin);
            books[index].isbn[strcspn(books[index].isbn, "\n")] = 0;
            break;
        case '4':
            printf("Enter new category: ");
            fgets(books[index].category, 50, stdin);
            books[index].category[strcspn(books[index].category, "\n")] = 0;
            break;
        case '5':
            printf("Enter new publication year: ");
            scanf("%d", &books[index].year);
            while(getchar() != '\n');
            break;
        case '6':
            printf("Enter new price: ");
            scanf("%f", &books[index].price);
            while(getchar() != '\n');
            break;
        case '7':
            printf("Enter new quantity: ");
            scanf("%d", &books[index].quantity);
            // Adjust available count
            books[index].available = books[index].quantity;
            while(getchar() != '\n');
            break;
        case '8':
            printf("Update cancelled.\n");
            return;
        default:
            printf("Invalid choice!\n");
            return;
    }
    
    // Save updated book list to file
    FILE *file = fopen(FILENAME_BOOKS, "wb");
    if (file == NULL) {
        printf("Error saving updated book information!\n");
        return;
    }
    
    fwrite(books, sizeof(Book), bookCount, file);
    fclose(file);
    
    printf("\nBook information updated successfully!\n");
    
    // Add to history
    char details[200];
    sprintf(details, "Updated book ID: %d", bookId);
    addToHistory("UPDATE_BOOK", details);
}

void deleteBook() {
    int bookId, index;
    char choice;
    
    printf("\n========== DELETE BOOK RECORD ==========\n");
    printf("Enter Book ID to delete: ");
    scanf("%d", &bookId);
    while(getchar() != '\n');
    
    index = findBookById(bookId);
    if (index == -1) {
        printf("Book with ID %d not found!\n", bookId);
        return;
    }
    
    printf("\nBook to delete:\n");
    printf("ID: %d\n", books[index].id);
    printf("Title: %s\n", books[index].title);
    printf("Author: %s\n", books[index].author);
    printf("ISBN: %s\n", books[index].isbn);
    
    printf("\nAre you sure you want to delete this book? (y/n): ");
    scanf("%c", &choice);
    
    if (tolower(choice) != 'y') {
        printf("Deletion cancelled.\n");
        return;
    }
    
    
    for (int i = 0; i < issuedCount; i++) {
        if (issuedBooks[i].bookId == bookId && !issuedBooks[i].isReturned) {
            printf("Cannot delete book! It is currently issued to %s\n", issuedBooks[i].borrowerName);
            return;
        }
    }
    
    
    for (int i = index; i < bookCount - 1; i++) {
        books[i] = books[i + 1];
    }
    bookCount--;
    
    FILE *file = fopen(FILENAME_BOOKS, "wb");
    if (file == NULL) {
        printf("Error saving updated book list!\n");
        return;
    }
    
    fwrite(books, sizeof(Book), bookCount, file);
    fclose(file);
    
    printf("\nBook deleted successfully!\n");
    
    // Add to history
    char details[200];
    sprintf(details, "Deleted book ID: %d", bookId);
    addToHistory("DELETE_BOOK", details);
}