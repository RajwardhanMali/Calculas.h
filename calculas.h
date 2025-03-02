#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Term {
    double coefficient;
    char typechar[40];
    int exponent;
    struct Term* next;
    struct Term* internal;
    struct Term* factor1; 
    struct Term* factor2;
} Term;

Term* createTerm(double coefficient, int exponent, const char* type) {
    Term* term = (Term*)malloc(sizeof(Term));
    term->coefficient = coefficient;
    term->exponent = exponent;
    strncpy(term->typechar, type, sizeof(term->typechar) - 1);
    term->typechar[sizeof(term->typechar) - 1] = '\0';
    term->next = NULL;
    term->internal = NULL;
    term->factor1 = NULL;
    term->factor2 = NULL;
    return term;
}

void addTerm(Term** poly, Term* term) {
    if (*poly == NULL) {
        *poly = term;
    } else {
        Term* current = *poly;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = term;
    }
}

void replaceSubstring(char *str, const char *oldSubstring, const char *newSubstring) {
    char buffer[1024]; 
    char *pos;
    pos = strstr(str, oldSubstring);
    if (pos == NULL) {
        strcpy(buffer, str);
    } else {
        strncpy(buffer, str, pos - str);
        buffer[pos - str] = '\0';
        strcat(buffer, newSubstring);
        strcat(buffer, pos + strlen(oldSubstring));
    }
    strcpy(str, buffer);
}

//new
void prependString(char *dest, const char *prefix) {
    char buffer[1024]; 
    strcpy(buffer, prefix);
    strcat(buffer, dest);
    strcpy(dest, buffer);
}

Term* parseTerm(char* termStr) {

    double coefficient = 1.0;
    int exponent = 0;
    char typechar[40] = "POLYNOMIAL";
    if (strstr(termStr, "*") != NULL) {
        strcpy(typechar, termStr);
        char* factor1Str = strtok(termStr, "*");
        char* factor2Str = strtok(NULL, "*");
        Term* term = createTerm(coefficient, exponent, typechar);
        term->factor1 = parseTerm(factor1Str);
        term->factor2 = parseTerm(factor2Str); 
        return term;
    }
    if (strstr(termStr, "sin") != NULL) {
        sscanf(termStr, "%lfsin", &coefficient);
        strcpy(typechar, "sin");
    } else if (strstr(termStr, "cos") != NULL) {
        sscanf(termStr, "%lfcos", &coefficient);
        strcpy(typechar, "cos");
    } else if (strstr(termStr, "tan") != NULL) {
        sscanf(termStr, "%lftan", &coefficient);
        strcpy(typechar, "tan");
    } else if (strstr(termStr, "sec") != NULL) {
        sscanf(termStr, "%lfsec", &coefficient);
        strcpy(typechar, "sec");
    } else if (strstr(termStr, "csc") != NULL) {
        sscanf(termStr, "%lfcsc", &coefficient);
        strcpy(typechar, "csc");
    } else if (strstr(termStr, "cot") != NULL) {
        sscanf(termStr, "%lfcot", &coefficient);
        strcpy(typechar, "cot");
    } else if (strstr(termStr, "log") != NULL) {
        sscanf(termStr, "%lflog", &coefficient);
        strcpy(typechar, "log");
    } else if (strstr(termStr, "e^") != NULL) {
        sscanf(termStr, "%lfe^", &coefficient);
        exponent = 1;
        strcpy(typechar, "e^");
    } else {
        char* xPos = strchr(termStr, 'x');
        if (xPos != NULL) {
            if (xPos == termStr) {
                coefficient = 1.0;
            } else if (xPos == termStr + 1 && *termStr == '-') {
                coefficient = -1.0;
            } else {
                sscanf(termStr, "%lf", &coefficient);
            }
            char* expPos = strchr(xPos, '^');
            if (expPos != NULL) {
                sscanf(expPos + 1, "%d", &exponent);
            } else {
                exponent = 1;
            }
            strcpy(typechar, "POLYNOMIAL");
        } 
        else {
            sscanf(termStr, "%lf", &coefficient);
            exponent = 0;
            strcpy(typechar, "CONSTANT");
        }
    }

    Term* term = createTerm(coefficient, exponent, typechar);

    char* openParen = strchr(termStr, '(');
    if (openParen != NULL) {
        char* closeParen = strchr(openParen, ')');
        if (closeParen != NULL) {
            char* compositeTermStr = (char*)malloc(closeParen - openParen);
            strncpy(compositeTermStr, openParen + 1, closeParen - openParen - 1);
            compositeTermStr[closeParen - openParen - 1] = '\0';
            if (strcmp(compositeTermStr, "x") != 0) {
                term->internal = parseTerm(compositeTermStr);
            }
            free(compositeTermStr);
        }
    }

    return term;
}

