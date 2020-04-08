#include <stdio.h>

#include <libddcd.h>


int main() {
    /*int retval;
    int response_len = 0;
    char* response = (char*)malloc(sizeof(char));
    char* request = "hello,, world";
    retval = ddccSendMessageToRegion(
            "spain",
            request,
            (int)strlen(request),
            &response,
            &response_len
    );*/

    float **a, **b, **result;
    int x=5, y=5;

    a = (float**)malloc(sizeof(float*)*x);
    b = (float**)malloc(sizeof(float*)*x);
    result = (float**)malloc(sizeof(float*)*x);
    for (int i = 0; i < x; ++i) {
        a[i] = (float*)malloc(sizeof(float)*y);
        b[i] = (float*)malloc(sizeof(float)*y);
        result[i] = (float*)malloc(sizeof(float)*y);
        for (int j = 0; j < y; ++j) {
            a[i][j] = (float)i+j;
            b[i][j] = (float)i-j;
        }
    }


    //ddccMatrixMultiplication(a, x, y, b, x, y, &result);
    ddccVectorAddition(a[1],5,b[1],5,&(result[1]));
    ddccMatrixMultiplication(a, x, y, b, x, y, &result);
    ddccMatrixMultiplication(a, x, y, b, x, y, &result);
    ddccVectorAddition(a[1],5,b[1],5,&(result[1]));
    ddccVectorAddition(a[1],5,b[1],5,&(result[1]));
    ddccVectorAddition(a[1],5,b[1],5,&(result[1]));
    ddccVectorAddition(a[1],5,b[1],5,&(result[1]));
    return 0;
}