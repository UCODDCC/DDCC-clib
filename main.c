#include <stdio.h>

#include <libddcd.h>


int main() {
    /*int retval;
    int response_len = 0;
    char* response = (char*)malloc(sizeof(char));
    char* request = "hello,, world";
    retval = ddcdSendMessageToRegion(
            "spain",
            request,
            (int)strlen(request),
            &response,
            &response_len
    );*/

    int **a, **b, **result, x=5, y=5;

    a = (int**)malloc(sizeof(int*)*x);
    b = (int**)malloc(sizeof(int*)*x);
    result = (int**)malloc(sizeof(int*)*x);
    for (int i = 0; i < x; ++i) {
        a[i] = (int*)malloc(sizeof(int)*y);
        b[i] = (int*)malloc(sizeof(int)*y);
        result[i] = (int*)malloc(sizeof(int)*y);
        for (int j = 0; j < y; ++j) {
            a[i][j] = i+j;
            b[i][j] = i-j;
        }
    }


    ddcdMatrixMultiplication(a,x,5,b,x,y,&result);
    return 0;
}