void makePOLYNOMIAL(Term** poly, char* input) {
    char* termStart = input;
    char* plusPos = strchr(termStart, '+');
    while (plusPos != NULL) {
        *plusPos = '\0'; 
        Term* term = parseTerm(termStart);
        addTerm(poly, term);
        
        termStart = plusPos + 1; 
        plusPos = strchr(termStart, '+'); 
    }
    
    if (*termStart != '\0') {
        Term* term = parseTerm(termStart);
        addTerm(poly, term);
    }
}

char* getTerm(Term* current) {
    if (current == NULL) {
        return NULL;
    }

    size_t termStrSize = 100; 
    char* termStr = (char*)malloc(termStrSize);
    if (termStr == NULL) {
        return NULL; 
    }

    if (current->internal != NULL) {
        sprintf(termStr, "%.2lf%s(", current->coefficient, current->typechar);
        char* internalStr = getTerm(current->internal);
        if (internalStr == NULL) {
            free(termStr);
            return NULL; 
        }
        size_t internalStrSize = strlen(internalStr);
        if (internalStrSize + strlen(termStr) + 2 > termStrSize) {
            termStrSize = internalStrSize + strlen(termStr) + 2;
            termStr = (char*)realloc(termStr, termStrSize);
            if (termStr == NULL) {
                free(internalStr);
                return NULL; 
            }
        }
        strcat(termStr, internalStr);
        strcat(termStr, ")");
        free(internalStr);
    } else {
        if (strcmp(current->typechar, "POLYNOMIAL") == 0) {
            sprintf(termStr, "%.2lfx^%d", current->coefficient, current->exponent);
        } else if (strcmp(current->typechar, "sin") == 0) {
            sprintf(termStr, "%.2lfsin(x)", current->coefficient);
        } else if (strcmp(current->typechar, "cos") == 0) {
            sprintf(termStr, "%.2lfcos(x)", current->coefficient);
        } else if (strcmp(current->typechar, "tan") == 0) {
            sprintf(termStr, "%.2lftan(x)", current->coefficient);
        } else if (strcmp(current->typechar, "sec") == 0) {
            sprintf(termStr, "%.2lfsec(x)", current->coefficient);
        } else if (strcmp(current->typechar, "csc") == 0) {
            sprintf(termStr, "%.2lfcsc(x)", current->coefficient);
        } else if (strcmp(current->typechar, "cot") == 0) {
            sprintf(termStr, "%.2lfcot(x)", current->coefficient);
        } else if (strcmp(current->typechar, "log") == 0) {
            sprintf(termStr, "%.2lflog(x)", current->coefficient);
        } else if (strcmp(current->typechar, "e^") == 0) {
            sprintf(termStr, "%.2lfe^x", current->coefficient);
        } else {
            sprintf(termStr, "%s", current->typechar);
        }
    }
    return termStr;
}   

