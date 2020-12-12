#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*  Function which gets the input string and returns
    pointer to it. Length is stored to the address given
    as argument */
char * get_frm(int *length)
{   int l;
	char *frm = calloc (1000, sizeof (char));
    if (!fgets (frm, 999, stdin)) {
        fprintf(stderr, "String size exceeds 1000");
        exit(1);
    }
    l = strlen (frm);
    frm [l - 1] = '\0';
    l--;
    frm = realloc (frm, sizeof(*frm) * (l + 1));
    *length = l;
    return frm;
}

void print_stacks (double *numbers, int ndepth, char *signs, int sdepth)
{
    int i;
    for (i = 0; i < ndepth; i++) {
        printf ("%.4f ", numbers[i]);
    }
    printf("\n");
    for (i = 0; i < ndepth; i++) {
        printf ("%c ", signs[i]);
    }
    printf("\n");
}

// Function to help operation(...) function
double getresult (double first, double second, char op)
{
    if (op == '+') return first+second;
    if (op == '-') return first-second;
    if (op == '*') return first*second;
    if (op == '/') return first/second;
    return 0.0;
}

// Perform an operation
void operation (double *numbers, int *ndepth, char *signs, int *sdepth)
{
    numbers[*ndepth - 2] = getresult (numbers[*ndepth - 2], numbers[*ndepth - 1], signs [*sdepth - 1]);
    (*ndepth) --;
    (*sdepth) --;
}

// Is c a digit?
int digit (char c)
{
    return ((c >= '0') && (c <= '9'));
}

// Function which puts new operator with priority
void put_with_pr(char *signs, int *sdepth, int *wt, char x, int pr)
{
    signs[*sdepth] = x;
    (*sdepth)++;
    (*wt) = pr;
}

void fix_wt(char *signs, int sdepth, int *wt)
{
    int i = sdepth - 1, f = 0;
    char c;
    while (i >= 0) {
        c = signs[i];
        if ((c == '*') || (c == '/') || (c == '+') || (c == '-')) {
            f = 1;
        }
        i--;
    }
    if (f == 0) (*wt) = 0;
}

int main(void)
{   char *s;
    //I allocate 1000 signs buffer having an idea it is enough.
    //l is the length of my input string.
    int l, i, k = 0;
    char *frm = get_frm(&l);
    //"Stack" of numbers inputed. ndepth is the length of it.
    double *numbers = calloc (l,sizeof(double));
    int ndepth = 0;
    //"Stack" of operatos inputed. sdepth is the length of it.
    char *signs = calloc (l, sizeof(char));
    int sdepth = 0;
    /*  wt - priority marker.
        3 - Priority of * and /
        2 - Priority of + and -
        1 - No priority           */
    int wt = 0;
    // Number, Sign and Dot pointers
    int numpointer = 0;
    int signpointer = 0;
    // Variables used for reading doubles
    double fraction = 1.0;
    char *endptr;
    // For-loop for string proceeding
    s = calloc (25, sizeof (s));
    for (i = 0; i < l; i++) {
        if  (digit(frm[i]) || (frm[i] == '.')) {
            s[k] = frm[i];
            k ++ ;
            numpointer = 1;
        } else {
            if (numpointer) {
                s[k] = ' ';
                k++;
                s[k] = '\0';
                numbers[ndepth] = strtod (s,&endptr);
                ndepth ++;
                numpointer = 0;
                k = 0;
                s[k] = '\0';
            }
            if ((frm[i] == '*') || (frm[i] == '/')) {
                if (wt >= 2) {
                    operation (numbers, &ndepth, signs, &sdepth);
                }
                put_with_pr(signs,&sdepth,&wt,frm[i],2);
            } else if (frm[i] == '+') {
                if (wt >= 1) {
                    operation (numbers, &ndepth, signs, &sdepth);
                }
                put_with_pr(signs,&sdepth,&wt,frm[i],1);
            } else if (frm[i] == '-') {
                if (wt >= 1) {
                    operation (numbers, &ndepth, signs, &sdepth);
                }
                if (i && frm[i - 1] != '(') {
                    put_with_pr(signs, &sdepth, &wt, '+', 1);
                }
                numbers[ndepth] = -1.0;
                ndepth++;
                if (wt >= 2) {
                    operation (numbers, &ndepth, signs, &sdepth);
                }
                put_with_pr(signs,&sdepth,&wt,'*',2);
            } else if (frm[i] == '(') {
                put_with_pr(signs,&sdepth,&wt,frm[i],0);
            } else if (frm[i] == ')') {
                while (signs [sdepth - 1] != '(') {
                    operation (numbers, &ndepth, signs, &sdepth);
                }
                sdepth --;
                /* If there was an operator with priority 2
                before brackets execute it. */
                if ((sdepth - 1 >= 0) && (signs[sdepth - 1] == '*' || signs[sdepth - 1] == '/')) {
                    wt = 2;
                } else {
                    wt = 1;
                }
                fix_wt(signs,sdepth,&wt);
                continue;
            
            } else if (frm[i] != ' ') {
                fprintf(stderr, "Invalid characters\n");
                exit(1);
            }
            if (!digit(frm[i])) {
                fraction = 1.0;
            }
            fix_wt(signs,sdepth,&wt);
        }
    }
    if (numpointer) {
        s[k] = ' ';
        k++;
        s[k] = '\0';
        numbers[ndepth] = strtod (s,&endptr);
        ndepth ++;
        numpointer = 0;
        k = 0;
        s[k] = '\0';
    }
    //Post for-loop operations
    if ((l - 1) && digit(frm[l - 1]) && signpointer) {
        signpointer = 0;
        numbers[ndepth - 1] *= (-1.0);
    }
    signs[sdepth]='\0';
    while (sdepth != 0) {
        operation (numbers, &ndepth, signs, &sdepth);
    }

    //Print the final result
    printf ("\nThe result is: %.5f\n", numbers[0]);

    //Free up all allocated pointers.
    free (numbers);
    free (frm);
    free (signs);
    return 0;
}
