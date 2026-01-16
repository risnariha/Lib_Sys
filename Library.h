#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "library.h"

extern Book books[MAX_BOOKS];
extern IssuedBook issuedBooks[MAX_BOOKS];
extern int bookCount;
extern int issuedCount;

void issueReturnMenu() {
    int choice;
    
    do {
        printf("\n========== ISSUE/RETURN MANAGEMENT ==========\n");
        printf("1. Issue Book\n");
        printf("2. Return Book\n");
        printf("3. View Currently Issued Books\n");
        printf("4. View Overdue Books\n");
        printf("5. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        while(getchar() != '\n');
        
        switch(choice) {
            case 1:
                issueBook();
                break;
            case 2:
                returnBook();
                break;
            case 3:
                displayIssuedBooks();
                break;
            case 4:
                displayOverdueBooks();
                break;
            case 5:
                return;
            default:
                printf("Invalid choice!\n");
        }
    } while(choice != 5);
}

void getCurrentDate(char *dateStr) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(dateStr, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

void issueBook() {
    int bookId, index;
    IssuedBook newIssue;
    
    printf("\n========== ISSUE BOOK ==========\n");
    printf("Enter Book ID to issue: ");
    scanf("%d", &bookId);
    while(getchar() != '\n');
    
    index = findBookById(bookId);
    if (index == -1) {
        printf("Book with ID %d not found!\n", bookId);
        return;
    }
    
    if (books[index].available <= 0) {
        printf("Sorry, this book is not available for issue.\n");
        return;
    }
    
    newIssue.issueId = getNextIssueId();
    newIssue.bookId = bookId;
    strcpy(newIssue.bookTitle, books[index].title);
    
    printf("Enter Borrower Name: ");
    fgets(newIssue.borrowerName, 100, stdin);
    newIssue.borrowerName[strcspn(newIssue.borrowerName, "\n")] = 0;
    
    printf("Enter Borrower ID: ");
    fgets(newIssue.borrowerId, 20, stdin);
    newIssue.borrowerId[strcspn(newIssue.borrowerId, "\n")] = 0;
    
    getCurrentDate(newIssue.issueDate);
    
   
    time_t t = time(NULL);
    t += 14 * 24 * 60 * 60; 
    struct tm *tm = localtime(&t);
    sprintf(newIssue.returnDate, "%04d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    
    strcpy(newIssue.actualReturnDate, "");
    newIssue.isReturned = 0;
    newIssue.fine = 0.0;
    
    
    books[index].available--;
    
    
    saveIssuedBook(newIssue);
    
    
    FILE *file = fopen(FILENAME_BOOKS, "wb");
    if (file != NULL) {
        fwrite(books, sizeof(Book), bookCount, file);
        fclose(file);
    }
    
    printf("\nBook issued successfully!\n");
    printf("Issue ID: %d\n", newIssue.issueId);
    printf("Book: %s\n", newIssue.bookTitle);
    printf("Borrower: %s\n", newIssue.borrowerName);
    printf("Issue Date: %s\n", newIssue.issueDate);
    printf("Due Date: %s\n", newIssue.returnDate);
    
    
    char details[200];
    sprintf(details, "Issued book ID %d to %s (Issue ID: %d)", bookId, newIssue.borrowerName, newIssue.issueId);
    addToHistory("ISSUE_BOOK", details);
}

void returnBook() {
    int issueId, issueIndex = -1, bookIndex;
    float finePerDay = 1.0; 
    char currentDate[20];
    struct tm dueDate, currentTm;
    time_t dueTime, currentTime;
    int daysOverdue = 0;
    float fine = 0.0;
    
    printf("\n========== RETURN BOOK ==========\n");
    printf("Enter Issue ID to return: ");
    scanf("%d", &issueId);
    while(getchar() != '\n');
    
    
    for (int i = 0; i < issuedCount; i++) {
        if (issuedBooks[i].issueId == issueId && !issuedBooks[i].isReturned) {
            issueIndex = i;
            break;
        }
    }
    
    if (issueIndex == -1) {
        printf("Issue ID %d not found or book already returned!\n", issueId);
        return;
    }
    
    
    getCurrentDate(currentDate);
    strcpy(issuedBooks[issueIndex].actualReturnDate, currentDate);
    
    
    sscanf(issuedBooks[issueIndex].returnDate, "%d-%d-%d", 
           &dueDate.tm_year, &dueDate.tm_mon, &dueDate.tm_mday);
    dueDate.tm_year -= 1900;
    dueDate.tm_mon -= 1;
    dueDate.tm_hour = 0;
    dueDate.tm_min = 0;
    dueDate.tm_sec = 0;
    dueDate.tm_isdst = -1;
    
    sscanf(currentDate, "%d-%d-%d", 
           &currentTm.tm_year, &currentTm.tm_mon, &currentTm.tm_mday);
    currentTm.tm_year -= 1900;
    currentTm.tm_mon -= 1;
    currentTm.tm_hour = 0;
    currentTm.tm_min = 0;
    currentTm.tm_sec = 0;
    currentTm.tm_isdst = -1;
    
    dueTime = mktime(&dueDate);
    currentTime = mktime(&currentTm);
    
    if (currentTime > dueTime) {
        daysOverdue = (int)((currentTime - dueTime) / (24 * 60 * 60));
        fine = daysOverdue * finePerDay;
        issuedBooks[issueIndex].fine = fine;
        printf("Book is %d days overdue. Fine: $%.2f\n", daysOverdue, fine);
    }
    

    issuedBooks[issueIndex].isReturned = 1;
    
    
    bookIndex = findBookById(issuedBooks[issueIndex].bookId);
    if (bookIndex != -1) {
        books[bookIndex].available++;
    }
    
    
    FILE *file = fopen(FILENAME_ISSUED, "wb");
    if (file != NULL) {
        fwrite(issuedBooks, sizeof(IssuedBook), issuedCount, file);
        fclose(file);
    }
    
    
    file = fopen(FILENAME_BOOKS, "wb");
    if (file != NULL) {
        fwrite(books, sizeof(Book), bookCount, file);
        fclose(file);
    }
    
    printf("\nBook returned successfully!\n");
    printf("Book: %s\n", issuedBooks[issueIndex].bookTitle);
    printf("Borrower: %s\n", issuedBooks[issueIndex].borrowerName);
    printf("Return Date: %s\n", currentDate);
    if (fine > 0) {
        printf("Fine Paid: $%.2f\n", fine);
    }
    
    
    char details[200];
    sprintf(details, "Returned book (Issue ID: %d), Fine: $%.2f", issueId, fine);
    addToHistory("RETURN_BOOK", details);
}

void displayIssuedBooks() {
    int activeCount = 0;
    
    printf("\n========== CURRENTLY ISSUED BOOKS ==========\n");
    
    printf("%-8s %-30s %-20s %-15s %-12s %-12s\n", 
           "Issue ID", "Book Title", "Borrower Name", "Borrower ID", "Issue Date", "Due Date");
    printf("------------------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < issuedCount; i++) {
        if (!issuedBooks[i].isReturned) {
            printf("%-8d %-30s %-20s %-15s %-12s %-12s\n",
                   issuedBooks[i].issueId,
                   issuedBooks[i].bookTitle,
                   issuedBooks[i].borrowerName,
                   issuedBooks[i].borrowerId,
                   issuedBooks[i].issueDate,
                   issuedBooks[i].returnDate);
            activeCount++;
        }
    }
    
    if (activeCount == 0) {
        printf("No books are currently issued.\n");
    } else {
        printf("\nTotal books currently issued: %d\n", activeCount);
    }
}

void displayOverdueBooks() {
    char currentDate[20];
    int overdueCount = 0;
    
    getCurrentDate(currentDate);
    
    printf("\n========== OVERDUE BOOKS ==========\n");
    printf("As of: %s\n\n", currentDate);
    
    printf("%-8s %-30s %-20s %-12s %-12s %-10s\n", 
           "Issue ID", "Book Title", "Borrower Name", "Issue Date", "Due Date", "Days Over");
    printf("-------------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < issuedCount; i++) {
        if (!issuedBooks[i].isReturned) {
            
            if (strcmp(issuedBooks[i].returnDate, currentDate) < 0) {
                printf("%-8d %-30s %-20s %-12s %-12s %-10s\n",
                       issuedBooks[i].issueId,
                       issuedBooks[i].bookTitle,
                       issuedBooks[i].borrowerName,
                       issuedBooks[i].issueDate,
                       issuedBooks[i].returnDate,
                       "7"); 
                overdueCount++;
            }
        }
    }
    
    if (overdueCount == 0) {
        printf("No overdue books.\n");
    } else {
        printf("\nTotal overdue books: %d\n", overdueCount);
    }
}