void printPoly(Term* poly) {
    Term* current = poly;

    while (current != NULL) {
        if (current->internal != NULL) {
            printf(" %.2lf%s(", current->coefficient, current->typechar);
            printPoly(current->internal);
            printf(")");
        } else {
            if (strcmp(current->typechar, "POLYNOMIAL") == 0) {
                printf(" %.2lfx^%d ", current->coefficient, current->exponent);
            } 
            else if (strcmp(current->typechar, "sin") == 0) {
                printf(" %.2lfsin(x) ", current->coefficient);
            } 
            else if (strcmp(current->typechar, "cos") == 0) {
                printf(" %.2lfcos(x) ", current->coefficient);
            } 
            else if (strcmp(current->typechar, "tan") == 0) {
                printf(" %.2lftan(x) ", current->coefficient);
            } 
            else if (strcmp(current->typechar, "sec") == 0) {
                printf(" %.2lfsec(x) ", current->coefficient);
            } 
            else if (strcmp(current->typechar, "csc") == 0) {
                printf(" %.2lfcsc(x) ", current->coefficient);
            } 
            else if (strcmp(current->typechar, "cot") == 0) {
                printf(" %.2lfcot(x) ", current->coefficient);
            } 
            else if (strcmp(current->typechar, "log") == 0) {
                printf(" %.2lflog(x) ", current->coefficient);
            } 
            else if (strcmp(current->typechar, "e^") == 0) {
                printf(" %.2lfe^x ", current->coefficient);
            }
            else{
                printf("%s",current->typechar);
            }
        }
        current = current->next;
        if (current != NULL) {
            printf(" + ");
        }
    }
}

Term* integrate(Term* poly) {
    Term* integralPoly = NULL;
    Term* current = poly;

    while (current != NULL) {
        if (strcmp(current->typechar, "POLYNOMIAL") == 0) {
            if (current->exponent != -1) { 
                double newCoefficient = current->coefficient / (current->exponent + 1.0);
                int newExponent = current->exponent + 1;
                char termStr[40];
                sprintf(termStr, "%.2lfx^%d", newCoefficient, newExponent);
                addTerm(&integralPoly, createTerm(newCoefficient, newExponent, termStr));
            } else {  
                char termStr[40];
                sprintf(termStr, "%.2lflog(x)", current->coefficient);
                addTerm(&integralPoly, createTerm(current->coefficient, 1, termStr));
            }
        } else if (strcmp(current->typechar, "sin") == 0) {
            char termStr[40];
            sprintf(termStr, "%.2lfcos(x)", -current->coefficient);
            addTerm(&integralPoly, createTerm(-current->coefficient, 1, termStr));
        } else if (strcmp(current->typechar, "cos") == 0) {
            char termStr[40];
            sprintf(termStr, "%.2lfsin(x)", current->coefficient);
            addTerm(&integralPoly, createTerm(current->coefficient, 1, termStr));
        } else if (strcmp(current->typechar, "sec^2") == 0) {
            char termStr[40];
            sprintf(termStr, "%.2lftan(x)", current->coefficient);
            addTerm(&integralPoly, createTerm(current->coefficient, 1, termStr));
        } else if (strcmp(current->typechar, "e^") == 0) {
            char termStr[40];
            sprintf(termStr, "%.2lfe^x", current->coefficient);
            addTerm(&integralPoly, createTerm(current->coefficient, 1, termStr));
        } else if (strcmp(current->typechar, "log") == 0) {
            char termStr[40];
            sprintf(termStr, "%.2lf(x*log(x) - x)", current->coefficient);
            addTerm(&integralPoly, createTerm(current->coefficient, 1, termStr));
        }
        else{
            return NULL;
        }
        
        current = current->next;
    }
    
    addTerm(&integralPoly, createTerm(1.0, 0, "C"));
    return integralPoly;
}

