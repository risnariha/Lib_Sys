
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "library.h"

void displayMenu();
void handleMenuChoice(int choice);

int main() {
    int choice;
    
    
    initializeFiles();
    
    printf("\n=============================================\n");
    printf("    LIBRARY MANAGEMENT SYSTEM\n");
    printf("=============================================\n");
    
    do {
        displayMenu();
        printf("Enter your choice (1-7): ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        if (choice >= 1 && choice <= 7) {
            handleMenuChoice(choice);
        } else if (choice != 8) {
            printf("\nInvalid choice! Please try again.\n");
        }
        
        if (choice != 8) {
            printf("\nPress Enter to continue...");
            getchar();
        }
        
    } while (choice != 8);
    
    printf("\nThank you for using Library Management System!\n");
    return 0;
}

void displayMenu() {
    printf("\n=============================================\n");
    printf("               MAIN MENU\n");
    printf("=============================================\n");
    printf("1. Add New Book\n");
    printf("2. Display All Books\n");
    printf("3. Search Book\n");
    printf("4. Update Book Information\n");
    printf("5. Delete Book Record\n");
    printf("6. Manage Book Issues/Returns\n");
    printf("7. Generate Reports\n");
    printf("8. Exit\n");
    printf("=============================================\n");
}

void handleMenuChoice(int choice) {
    switch(choice) {
        case 1:
            addBook();
            break;
        case 2:
            displayAllBooks();
            break;
        case 3:
            searchMenu();
            break;
        case 4:
            updateBook();
            break;
        case 5:
            deleteBook();
            break;
        case 6:
            issueReturnMenu();
            break;
        case 7:
            reportsMenu();
            break;
    }
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}