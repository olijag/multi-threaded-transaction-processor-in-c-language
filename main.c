// Error codes:
// 00 or 0: No errors in both threads - all ok.
// 01: No error in 'innThread', but error opening file in 'utThread'.
// 10: Error opening file 'incoming.txt', no error in 'utThread'.
// 11: Error opening file 'incoming.txt' and 'outgoing.txt'.
// 02: No error in 'innThread', invalid data format when reading in 'utThread'.
// 20: Invalid data format when reading in 'innThread', no error in 'utThread'.
// 21: Invalid data format when reading in 'innThread' and error opening file in 'utThread'.
// 22: Error closing both files.
// 23: Error closing file 'incoming.txt' and invalid data format when reading in 'utThread'.
// 03: No error in 'innThread', error closing file in 'utThread'.
// 30: Error closing file 'incoming.txt', no error in 'utThread'.
// 31: Error closing file 'incoming.txt' and error opening file in 'utThread'.
// 32: Error closing file 'incoming.txt' and error closing file in 'utThread'.
// 33: Invalid data format when reading in both threads.
// Other: Unknown error.
// Note: The code is structured so that the first digit represents the result from 'innThread' and the second from 'utThread'.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

int balance = 0; // Global variable holding the balance.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex to synchronize access to 'balance'.

// Checks if a string contains only numbers. This does not allow negative signs.
int isValidNumber(const char *string) {
    int i = 0;
    while (string[i] != '\0') {
        if (!isdigit(string[i]) && string[i] != '\n') {  // Ignore special characters
            return 0;
        }
        i++;
    }
    return 1;
}

// Reads amounts from the file "incoming.txt" and increases 'balance'.
void *readIn() { 
    FILE *inn_file_pointer = fopen("incoming.txt", "r");
    int *return_code = malloc(sizeof(int));

    if (inn_file_pointer == NULL) {
        *return_code = 1;
        return return_code;
    }
    
    char amountIn[256];
    int line = 0;

    while (fgets(amountIn, 255, inn_file_pointer) != NULL) { // Uses fgets to avoid overbuffering
        line++;
        if (!isValidNumber(amountIn)) {
            printf("Error on line %d in file incoming.txt: '%s' is not a valid integer.\n", line, amountIn);
            fclose(inn_file_pointer);
            *return_code = 3;
            return return_code;
        }

        pthread_mutex_lock(&mutex);
        balance += atoi(amountIn);
        pthread_mutex_unlock(&mutex);
    }
    
    if (fclose(inn_file_pointer) != 0) {
        *return_code = 2;
        return return_code;
    }

    *return_code = 0; // No errors
    return return_code;
}

// Reads amounts from the file "outgoing.txt" and decreases 'balance'.
void *readOut() {
    FILE *ut_file_pointer = fopen("outgoing.txt", "r");
    int *return_code = malloc(sizeof(int));

    if (ut_file_pointer == NULL) {
        *return_code = 1;
        return return_code;
    }
    
    char amountOut[256];
    int line = 0;

    while (fgets(amountOut, 255, ut_file_pointer) != NULL) { // Uses fgets to avoid overbuffering
        line++;
        if (!isValidNumber(amountOut)) {
            printf("Error on line %d in file outgoing.txt: %s is not a valid integer.\n", line, amountOut);
            fclose(ut_file_pointer);
            *return_code = 3;
            return return_code;
        }

        pthread_mutex_lock(&mutex);
        balance -= atoi(amountOut);
        pthread_mutex_unlock(&mutex);
    }
    
    if (fclose(ut_file_pointer) != 0) {
        *return_code = 2;
        return return_code;
    }

    *return_code = 0; // No errors
    return return_code;
}

// Prints error messages based on the error code and file name.
void printErrorMessage(long errorCode, char fileName[]) {
    switch(errorCode) {
        case 1:
            printf("Error: Could not open file %s.\n", fileName);
            break;
        case 2:
            printf("Error: Error closing file %s.\n", fileName);
            break;
        case 3:
            printf("Error: Invalid data format in file %s.\n", fileName);
            break;
        default:
            printf("Error: Unknown error occurred in file %s.\n", fileName);
            break;
    }
}

// Main function that creates threads to read in and out amounts, and handles errors.
int main(){
    pthread_t innThread, utThread;

    // Creates threads for reading in and out amounts.
    if (pthread_create(&innThread, NULL, &readIn, NULL) != 0) {
        printf("Error creating 'innThread'.\n");
        return 1;
    }
    
    if (pthread_create(&utThread, NULL, &readOut, NULL) != 0) {
        printf("Error creating 'utThread'.\n");
        return 2;
    }

    // Waits for the threads to finish and checks for any errors.
    void *innStatus;
    void *utStatus;
    pthread_join(innThread, &innStatus);
    pthread_join(utThread, &utStatus);

    int errorCodeINN = innStatus ? *(int*)innStatus : 0;
    int errorCodeUT = utStatus ? *(int*)utStatus : 0;

    // Free memory after use
    free(innStatus);
    free(utStatus);

    // Check if one or more errors occurred, and print the message
    if (errorCodeINN != 0 || errorCodeUT != 0) {
        printf("One or more errors occurred during processing. The balance may be incorrect.\n");
        if (errorCodeINN != 0) printErrorMessage(errorCodeINN, "incoming.txt");
        if (errorCodeUT != 0) printErrorMessage(errorCodeUT, "outgoing.txt");
    } else {
        printf("Balance: $ %d\n", balance);
    }

    // Print error code
    // A meaningful error code for debugging
    int returnCode = (errorCodeINN * 10) + errorCodeUT;
    return returnCode;
}