void simplifyPolynomial(Term* poly) {
    Term *current, *prev, *temp;

    for (current = poly; current != NULL; current = current->next) {
        prev = current;
        for (temp = current->next; temp != NULL; temp = temp->next) {
            if (strcmp(current->typechar, temp->typechar) == 0 && current->exponent == temp->exponent) {
                if (current->coefficient * temp->coefficient >= 0) {
                    current->coefficient += temp->coefficient;  
                } else {
                    current->coefficient -= temp->coefficient;  
                }
                prev->next = temp->next;
                free(temp);
                temp = prev;
            } else {
                prev = temp;
            }
        }
    }

    for (current = poly; current != NULL; current = current->next) {
        for (temp = current->next; temp != NULL; temp = temp->next) {
            if (strcmp(current->typechar, temp->typechar) == 0) {
                if (current->coefficient != 0 && temp->coefficient != 0) {
                    if (strcmp(current->typechar, "POLYNOMIAL") == 0) {
                        current->coefficient *= temp->coefficient;
                        current->exponent += temp->exponent;  
                    } 
                    else if (strcmp(current->typechar, "sin") == 0 || strcmp(current->typechar, "cos") == 0 ||
                             strcmp(current->typechar, "tan") == 0 || strcmp(current->typechar, "sec") == 0 || 
                             strcmp(current->typechar, "csc") == 0 || strcmp(current->typechar, "cot") == 0) {
                        current->coefficient *= temp->coefficient;  
                    } 
                    else if (strcmp(current->typechar, "log") == 0) {
                        current->coefficient *= temp->coefficient;  
                    } 
                    else if (strcmp(current->typechar, "e^") == 0) {
                        current->coefficient *= temp->coefficient;  
                    }
                }
                prev->next = temp->next;
                free(temp);
                temp = prev;
            } else {
                prev = temp;
            }
        }
    }
}

char* deriveTerm(Term* current) {
    char* str1 = (char*)malloc(sizeof(char) * 40);
        if (strcmp(current->typechar, "POLYNOMIAL") == 0 && current->exponent > 0) {
            double newCoefficient = current->coefficient * current->exponent;
            int newExponent = current->exponent - 1;
            sprintf(str1, "%.2lfx^%d", newCoefficient, newExponent);
        } else if (strcmp(current->typechar, "sin") == 0) {// Derivative of sin(x) = cos(x)
            sprintf(str1, "%.2lfcos(x)", current->coefficient);
        } else if (strcmp(current->typechar, "cos") == 0) {// Derivative of cos(x) = -sin(x)
            sprintf(str1, " %.2lf sin(x)", -current->coefficient);
        } else if (strcmp(current->typechar, "tan") == 0) {// Derivative of tan(x) = sec^2(x)
            sprintf(str1, "%.2lfsec^2(x)", current->coefficient);
        } else if (strcmp(current->typechar, "sec") == 0) {// Derivative of sec(x) = sec(x) * tan(x)
            sprintf(str1, "%.2lfsec(x)*tan(x)", current->coefficient);
        }else if (strcmp(current->typechar, "csc") == 0) {// Derivative of sec(x) = sec(x) * tan(x)
            sprintf(str1, "%.2lfcosec(x)*cot(x)", -current->coefficient);
        }else if (strcmp(current->typechar, "cot") == 0) {// Derivative of sec(x) = sec(x) * tan(x)
            sprintf(str1, "%.2lfcosec^2(x)", -current->coefficient);
        } else if (strcmp(current->typechar, "log") == 0) {// Derivative of log(x) = 1/x
            sprintf(str1, "%.2lfx^%d", current->coefficient,-1);
        } else if (strcmp(current->typechar, "e^") == 0) {// Derivative of e^x = e^x
            sprintf(str1, "%.2lfe^x", current->coefficient,1);
        }
    return str1;
}

