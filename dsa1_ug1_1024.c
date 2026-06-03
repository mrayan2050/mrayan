#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

const char *expr;

double parse_expression();
double parse_term();
double parse_factor();

void skip_whitespace() {
    while (*expr == ' ' || *expr == '\t') {
        expr++;
    }
}

double parse_expression() {
    double result = parse_term();
    skip_whitespace();
    while (*expr == '+' || *expr == '-') {
        char op = *expr;
        expr++;
        double next_term = parse_term();
        if (op == '+') result += next_term;
        else result -= next_term;
        skip_whitespace();
    }
    return result;
}

double parse_term() {
    double result = parse_factor();
    skip_whitespace();
    while (*expr == '*' || *expr == '/') {
        char op = *expr;
        expr++;
        double next_factor = parse_factor();
        if (op == '*') {
            result *= next_factor;
        } else {
            if (next_factor == 0.0) {
                fprintf(stderr, "Error: Division by zero\n");
                exit(EXIT_FAILURE);
            }
            result /= next_factor;
        }
        skip_whitespace();
    }
    return result;
}

double parse_factor() {
    skip_whitespace();
    
    if (*expr == '-') {
        expr++;
        return -parse_factor();
    }
    if (*expr == '+') {
        expr++;
        return parse_factor();
    }

    if (*expr == '(') {
        expr++; 
        double result = parse_expression();
        skip_whitespace();
        if (*expr == ')') {
            expr++; 
        } else {
            fprintf(stderr, "Error: Mismatched parenthesis\n");
            exit(EXIT_FAILURE);
        }
        return result;
    }

    if (*expr == '|') {
        expr++; 
        double result = parse_expression();
        skip_whitespace();
        if (*expr == '|') {
            expr++; 
        } else {
            fprintf(stderr, "Error: Mismatched absolute value vertical bar\n");
            exit(EXIT_FAILURE);
        }
        return fabs(result);
    }

    if (isdigit((unsigned char)*expr) || *expr == '.') {
        char *next_ptr;
        double result = strtod(expr, &next_ptr);
        expr = next_ptr;
        return result;
    }

    fprintf(stderr, "Error: Unexpected character '%c'\n", *expr);
    exit(EXIT_FAILURE);
}

int main() {
    char input[1024];

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 0;
    }

    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }

    expr = input;
    double result = parse_expression();
    
    skip_whitespace();
    if (*expr != '\0') {
        fprintf(stderr, "Error: Unparsed characters remaining at end of expression\n");
        exit(EXIT_FAILURE);
    }

    printf("%g\n", result);

    return 0;
}