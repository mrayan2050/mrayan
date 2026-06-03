#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef struct {
    char type; 
    double val;
} Token;

Token tokens[1024];
int token_count = 0;

void tokenize(const char *str) {
    int len = strlen(str);
    char last_type = 'O'; 

    for (int i = 0; i < len; i++) {
        if (isspace(str[i])) continue;

        if (isdigit(str[i]) || str[i] == '.') {
            char *end;
            tokens[token_count].type = 'N';
            tokens[token_count].val = strtod(&str[i], &end);
            i += (end - &str[i]) - 1;
            token_count++;
            last_type = 'N';
        } 
        else if (str[i] == '|') {
            if (last_type == 'O' || last_type == '(' || last_type == '[') {
                tokens[token_count++].type = '['; 
                last_type = '[';
            } else {
                tokens[token_count++].type = ']'; 
                last_type = ']';
            }
        } 
        else {
            tokens[token_count++].type = str[i];
            last_type = (str[i] == ')') ? 'N' : 'O';
        }
    }
}

double evaluate(int start, int end) {
    double values[1024];
    char ops[1024];
    int v_top = -1, o_top = -1;

    for (int i = start; i <= end; i++) {
        char type = tokens[i].type;

        if (type == 'N') {
            values[++v_top] = tokens[i].val;
        } 
        else if (type == '(' || type == '[') {
            int count = 1;
            int j = i + 1;
            char target_close = (type == '(') ? ')' : ']';
            char target_open = type;

            while (j <= end && count > 0) {
                if (tokens[j].type == target_open) count++;
                if (tokens[j].type == target_close) count--;
                if (count == 0) break;
                j++;
            }

            double sub_res = evaluate(i + 1, j - 1);
            if (type == '[') sub_res = fabs(sub_res);

            values[++v_top] = sub_res;
            i = j; 
        } 
        else if (type == '+' || type == '-' || type == '*' || type == '/') {
            while (o_top >= 0 && ((ops[o_top] == '*' || ops[o_top] == '/') || 
                   ((ops[o_top] == '+' || ops[o_top] == '-') && (type == '+' || type == '-')))) {
                double val2 = values[v_top--];
                double val1 = values[v_top--];
                char op = ops[o_top--];
                
                if (op == '+') values[++v_top] = val1 + val2;
                if (op == '-') values[++v_top] = val1 - val2;
                if (op == '*') values[++v_top] = val1 * val2;
                if (op == '/') values[++v_top] = val1 / val2;
            }
            ops[++o_top] = type;
        }
    }

    while (o_top >= 0) {
        double val2 = values[v_top--];
        double val1 = values[v_top--];
        char op = ops[o_top--];

        if (op == '+') values[++v_top] = val1 + val2;
        if (op == '-') values[++v_top] = val1 - val2;
        if (op == '*') values[++v_top] = val1 * val2;
        if (op == '/') values[++v_top] = val1 / val2;
    }

    return values[0];
}

int main() {
    char input[1024];
    if (fgets(input, sizeof(input), stdin) == NULL) return 0;

    tokenize(input);
    printf("%g\n", evaluate(0, token_count - 1));

    return 0;
}