Term* compoDerivative(Term* poly) {
    Term* derivativePoly = NULL;
    Term* current = poly;
    char* str = (char*)malloc(sizeof(char) * 40);
    while (current != NULL) {
        if (strcmp(current->typechar, "POLYNOMIAL") == 0 && current->exponent > 0) {
            double newCoefficient = current->coefficient * current->exponent;
            int newExponent = current->exponent - 1;
            sprintf(str, "%.2lfx^%d", newCoefficient, newExponent);
            addTerm(&derivativePoly, createTerm(newCoefficient, newExponent, str));
            
        } else if (strcmp(current->typechar, "sin") == 0) {// Derivative of sin(x) = cos(x)
            sprintf(str, "%.2lfcos(x)", current->coefficient);
            addTerm(&derivativePoly, createTerm(current->coefficient, 1, str));

        } else if (strcmp(current->typechar, "cos") == 0) {// Derivative of cos(x) = -sin(x)
            sprintf(str, " %.2lf sin(x)", -current->coefficient);
            addTerm(&derivativePoly, createTerm(-current->coefficient, 1, str));
            
        } else if (strcmp(current->typechar, "tan") == 0) {// Derivative of tan(x) = sec^2(x)
            sprintf(str, "%.2lfsec^2(x)", current->coefficient);
            addTerm(&derivativePoly, createTerm(current->coefficient, 2, str));

        } else if (strcmp(current->typechar, "sec") == 0) {// Derivative of sec(x) = sec(x) * tan(x)
            sprintf(str, "%.2lfsec(x)*tan(x)", current->coefficient);
            addTerm(&derivativePoly, createTerm(current->coefficient, 1, str));

        }else if (strcmp(current->typechar, "csc") == 0) {// Derivative of sec(x) = sec(x) * tan(x)
            sprintf(str, "%.2lfcosec(x)*cot(x)", -current->coefficient);
            addTerm(&derivativePoly, createTerm(current->coefficient, 1, str));
            
        }else if (strcmp(current->typechar, "cot") == 0) {// Derivative of sec(x) = sec(x) * tan(x)
            sprintf(str, "%.2lfcosec^2(x)", -current->coefficient);
            addTerm(&derivativePoly, createTerm(current->coefficient, 1, str));
            
        } else if (strcmp(current->typechar, "log") == 0) {// Derivative of log(x) = 1/x
            sprintf(str, "%.2lfx^%d", current->coefficient,-1);
            addTerm(&derivativePoly, createTerm(current->coefficient, -1, str));
            
        } else if (strcmp(current->typechar, "e^") == 0) {// Derivative of e^x = e^x
            char* str = (char*)malloc(sizeof(char) * 40);
            sprintf(str, "%.2lfe^x", current->coefficient,1);
            addTerm(&derivativePoly, createTerm(current->coefficient, 1, str));
            
        } else {
            Term* f = current->factor1;
            Term* g = current->factor2;
            Term* fPrime = compoDerivative(f);
            Term* gPrime = compoDerivative(g);
            char* term1 = (char*)malloc(sizeof(char) * 40);
            char* term2 = (char*)malloc(sizeof(char) * 40);
            char* ftype = (char*)malloc(sizeof(char) * 40); 
            char* gtype = (char*)malloc(sizeof(char) * 40);
            strcpy(ftype, getTerm(f));
            strcpy(gtype, getTerm(g));  
            sprintf(term1, "%s * %s", ftype, gPrime->typechar);
            sprintf(term2, "%s * %s", gtype, fPrime->typechar);
            addTerm(&derivativePoly, createTerm(f->coefficient * gPrime->coefficient, f->exponent + gPrime->exponent, term1));
            addTerm(&derivativePoly, createTerm(g->coefficient * fPrime->coefficient, g->exponent + fPrime->exponent, term2));
        }
        if(current->internal == NULL) {
            continue;
        }
        else {
            char* compo = (char*)malloc(sizeof(char) * 40);
            char* base = (char*)malloc(sizeof(char) * 40);
            compo = getTerm(current->internal);
            strcat(compo, ")");
            prependString(compo, "(");
            printf("%s\n", compo);
            strcpy(base, str);
            //while(strstr(base, "(x)") != NULL) {
                replaceSubstring(base, "(x)", compo); ////////////aprehend 
            //}
            strcat(base, " * ");
            Term* internalDerivative = current->internal;
            strcpy(compo, deriveTerm(internalDerivative));
            strcat(base, compo);
            Term* temp = derivativePoly;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            strcpy(temp->typechar, base);
        }
        current = current->next;
    }
    return derivativePoly;
}

