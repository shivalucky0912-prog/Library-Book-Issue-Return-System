
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_BOOKS 100
#define MAX_QUEUE 100
#define MAX_RECORDS 500
#define TITLE_LEN 100
#define NAME_LEN 50

typedef struct {
    int id;
    char title[TITLE_LEN];
    int total, available;
} Book;

typedef struct {
    int studentId;
    char studentName[NAME_LEN];
    int bookId;
} Request;

typedef struct {
    int studentId;
    char studentName[NAME_LEN];
    int bookId;
    int active;
} BorrowRecord;

static Book books[MAX_BOOKS];
static int bookCount;

static Request queue[MAX_QUEUE];
static int front, rear, queueCount;

static BorrowRecord records[MAX_RECORDS];
static int recordCount;

static int readLine(const char *prompt, char *out, size_t size) {
    size_t n;
    int ch;

    printf("%s", prompt);

    if (!fgets(out, (int)size, stdin))
        return 0;

    n = strlen(out);

    if (n && out[n - 1] == '\n')
        out[n - 1] = '\0';
    else
        while ((ch = getchar()) != '\n' && ch != EOF) { }

    while (out[0] && isspace((unsigned char)out[0]))
        memmove(out, out + 1, strlen(out));

    n = strlen(out);

    while (n && isspace((unsigned char)out[n - 1]))
        out[--n] = '\0';

    return 1;
}

static int readInt(const char *prompt, int *value) {
    char line[128], *end;
    long n;

    for (;;) {
        if (!readLine(prompt, line, sizeof line))
            return 0;

        if (!line[0]) {
            puts("Please enter a number.");
            continue;
        }

        n = strtol(line, &end, 10);

        while (isspace((unsigned char)*end))
            end++;

        if (*end || end == line ||
            n < -2147483647L - 1L || n > 2147483647L) {
            puts("Invalid number. Try again.");
            continue;
        }

        *value = (int)n;
        return 1;
    }
}

static int findBook(int id) {
    int i;

    for (i = 0; i < bookCount; i++)
        if (books[i].id == id)
            return i;

    return -1;
}

static int hasActiveBorrow(int studentId, int bookId) {
    int i;

    for (i = 0; i < recordCount; i++)
        if (records[i].active &&
            records[i].studentId == studentId &&
            records[i].bookId == bookId)
            return 1;

    return 0;
}

static void addBook(void) {
    int id, copies;
    char title[TITLE_LEN];

    if (bookCount >= MAX_BOOKS) {
        puts("Book storage is full.");
        return;
    }

    if (!readInt("Enter book ID: ", &id))
        return;

    if (id <= 0 || findBook(id) != -1) {
        puts("Book ID must be positive and unique.");
        return;
    }

    if (!readLine("Enter book title: ", title, sizeof title))
        return;

    if (!title[0]) {
        puts("Title cannot be empty.");
        return;
    }

    if (!readInt("Enter total copies: ", &copies))
        return;

    if (copies <= 0) {
        puts("Copies must be greater than zero.");
        return;
    }

    books[bookCount].id = id;
    strcpy(books[bookCount].title, title);
    books[bookCount].total = copies;
    books[bookCount].available = copies;
    bookCount++;

    puts("Book added successfully!");
}

static void displayBooks(void) {
    int i;

    puts("\n--- Library Book Inventory ---");

    if (!bookCount) {
        puts("No books have been added.");
        return;
    }

    printf("%-10s %-30s %-10s %-10s\n",
           "Book ID", "Title", "Total", "Available");

    for (i = 0; i < bookCount; i++)
        printf("%-10d %-30s %-10d %-10d\n",
               books[i].id, books[i].title,
               books[i].total, books[i].available);
}

static void searchBook(void) {
    int id, i;

    if (!readInt("Enter book ID to search: ", &id))
        return;

    i = findBook(id);

    if (i < 0) {
        puts("Book not found.");
        return;
    }

    printf("Book ID: %d\n", books[i].id);
    printf("Title: %s\n", books[i].title);
    printf("Total copies: %d\n", books[i].total);
    printf("Available copies: %d\n", books[i].available);
}

