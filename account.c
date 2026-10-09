#include "account.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR(x) #x
#define XSTR(x) STR(x)
#define NAME_FMT "%" XSTR(NAME_MAX_LEN_M1) "s"
#define NAME_MAX_LEN_M1 63

static Account **v_list = NULL;
static size_t v_len = 0;
static size_t v_cap = 0;
static long cumulative_acct_num = 0;

static int list_push(Account *a) {
    if (v_len == v_cap) {
        size_t ncap = v_cap ? v_cap * 2 : 16;
        Account **n = realloc(v_list, ncap * sizeof *n);
        if (!n)
            return -1;
        v_list = n;
        v_cap = ncap;
    }
    v_list[v_len++] = a;
    return 0;
}

static Account *account_new(long num, const char *first, const char *last, long amt) {
    Account *a = malloc(sizeof *a);
    if (!a)
        return NULL;
    a->acct_num = num;
    snprintf(a->first_name, sizeof a->first_name, "%s", first);
    snprintf(a->last_name, sizeof a->last_name, "%s", last);
    a->acct_amt = amt;
    return a;
}

static void flush_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* Prompt for a long; returns 1 on success, 0 on bad input/EOF. */
static int read_long(long *out) {
    if (scanf("%ld", out) == 1)
        return 1;
    if (!feof(stdin))
        flush_line();
    return 0;
}

static int read_name(char *out) {
    return scanf(NAME_FMT, out) == 1;
}

void account_init(void) {
    FILE *f = fopen(LEDGER_FILE, "r");
    if (f) {
        long num, amt;
        char first[NAME_MAX_LEN], last[NAME_MAX_LEN];
        while (fscanf(f, "%ld " NAME_FMT " " NAME_FMT " %ld", &num, first, last, &amt) == 4) {
            Account *a = account_new(num, first, last, amt);
            if (!a || list_push(a) != 0) {
                free(a);
                break;
            }
        }
        fclose(f);
    }
    cumulative_acct_num = v_len ? v_list[v_len - 1]->acct_num : 0;
}

void account_shutdown(void) {
    account_ledger_dump();
    for (size_t i = 0; i < v_len; i++)
        free(v_list[i]);
    free(v_list);
    v_list = NULL;
    v_len = v_cap = 0;
}

Account *account_search(long acct_num) {
    for (size_t i = 0; i < v_len; i++)
        if (v_list[i]->acct_num == acct_num)
            return v_list[i];
    return NULL;
}

void account_print(const Account *a) {
    printf("\nAccount Number: %ld\nFirst Name: %s\nLast Name: %s\nAccount Amount: %ld",
           a->acct_num, a->first_name, a->last_name, a->acct_amt);
}

static void write_record(FILE *f, const Account *a) {
    fprintf(f, "\n%ld\n%s\n%s\n%ld", a->acct_num, a->first_name, a->last_name, a->acct_amt);
}

void account_ledger_dump(void) {
    FILE *f = fopen(LEDGER_FILE, "w");
    if (!f) {
        perror(LEDGER_FILE);
        return;
    }
    for (size_t i = 0; i < v_len; i++)
        write_record(f, v_list[i]);
    fclose(f);
}

void account_open(void) {
    char first[NAME_MAX_LEN], last[NAME_MAX_LEN];
    long amt;

    printf("\n*OPEN AN ACCOUNT*\n");
    printf("First Name: \n");
    if (!read_name(first)) return;
    printf("Last Name: \n");
    if (!read_name(last)) return;
    printf("Account Amount: \n");
    if (!read_long(&amt)) {
        printf("Invalid amount.\n");
        return;
    }

    Account *a = account_new(cumulative_acct_num + 1, first, last, amt);
    if (!a || list_push(a) != 0) {
        free(a);
        fprintf(stderr, "Out of memory.\n");
        return;
    }
    cumulative_acct_num++;
    account_print(a);
    printf("\n");

    FILE *f = fopen(LEDGER_FILE, "a");
    if (f) {
        write_record(f, a);
        fclose(f);
    } else {
        perror(LEDGER_FILE);
    }
}

void account_balance(void) {
    long num;
    printf("\n*BALANCE ENQUIRY*\n");
    printf("Enter Account Number: \n");
    if (!read_long(&num)) {
        printf("Account Not Found. \n");
        return;
    }
    Account *a = account_search(num);
    if (a) {
        account_print(a);
        printf("\n");
    } else {
        printf("Account Not Found. \n");
    }
}

void account_deposit(void) {
    long num, amt;
    printf("\n*DEPOSIT*\n");
    printf("Enter Account Number: \n");
    if (!read_long(&num)) {
        printf("Account Not Found. \n");
        return;
    }
    Account *a = account_search(num);
    if (!a) {
        printf("Account Not Found. \n");
        return;
    }
    account_print(a);
    printf("\n\nEnter Deposit Amount: \n");
    if (!read_long(&amt)) {
        printf("Invalid amount.\n");
        return;
    }
    a->acct_amt += amt;
    printf("Total Amount %ld has been deposited into Account Number %ld\n", amt, num);
    account_ledger_dump();
    account_print(a);
    printf("\n");
}

void account_withdraw(void) {
    long num, amt;
    printf("\n*WITHDRAWAL*\n");
    printf("Enter Account Number: \n");
    if (!read_long(&num)) {
        printf("Account Not Found. \n");
        return;
    }
    Account *a = account_search(num);
    if (!a) {
        printf("Account Not Found. \n");
        return;
    }
    account_print(a);
    printf("\n\nEnter Withdrawal Amount: \n");
    if (!read_long(&amt)) {
        printf("Invalid amount.\n");
        return;
    }
    a->acct_amt -= amt;
    printf("Total Amount %ld has been withdrawn into Account Number %ld\n", amt, num);
    account_ledger_dump();
    account_print(a);
    printf("\n");
}

void account_close(void) {
    long num;
    printf("\n *CLOSE ACCOUNT* \n");
    printf("Enter Account Number: \n");
    if (!read_long(&num)) {
        printf("Account Not Found. \n");
        return;
    }
    for (size_t i = 0; i < v_len; i++) {
        if (v_list[i]->acct_num == num) {
            account_print(v_list[i]);
            printf("\n");
            free(v_list[i]);
            memmove(&v_list[i], &v_list[i + 1], (v_len - i - 1) * sizeof *v_list);
            v_len--;
            account_ledger_dump();
            printf("Account Number %ld has been closed.\n", num);
            return;
        }
    }
    printf("Account Not Found. \n");
}

void account_show_all(void) {
    printf("\n*ALL ACCOUNTS*");
    for (size_t i = 0; i < v_len; i++) {
        account_print(v_list[i]);
        printf("\n");
    }
}
