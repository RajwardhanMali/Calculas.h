#include "calculas.h"

int main() {
    Term* poly = NULL;
    char input[256];
    printf("Enter a function :");
    fgets(input,sizeof(input),stdin);
    makePOLYNOMIAL(&poly, input);
    printf("Given function: ");
    printPoly(poly);
    Term* derivativePoly = derivative(poly);
    printf("\nDerivative: ");
    printPoly(derivativePoly);
    Term* integralPoly = integrate(poly);
    printf("\nIntegral : ");
    if(integralPoly)
    printPoly(integralPoly);
    else
    printf("N/A");
    return 0;
}