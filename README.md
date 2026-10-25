# Library-Book-Issue-Return-System
A C project using a FIFO queue to manage library book issue requests and returns.
Library Book Issue and Return Management System (C)
A console-based C project for managing book inventory, student issue requests, book returns, and active borrowing records. The project demonstrates a FIFO (First In, First Out) queue.
Features
- Add, display, and search books
- Add student issue requests to a FIFO queue
- Process the next queued request
- Display pending requests
- Return books and update availability
- Display currently issued books
Technologies
- C
- GCC
- Visual Studio Code
Compile and run (Windows PowerShell)
Run these commands from the project folder:
gcc -Wall -Wextra -std=c11 main.c -o library.exe
.\library.exe
If GCC is not on your PATH, use the full path to your GCC executable or fix your PATH configuration.
Queue concept
FIFO means First In, First Out. The first student request added to the queue is the first request processed.
Limitations
The current program stores data in memory. Books and records reset when the program exits; permanent file storage is not implemented.
Example test
1. Add Book ID 101, title C Programming, total copies 3.
2. Add a request for student 1001 (Pranee), Book ID 101.
3. Add a request for student 1002 (Shivvv), Book ID 101.
4. Process the queue twice. Pranee should be processed before Shivvv.
5. Return the book for student 1001.
6. Display currently issued books. Student 1002 should remain listed.
Author:
Group 11
- Name: N Shiva Tej
- Roll number: 20251ECE0115
- Name: Pallavi P S
- Roll number: 20251ECE0117
- Name: Pradeep N
- Roll number: 20251ECE0118
- Name: Kannekanti Durga Jagadeswara Varaprasad
- Roll number: 20251ECE0119
- Name: Shivaraj Sanganabasappa Adagall
- Roll number: 20251ECE0120
- Name: Rajesh H
- Roll number: 20251LCE3004
