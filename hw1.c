// ------------------------
//  Elif Nazlı Böke
//  CMPE 382 - Sec02
//  HW1
// ------------------------

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int nrDigits (int num) {            // This method finds the 
    int digits = 0;                 // digits of the inputted 
    if(num == 0) return 1;          // number by dividing the               
    while (num != 0) {              // number by 10 until it 
        num = num/10;               // reaches 0 while incrementing 
        digits++;                   // the digits.
    }
    return digits;
}


int isPrime (int num) {                 // Prime numbers start from 2, and goes
    if(num <= 1) return 0;              // like 3, 5, 7, etc. This method finds
    for(int i = 2; i < num; i++)        // if the inputted number is prime or not
        if(num % i == 0)                // by dividing the number by the factors.
            return 0;                   // If there is no remainder after division,
    return 1;                           // it means it is not prime so it returns 0.
}

int main (int argc, char *argv[]) {

    pid_t pid2;
    pid_t pid3;

    int pfd1[2];    // parent --> first child (two ends of the pipe)
    int pfd2[2];    // parent --> second child (two ends of the pipe)
    int pfd3[2];    // first child --> parent (two ends of the pipe)
    int pfd4[2];    // second child --> parent (two ends of the pipe)

    pipe(pfd1);     // creating pipe for parent - first child
    pipe(pfd2);     // creating pipe for parent - second child
    pipe(pfd3);     // creating pipe for first child - parent
    pipe(pfd4);     // creating pipe for second child - parent

    char *filename = argv[1];           // We get the filename as the second argument in the command line.
    FILE* ptr = fopen(filename, "r");   // File is opened for reading using a file pointer.
    
    if (ptr == NULL) {                      // prints error message
        printf("Error in opening file");    // if file cannot be
        exit(-1);                           // opened for reading
    }
    
    


    pid2 = fork();  // Creating the first child process.

    if (pid2 < 0) {                 // gives error if 
        perror("fork failed");      // fork creation
        return -1;                  // has failed.
    }

    else if (pid2 == 0) {   // first child process
        // closing the unnecessary pipe ends.
        close(pfd1[1]);
        close(pfd2[0]);
        close(pfd2[1]);
        close(pfd3[0]);
        close(pfd4[1]);
        close(pfd4[0]);
        int num1;               // it will take the numbers one by one from the read end of the pipe.
        int arrayD[5] = {0};    // creating array for keeping the count of digits of numbers.

        while (read(pfd1[0], &num1, sizeof(int)) > 0) {     // in read() method, num1 variable gets the numbers that is
            int digit = nrDigits(num1);                     // written to pipe by parent one by one. Then, nrDigits()
            arrayD[digit-1]++;                              // method calculates its digit and sends it to an array that
        }                                                   // holds the numbers of corresponding digits.
        
        write(pfd3[1], arrayD, sizeof(arrayD));     // in write() method, arrayD elements are written to the write end of
                                                    // the pipe so that parent can read the elements from the pipe later.
        
        // closing the unnecessary pipe ends.
        close(pfd1[0]);
        close(pfd3[1]);

        exit(0);    // finish child process.
    }





    pid3 = fork();  // Creating the second child process.

    if (pid3 < 0) {         // gives error if 
        perror("child");    // fork creation
        return -1;           // has failed.
    }
    
    else if (pid3 == 0) {   // second child process
        // closing the unnecessary pipe ends.
        close(pfd1[0]);
        close(pfd1[1]);
        close(pfd2[1]);
        close(pfd3[1]);
        close(pfd3[0]);
        close(pfd4[0]);

        int num2;                   // it will take the numbers one by one from the read end of the pipe.
        int prime_arr[2] = {0};     // creating array for keeping the number of prime and non prime numbers.
        
        while (read(pfd2[0], &num2, sizeof(int)) > 0) {                 // in read() method, num2 variable gets the numbers that is
            if(isPrime(num2) == 0) prime_arr[isPrime(num2)]++;          // written to pipe by parent one by one. Then, isPrime()
            else if(isPrime(num2) == 1) prime_arr[isPrime(num2)]++;     // method gives if a number is prime or non prime, then the
        }                                                               // value in prime_arr is incremented one by one accordingly.
        
        write(pfd4[1], prime_arr, sizeof(prime_arr));       // in write() method, prime_arr elements are written to the write end of
                                                            // the pipe so that parent can read the elements from the pipe later.
        
        // closing the unnecessary pipe ends.
        close(pfd2[0]);
        close(pfd4[1]);

        exit(0);    // finish child process.
    }




    /* -------------------- parent process works here -------------------- */
    
    // closing the unnecessary pipe ends.
    close(pfd1[0]);
    close(pfd2[0]);
    close(pfd3[1]);
    close(pfd4[1]);

    int numP;   // it will store the value that is read from file.

    while (fscanf(ptr, "%d", &numP) > 0) {      // fscanf() method reads the integer values one by one from the
        write(pfd1[1], &numP, sizeof(int));     // stream using the file pointer and numP variable stores these
        write(pfd2[1], &numP, sizeof(int));     // values. write() method writes these values to the write ends
    }                                           // of both pipes so that both child processes can read from the pipe.
    
    fclose(ptr);    // close the stream

    // closing the unnecessary pipe ends.
    close(pfd1[1]);
    close(pfd2[1]);

    wait(NULL);     // waits for child 
    wait(NULL);     // processes to finish.

    int digits_number[5] = {0};     // creating array that will store the result of child 1
    int prime_number[2] = {0};      // creating array that will store the result of child 2
    
    read(pfd3[0], digits_number, sizeof(digits_number));    // Read result from child 1 which wrote the results to the pipe.
    read(pfd4[0], prime_number, sizeof(prime_number));      // Read result from child 2 which wrote the results to the pipe.

    printf("Input file: %s\n\n", filename);
    printf("1 digits - %d\n2 digits - %d\n3 digits - %d\n4 digits - %d\n5 digits - %d\n", digits_number[0], digits_number[1], digits_number[2], digits_number[3], digits_number[4]);
    printf("Primes - %d\nNonprimes - %d", prime_number[1], prime_number[0]);

    // closing the unnecessary pipe ends.
    close(pfd3[0]);
    close(pfd4[1]);
  
    return 0;
}
