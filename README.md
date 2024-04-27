# Financial Transaction Tracker

## Overview
The Financial Transaction Tracker is a multi-threaded application developed in C. It reads financial transaction amounts from designated files and updates a global balance in a thread-safe manner. The program utilizes mutexes to ensure data consistency and prevent race conditions when multiple threads access and modify the balance simultaneously.

## How It Works
The application initiates two separate threads:
- **Incoming Transactions Thread:** This thread handles the 'incoming.txt' file. It reads transaction amounts line by line, validates the format, and increases the global balance.
- **Outgoing Transactions Thread:** This thread processes the 'outgoing.txt' file. It performs similar operations as the incoming transactions thread but decreases the balance based on the transaction amounts read.

Both threads ensure that the access to the global balance is synchronized using a mutex. This synchronization is crucial for preventing race conditions and ensuring that the balance updates are performed safely and correctly.

### Error Handling
The application robustly handles various file-related errors and data validation issues:
- **File Access Errors:** If a thread cannot open or close its respective file, it reports this error.
- **Data Format Errors:** If the data format in the files does not comply with the expected integer format, the error is logged.
- **Thread Execution Errors:** The program also checks for errors in thread creation and execution, ensuring that any such issues are addressed immediately.

Each error is associated with a specific code, making debugging and error tracking straightforward:
- `00` or `0`: No errors - operations in both threads completed successfully.
- `01`: No error in the incoming transactions but an error occurred while opening the outgoing transactions file.
- `10`: Error opening the incoming transactions file; no error in the outgoing transactions.
- Additional codes detail other combinations of file opening, closing, and data format errors.

## Requirements
To run the Financial Transaction Tracker, you need:
- A C compiler like gcc or clang.
- POSIX Threads Library (pthread) for thread management.

## Usage
Compile the program using a C compiler with pthread support. For example, using gcc:
```bash
gcc -o transaction_tracker main.c -lpthread
