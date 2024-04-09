# Financial Transaction Tracker

## Overview
The Financial Transaction Tracker is a multi-threaded application written in C that reads financial transaction amounts from files, updating a global balance in a thread-safe manner. It ensures data consistency using mutexes to prevent race conditions when multiple threads attempt to modify the balance simultaneously.

## How It Works
The program creates two threads:
- One for processing incoming transactions (`incoming.txt`), which increases the balance.
- Another for processing outgoing transactions (`outgoing.txt`), which decreases the balance.

Each thread reads its respective file line by line, validates the format of the transaction amount, and updates the global balance accordingly. The program ensures that access to the balance is synchronized using a mutex, preventing race conditions.

## Requirements
- A C compiler (e.g., gcc)
- POSIX Threads Library (pthread)