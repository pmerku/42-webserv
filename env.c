#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[], char *env[]) {
    setbuf(stdout, NULL);
    // headers
    printf("Content-Type: text/html\r\n");
    printf("Status: 418 Teapot\r\n");
    printf("\r\n");

    // argv
    printf("<h1>Argv</h1><ol>\n");
    for (int i = 0; i < argc; i++) {
        printf("<li>%s</li><br>\n", argv[i]);
    }

    // env
    printf("</ol><hr><h1>environment</h1><ul>\n");
    for (size_t i = 0; env[i] != NULL; i++) {
        printf("<li>%s</li>\n", env[i]);
    }

    // stdin
    printf("</ul><hr><h1>Stdin</h1>\n");
    printf("<pre>\n");
    size_t  stdinDataSize = 0;
    while (1) {
        char    data[BUFFER_SIZE];

        // read new data
        ssize_t ret = read(STDIN_FILENO, data, BUFFER_SIZE);
        if (ret == -1) {
            // failed to read
            write(STDOUT_FILENO, " - Failed to read - ", 20);
            break;
        }
        else if (ret == 0) {
            // stop when EOF
            break;
        }
        write(STDOUT_FILENO, data, ret);
        stdinDataSize += ret;
    }
    if (stdinDataSize == 0)
        write(STDOUT_FILENO, " - No data in STDIN - ", 22);
    printf("</pre>\n");
    return (0);
}
