#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>

enum { L_bit = 8, l_bit = 4, W_bit = 2, w_bit = 1, BUF_SIZE = 100 };

struct Elem
{
    int data;
    struct Elem *next;
};

struct List
{
    struct Elem *pHead;
    struct Elem *pTail;
};

// Make up a list
struct List * MakeList(void)
{
    struct List *a = calloc (1, sizeof(struct List));
    a->pHead = NULL;
    return a;
}

// Character to pointer translator
wchar_t *chtostr (wchar_t x)
{
    wchar_t *p = calloc (2, sizeof (wchar_t));
    p[0] = x;
    p[1] = '\0';
    return p;
}

// Add to list
void addToList(int data, struct List *a)
{
    struct Elem *temp = calloc(1, sizeof(struct Elem));
    struct Elem *cnt;
    if (a->pHead == NULL) {
        a->pHead = temp;
        temp->data = data;
        temp->next = NULL;
        a->pTail = temp;
    } else {
        if (a->pHead->data < data) {
            temp->data = data;
            temp->next = a->pHead;
            a->pHead = temp;
        } else if (a->pHead->data > data) {
            cnt = a->pHead;
            while (cnt->next && (cnt->next->data > data)) {
                cnt = cnt->next;
            }
            if (!(cnt->next)) {
                a->pTail->next = temp;
                temp->data = data;
                temp->next = NULL;
                a->pTail = temp;
            } else if (cnt->next->data < data) {
                temp->next = cnt->next;
                cnt->next = temp;
                temp->data = data;
            }
        }
        
    }
}

struct node
{
    struct node *right;
    struct node *left;
    wchar_t *data;
    int quantity;
    short height;
};

// Free up tree
void free_tree (struct node * tree)
{
    if (tree != NULL) {
        free_tree (tree->left);
        free_tree (tree->right);
        free (tree);
    }
}

// Free up element
void free_elem (struct Elem * x)
{
    if (x != NULL) {
        free_elem (x->next);
        free (x);
    }
}

// Free up list
void free_list (struct List a)
{
    free_elem(a.pHead);
}

// Get tree height
char height(struct node *p)
{
    if (p) {
        return p->height;
    } else {
        return 0;
    }
}

// Get balance factor
int BF (struct node *p)
{
    return height(p->right)-height(p->left);
}

// Fix height
void OverHeight(struct node *p)
{
    char hleft = height(p->left);
    char hright = height(p->right);
    p->height = (hleft > hright ? hleft : hright) + 1;
}

// Rotate right around x
struct node* RightRotation(struct node *x)
{
    struct node *y = x->left;
    x->left = y->right;
    y->right = x;
    OverHeight(x);
    OverHeight(y);
    return y;
}

// Rotate left around y
struct node *LeftRotation(struct node *y)
{
    struct node *x = y->right;
    y->right = x->left;
    x->left = y;
    OverHeight(y);
    OverHeight(x);
    return x;
}

// Balance the tree
struct node *Balance(struct node *x)
{
    OverHeight(x);
    if (BF(x) == 2) {
        if (BF(x->right) < 0)
        {
            x->right = RightRotation(x->right);
        }
        return LeftRotation(x);
    }
    if (BF(x) == -2) {
        if (BF(x->left) > 0) {
            x->left = LeftRotation(x->left);
        }
        return RightRotation(x);
    }
    return x;
}

// Insert a new node to tree
struct node *Insert(struct node *x, wchar_t *k)
{
    if (!x) {
        struct node *y = calloc(1,sizeof(struct node));
        y->data = k;
        y->quantity = 1;
        y->height = 1;
        y->left = y->right = NULL;
        return y;
    }
    int f = wcscmp(k,x->data);
    if (f < 0) {
        x->left = Insert(x->left, k);
    } else if (f == 0) {
        x->quantity ++;
    } else {
        x->right = Insert(x->right, k);
    }
    return Balance(x);
}

// Print the tree
void print_tree (struct node* root, FILE *f) {
    if (root) {
        print_tree(root->left,f);
        fwprintf(f, L"%ls  - %d\n",root->data, root->quantity);
        print_tree(root->right,f);
    }
}

// Create a list by quantities of tree nodes
void Init_List (struct node* root, struct List *head) {
    if (root) {
        Init_List(root->left,head);
        addToList(root->quantity,head);
        Init_List(root->right,head);
    }
}

// Print all the nodes of tree with quantity equal to q by lexics
void Go_Through (struct node* root, int q, FILE *f) {
    if (root) {
        Go_Through(root->left,q,f);
        if (root->quantity == q) {
            fwprintf(f,L"%ls - %d\n", root->data, q);
        }
        Go_Through(root->right,q,f);
    }
}

// Print the tree by quantities
void Print_On_Q (struct node* root, struct List *a, FILE *f)
{
    struct Elem *pTemp = a->pHead;
    while (pTemp != NULL)
    {
        Go_Through(root,pTemp->data,f);
        pTemp = pTemp->next;
    }
}

// Check if the command lie arguments are correct
int correct (int flags)
{
    return (!((( (flags & L_bit) > 0) && ( (flags & l_bit) > 0)) ||
              (( (flags & W_bit) > 0) && ( (flags & w_bit) > 0))));
}

/* In flags bytes corresponding to lay as foolows: LlWw,
 1 if exists, 0 if doesn't. For example if we met
 lWL we'll try to write 14 = 1110 in binary.
 As it is innapropriate for our programm we'll check it
 with correct(int) function. */
