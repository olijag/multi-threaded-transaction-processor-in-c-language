#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

int balance = 0; // Global variable that holds the balance.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex to synchronize access to 'balance'.

// Checks if a string contains only digits. This does not allow negative signs.
int isValidNumber(const char *string) {
    for (int i = 0; string[i] != '\0'; i++) {
        if (!isdigit(string[i]))
            return 0;
    }
    return 1;
}

// Reads amount from the file "incoming.txt" and increases 'balance'.
void *readIn() { 
    FILE *in_file_pointer = fopen("incoming.txt", "r");
    if (in_file_pointer == NULL) {
        return (void*)1;
    }
    
    char amountIn[256];
    int line = 0;

    while (fscanf(in_file_pointer, "%s", amountIn) == 1) {
        line++;
        if (!isValidNumber(amountIn)) {
            printf("Error on line %d in file incoming.txt: '%s' is not a valid integer.\n", line, amountIn);
            fclose(in_file_pointer);
            return (void*)3;
        }

        pthread_mutex_lock(&mutex);
        balance += atoi(amountIn);
        pthread_mutex_unlock(&mutex);
    }
    
    if (fclose(in_file_pointer) != 0) {
        return (void*)2;
    }

    return NULL;
}

// Reads amount from the file "outgoing.txt" and decreases 'balance'.
void *readOut() {
    FILE *out_file_pointer = fopen("outgoing.txt", "r");
    if (out_file_pointer == NULL) {
        return (void*)1;
    }
    
    char amountOut[256];
    int line = 0;

    while (fscanf(out_file_pointer, "%s", amountOut) == 1) {
        line++;
        if (!isValidNumber(amountOut)) {
            printf("Error on line %d in file outgoing.txt: '%s' is not a valid integer.\n", line, amountOut);
            fclose(out_file_pointer);
            return (void*)3;
        }

        pthread_mutex_lock(&mutex);
        balance -= atoi(amountOut);
        pthread_mutex_unlock(&mutex);
    }
    
    if (fclose(out_file_pointer) != 0) {
        return (void*)2;
    }

    return NULL;
}

// Prints out error messages based on the error code and file name.
void printErrorMessage(long errorCode, char fileName[]) {
    switch(errorCode) {
        case 1:
            printf("Error: The file '%s' could not be opened.\n", fileName);
            break;
        case 2:
            printf("Error: Error closing the file '%s'.\n", fileName);
            break;
        case 3:
            printf("Error: Invalid data format in the file '%s'.\n", fileName);
            break;
        default:
            printf("Error: Unknown error occurred in the file '%s'.\n", fileName);
            break;
    }
}

// Main function that creates threads to read in and out amounts, and handles errors.
int main(){
    pthread_t inThread, outThread;

    // Creates threads for reading in and out amounts.
    if (pthread_create(&inThread, NULL, &readIn, NULL) != 0) {
        printf("Error creating in-thread.\n");
        return 1;
    }
    
    if (pthread_create(&outThread, NULL, &readOut, NULL) != 0) {
        printf("Error creating out-thread.\n");
        return 2;
    }

    // Waits for the threads to finish and checks for any errors.
    void *inStatus;
    void *outStatus;
    pthread_join(inThread, &inStatus);
    pthread_join(outThread, &outStatus);

    // Handles errors if any, and prints out the final balance if no errors.
    if (inStatus != NULL || outStatus != NULL) {
        if (inStatus != NULL) printErrorMessage((long)inStatus, "incoming.txt");
        if (outStatus != NULL) printErrorMessage((long)outStatus, "outgoing.txt");
        printf("One or more errors occurred during processing. The balance may be incorrect.\n");
    } else {
        printf("Balance: %d USD\n", balance);
    }

    return 0;
}