static void addRequest(void) {
    int sid, bid, i;
    char name[NAME_LEN];

    if (queueCount >= MAX_QUEUE) {
        puts("Queue is full. Process requests first.");
        return;
    }

    if (!readInt("Enter student ID: ", &sid))
        return;

    if (sid <= 0) {
        puts("Student ID must be positive.");
        return;
    }

    if (!readLine("Enter student name: ", name, sizeof name))
        return;

    if (!name[0]) {
        puts("Student name cannot be empty.");
        return;
    }

    if (!readInt("Enter book ID: ", &bid))
        return;

    i = findBook(bid);

    if (i < 0) {
        puts("Book not found. Add it first.");
        return;
    }

    if (books[i].available == 0) {
        puts("No copies currently available.");
        return;
    }

    queue[rear].studentId = sid;
    strcpy(queue[rear].studentName, name);
    queue[rear].bookId = bid;

    rear = (rear + 1) % MAX_QUEUE;
    queueCount++;

    puts("Issue request added to the queue successfully.");
}

static void displayQueue(void) {
    int i, p;

    puts("\n--- Pending Issue Requests (FIFO) ---");

    if (!queueCount) {
        puts("No pending requests.");
        return;
    }

    for (i = 0; i < queueCount; i++) {
        p = (front + i) % MAX_QUEUE;

        printf("%d. Student ID: %d | Student: %s | Book ID: %d\n",
               i + 1, queue[p].studentId,
               queue[p].studentName, queue[p].bookId);
    }
}

static void processRequest(void) {
    Request r;
    int i;

    if (!queueCount) {
        puts("No requests to process.");
        return;
    }

    r = queue[front];
    front = (front + 1) % MAX_QUEUE;
    queueCount--;

    i = findBook(r.bookId);

    if (i < 0) {
        puts("Book no longer exists; request skipped.");
        return;
    }

    if (books[i].available <= 0) {
        puts("No copies available; request cannot be completed.");
        return;
    }

    if (recordCount >= MAX_RECORDS) {
        puts("Borrowing record storage is full.");
        return;
    }

    if (hasActiveBorrow(r.studentId, r.bookId)) {
        puts("Student already has this book issued.");
        return;
    }

    records[recordCount].studentId = r.studentId;
    strcpy(records[recordCount].studentName, r.studentName);
    records[recordCount].bookId = r.bookId;
    records[recordCount].active = 1;
    recordCount++;

    books[i].available--;

    printf("Book \"%s\" issued successfully to %s!\n",
           books[i].title, r.studentName);
}

static void returnBook(void) {
    int sid, bid, i, b;

    if (!readInt("Enter student ID: ", &sid))
        return;

    if (!readInt("Enter book ID to return: ", &bid))
        return;

    for (i = 0; i < recordCount; i++) {
        if (records[i].active &&
            records[i].studentId == sid &&
            records[i].bookId == bid) {

            b = findBook(bid);

            if (b < 0) {
                puts("Book record not found; return cancelled.");
                return;
            }

            if (books[b].available >= books[b].total) {
                puts("Inventory count is already at its maximum; check records.");
                return;
            }

            records[i].active = 0;
            books[b].available++;

            printf("Book \"%s\" returned successfully by %s!\n",
                   books[b].title, records[i].studentName);
            return;
        }
    }

    puts("No active issue record found for that student and book.");
}

static void displayBorrowed(void) {
    int i, found = 0;

    puts("\n--- Currently Issued Books ---");

    for (i = 0; i < recordCount; i++) {
        if (records[i].active) {
            printf("Student ID: %d | Student: %s | Book ID: %d\n",
                   records[i].studentId,
                   records[i].studentName,
                   records[i].bookId);
            found = 1;
        }
    }

    if (!found)
        puts("No books are currently issued.");
}

int main(void) {
    int choice;

    puts("Welcome to the Library Book Issue and Return Management System!");
    puts("Note: Data is stored only while this program is running.");

    for (;;) {
        puts("\n============================================");
        puts(" LIBRARY BOOK ISSUE AND RETURN MANAGEMENT");
        puts("============================================");
        puts("1. Add Book");
        puts("2. Display All Books");
        puts("3. Search Book");
        puts("4. Add Book Issue Request");
        puts("5. Process Next Request");
        puts("6. Display Pending Requests");
        puts("7. Return a Book");
        puts("8. Display Currently Issued Books");
        puts("0. Exit");

        if (!readInt("Enter your choice: ", &choice)) {
            puts("Input ended. Exiting.");
            break;
        }

        switch (choice) {
            case 1: addBook(); break;
            case 2: displayBooks(); break;
            case 3: searchBook(); break;
            case 4: addRequest(); break;
            case 5: processRequest(); break;
            case 6: displayQueue(); break;
            case 7: returnBook(); break;
            case 8: displayBorrowed(); break;
            case 0:
                puts("Thank you for using the Library Management System. Goodbye!");
                return 0;
            default:
                puts("Invalid choice. Enter a number from 0 to 8.");
        }
    }

    return 0;
}
