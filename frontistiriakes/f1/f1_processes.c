#include <stdio.h>
#include <stdlib.h> // for atoi, exit
#include <unistd.h> // for fork, sleep
#include <sys/wait.h> // for wait
//--------------------------------

int main(int argc, char* argv[]) {
    int n;
    long int sum=0, N, k, i, j, val;
    int fd[2];

    // create pipe
    pipe(fd);
    
    // get number of child processes from command line
    if (argc > 1) {
        n = atoi(argv[1]);
        N = atoi(argv[2]);
    }

    for (i=0; i < n; i++) {
        int pid = fork();

        if (pid == 0) { // child process
            sleep(i + 2); //
            // calc a sum
            sum = 0;
            for (k = ((N/n)*i)+1; k <= (N/n)*(i+1); k++)
                sum += k;

            printf("Computed sum by child %ld process is %ld\n", i, sum);
            
            close(fd[0]); // write, close read
            write(fd[1], &sum, sizeof(sum));
            
            close(fd[1]); // close write

            exit(0);
        }
        read(fd[0], &val, sizeof(val)); // waiting
        printf("Parent receives value %ld from child %ld\n", val, i);
        sum += val;
    }
        close(fd[0]); // close read
    
    // parent process, check total sum
    for(j=N; j>N-(N%n); j--)
        sum += j;

    if(sum==N*(N+1)/2)
        printf("The total sum is %ld\n", sum);

    return 0;
}