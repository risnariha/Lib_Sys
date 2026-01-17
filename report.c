#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"

extern Book books[MAX_BOOKS];
extern IssuedBook issuedBooks[MAX_BOOKS];
extern int bookCount;
extern int issuedCount;

void addToHistory(char *action, char *details) {
    FILE *file = fopen(FILENAME_HISTORY, "ab");
    if (file == NULL) {
        return;
    }
    
    char timestamp[20];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d", 
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    fprintf(file, "%s | %s | %s\n", timestamp, action, details);
    fclose(file);
}

void displayHistory() {
    FILE *file = fopen(FILENAME_HISTORY, "r");
    if (file == NULL) {
        printf("No history available.\n");
        return;
    }
    
    printf("\n========== SYSTEM HISTORY ==========\n");
    printf("%-20s %-15s %s\n", "Timestamp", "Action", "Details");
    printf("------------------------------------------------------------\n");
    
    char line[300];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    
    fclose(file);
}

void reportsMenu() {
    int choice;
    
    do {
        printf("\n========== REPORTS ==========\n");
        printf("1. Books Report\n");
        printf("2. Issued Books Report\n");
        printf("3. Popular Books Report\n");
        printf("4. System History\n");
        printf("5. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        while(getchar() != '\n');
        
        switch(choice) {
            case 1:
                generateBooksReport();
                break;
            case 2:
                generateIssuedReport();
                break;
            case 3:
                generatePopularBooks();
                break;
            case 4:
                displayHistory();
                break;
            case 5:
                return;
            default:
                printf("Invalid choice!\n");
        }
    } while(choice != 5);
}

void generateBooksReport() {
    int totalValue = 0;
    int totalBooks = 0;
    int booksByCategory[10] = {0};
    char categories[10][50];
    int categoryCount = 0;
    
    printf("\n========== BOOKS REPORT ==========\n");
    printf("Total Books in Library: %d\n", bookCount);