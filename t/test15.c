#include <stdio.h>
#include <math.h>

int main(void) {
    double a;
    scanf("%lf", &a);
    
    double r = sqrt(a / M_PI);
    printf("Radius is %lf\n", r);
    
    return 0;
}