Term* derivative(Term* poly) {
    Term* derivativePoly = NULL;
    Term* current = poly;
    if(current->internal != NULL)
    return compoDerivative(poly);
    while (current != NULL) {
        if (strcmp(current->typechar, "POLYNOMIAL") == 0 && current->exponent > 0) {
            double newCoefficient = current->coefficient * current->exponent;
            int newExponent = current->exponent - 1;
            char* str = (char*)malloc(sizeof(char) * 40);
            sprintf(str, "%.2lfx^%d", newCoefficient, newExponent);
            addTerm(&derivativePoly, createTerm(newCoefficient, newExponent, str));
            free(str);
        } else if (strcmp(current->typechar, "sin") == 0) {
            char* str = (char*)malloc(sizeof(char) * 40);
            sprintf(str, "%.2lfcos(x)", current->coefficient);
            addTerm(&derivativePoly, createTerm(current->coefficient, 1, str));
            free(str);
        } else if (strcmp(current->typechar, "cos") == 0) {
            char* str = (char*)malloc(sizeof(char) * 40);
            sprintf(str, " %.2lfsin(x)", -current->coefficient);
            addTerm(&derivativePoly, createTerm(-current->coefficient, 1, str));
            free(str);
        } else if (strcmp(current->typechar, "tan") == 0) {
            char* str = (char*)malloc(sizeof(char) * 40);
            sprintf(str, "%.2lfsec^2(x)", current->coefficient);
            addTerm(&derivativePoly, createTerm(current->coefficient, 2, str));
            free(str);
        } else if (strcmp(current->typechar, "sec") == 0) {
            char* str = (char*)malloc(sizeof(char) * 40);
            sprintf(str, "%.2lfsec(x)*tan(x)", current->coefficient);
            addTerm(&derivativePoly, createTerm(current->coefficient, 1, str));
            free(str);
        }else if (strcmp(current->typechar, "csc") == 0) {
            char* str = (char*)malloc(sizeof(char) * 40);
            sprintf(str, "%.2lfcosec(x)*cot(x)", -current->coefficient);
            addTerm(&derivativePoly, createTerm(current->coefficient, 1, str));
            free(str);
        }else if (strcmp(current->typechar, "cot") == 0) {
            char* str = (char*)malloc(sizeof(char) * 40);
            sprintf(str, "%.2lfcosec^2(x)", -current->coefficient);
            addTerm(&derivativePoly, createTerm(current->coefficient, 1, str));
            free(str);
        } else if (strcmp(current->typechar, "log") == 0) {
            char* str = (char*)malloc(sizeof(char) * 40);
            sprintf(str, "%.2lfx^%d", current->coefficient,-1);
            addTerm(&derivativePoly, createTerm(current->coefficient, -1, str));
            free(str);
        } else if (strcmp(current->typechar, "e^") == 0) {
            char* str = (char*)malloc(sizeof(char) * 40);
            sprintf(str, "%.2lfe^x", current->coefficient,1);
            addTerm(&derivativePoly, createTerm(current->coefficient, 1, str));
            free(str);
        } else if (strcmp(current->typechar, "CONSTANT") == 0){
            continue;
        } else {
            Term* f = current->factor1;
            Term* g = current->factor2;
            Term* fPrime = derivative(f);
            Term* gPrime = derivative(g);
            char* term1 = (char*)malloc(sizeof(char) * 40);
            char* term2 = (char*)malloc(sizeof(char) * 40);
            char* ftype = (char*)malloc(sizeof(char) * 40); 
            char* gtype = (char*)malloc(sizeof(char) * 40);
            strcpy(ftype, getTerm(f));
            strcpy(gtype, getTerm(g));  
            sprintf(term1, "%s * %s", ftype, gPrime->typechar);
            sprintf(term2, "%s * %s", gtype, fPrime->typechar);
            addTerm(&derivativePoly, createTerm(f->coefficient * gPrime->coefficient, f->exponent + gPrime->exponent, term1));
            addTerm(&derivativePoly, createTerm(g->coefficient * fPrime->coefficient, g->exponent + fPrime->exponent, term2));
            
        }
        current = current->next;
    }
    return derivativePoly;
}
