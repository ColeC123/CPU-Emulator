#include "stdio.h"

int main(void) {
    double x;
    printf("Enter a number: ");
    scanf("%lf", &x);

    double start = 0;
    double end = x;
    double middle = x / 2;

    //Basic square root algorithm by checking to see if midpoint is less than or equal to target value when squared
    //Doesn't check for negative inputs, so it will just return 0 if there is a negative number
    for (int i = 0; i < 70; i++) {
        if (middle * middle < x) {
            start = middle;
            middle = (end + start) / 2;
        } else if (middle * middle > x) {
            end = middle;
            middle = (end + start) / 2;
        } else {
            break;
        }
    }

    printf("\nSqrt %.15lf = %.15lf\n", x, middle);
    return 0;
}