FILE * flags (int argc, char *argv[], int * flags)
{
    int out = 0;
    FILE *fout;
    int i = 1;
    for (; i < argc; i++) {
        if (argv[i][0] == '-') {
            int l = strlen(argv[i]);
            for (int j = 1; j < l; j++) {
                if (argv[i][j] == 'o') {
                    fout = fopen(argv[i + 1],"w");
                    if (!fout) {
                        fprintf(stderr,"Error opening output file.\n");
                        return NULL;
                    }
                } else if (argv[i][j] == 'L') {
                    out = out | L_bit;
                } else if (argv[i][j] == 'l') {
                    out = out | l_bit;
                } else if (argv[i][j] == 'W') {
                    out = out | W_bit;
                } else if (argv[i][j] == 'w') {
                    out = out | w_bit;
                }
            }
        }
    }
    if (!correct(out)) {
        fprintf(stderr,"Error with programm flags.\n");
        return NULL;
    }
    (*flags) = out;
    return fout;
}

int out_loc (int argc, char *argv[])
{
    int out = 0, f = 0, o, i, j;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (j = 1; j < strlen(argv[i]); j++) {
                if (argv[i][j] == 'o') {
                    f = 1;
                    out ++;
                    o = i + 1;
                }
            }
            if (f) {
                f = 0;
                if (i == argc - 1) return 0;
                if (argv[i + 1][0] == '-') return 0;
            }
        }
    }
    if (out != 1) return 0;
    return o;
}

int in_loc (int argc, char *argv[], int out_loc)
{
    int input = 0, f = 0, in, i;
    for (i = 1; i < argc; i++) {
        if ((argv[i][0] != '-') && (i != out_loc)) {
            input++;
            in = i;
        }
    }
    if (input != 1) return 0;
    return in;
}

int legal (char x)
{
    return ((x == 'l') || (x == 'L') || (x == 'w') || (x == 'W') || (x == 'o'));
}

int legal_flags (int argc, char *argv[])
{
    int i, j;
    for (i = 1; i < argc; i++) {
         if (argv[i][0] == '-') {
            for (j = 1; j < strlen(argv[i]); j++) {
                if (!legal(argv[i][j])) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

int main (int argc, char *argv[])
{
    int out,in, flag = 0, i;
    FILE *fin, *fout;
    wint_t x;
    wchar_t c;
    struct List a,b;
    struct node *STree = NULL, *CTree = NULL;
    wchar_t *buf;
    if ((out = out_loc(argc,argv)) == 0) {
        fprintf(stderr,"Error with flags for output.\n");
        exit(1);
    }
    if ((in = in_loc(argc,argv,out)) == 0) {
        fprintf(stderr,"Error with input tags in command line.\n");
        exit(1);
    }
    fin = fopen(argv[in],"r");
    if ((legal_flags(argc,argv)) == 0) {
        fprintf(stderr,"Error with flags in command line: some of them are not L,l,W,w,o.\n");
        exit(1);
    }
    setlocale(LC_ALL, "");
    if (!fin) {
        fprintf(stderr,"Error opening input file.\n");
        exit(1);
    }
    fout = flags(argc,argv,&flag);
    if (!fout) {
        exit(1);
    }
    if (!flag) {
        flag = 5;
        fwprintf(fout,L"Printing by default (by lexics):\n");
    } else {
        fwprintf(fout,L"Result:\n");
    }
    a.pHead = NULL;
    b.pHead = NULL;
    buf = calloc (BUF_SIZE, sizeof(wchar_t));
    i = 0;
    while ((x = fgetwc(fin)) != EOF) {
        if (!isspace(x)) {
            if (iswupper(x)) {
                x = towlower(x);
            }
            c = (wchar_t) x;
            if ((flag & (L_bit | l_bit)) > 0) {
                CTree = Insert(CTree,chtostr(c));
            }
            if ((flag & (W_bit | w_bit)) > 0) {
                if (iswalpha(x)) {
                    buf[i] = c;
                    i++;
                } else if (i > 0) {
                    i++;
                    buf[i] = '\0';
                    buf = realloc(buf, (i + 1) * sizeof(wchar_t));
                    STree = Insert(STree,buf);
                    buf = calloc (BUF_SIZE, sizeof(wchar_t));
                    i = 0;
                }
            }
        } else if (((flag & (W_bit | w_bit)) > 0) && i) {
            i++;
            buf[i] = '\0';
            buf = realloc(buf, (i + 1) * sizeof(wchar_t));
            STree = Insert(STree,buf);
            buf = calloc (BUF_SIZE, sizeof(wchar_t *));
            i = 0;
        }
    }
    if (((flag & (W_bit | w_bit)) > 0) && i) {
        i++;
        buf[i] = '\0';
        buf = realloc(buf, (i + 1) * sizeof(wchar_t));
        STree = Insert(STree,buf);
    }
    if ((flag & w_bit) == w_bit) {
        fwprintf(fout,L"\nWords by alphabetic order:\n\n");
        print_tree(STree,fout);
    }
    if ((flag & l_bit) == l_bit) {
        fwprintf(fout,L"\nCharacters by alphabetic order:\n\n");
        print_tree(CTree,fout);
    }
    if ((flag & W_bit) == W_bit) {
        fwprintf(fout,L"\nWords by quantity:\n\n");
        Init_List(STree,&b);
        Print_On_Q (STree,&b,fout);
    }
    if ((flag & L_bit) == L_bit) {
        fwprintf(fout,L"\nCharacters by quantity:\n\n");
        Init_List(CTree,&a);
        Print_On_Q (CTree,&a,fout);
    }
    if ((flag & (W_bit | w_bit)) > 0) free_list(a);
    if ((flag & (L_bit | l_bit)) > 0) free_list(b);
    if ((flag & (W_bit | w_bit)) > 0) free_tree(STree);
    if ((flag & (L_bit | l_bit)) > 0) free_tree(CTree);
    return 0;
}
