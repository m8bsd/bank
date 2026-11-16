/* Banking System - C version */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_BALANCE 500
#define NAME_LEN 50
#define DATA_FILE "Bank.data"

typedef struct {
    long  accountNumber;
    char  firstName[NAME_LEN];
    char  lastName[NAME_LEN];
    float balance;
} Account;

typedef struct {
    Account *accounts;
    size_t   count;
    size_t   capacity;
} Bank;

static long NextAccountNumber = 0;

/* ---------- Account ---------- */

static void account_print(const Account *acc)
{
    printf("First Name:%s\n", acc->firstName);
    printf("Last Name:%s\n", acc->lastName);
    printf("Account Number:%ld\n", acc->accountNumber);
    printf("Balance:%.2f\n", acc->balance);
}

static void account_deposit(Account *acc, float amount)
{
    acc->balance += amount;
}

/* returns 0 on success, -1 on insufficient funds */
static int account_withdraw(Account *acc, float amount)
{
    if (acc->balance - amount < MIN_BALANCE)
        return -1;
    acc->balance -= amount;
    return 0;
}

/* ---------- Bank ---------- */

static void bank_save(const Bank *b)
{
    FILE *f = fopen(DATA_FILE, "w");
    size_t i;
    if (!f) return;
    for (i = 0; i < b->count; i++) {
        const Account *a = &b->accounts[i];
        fprintf(f, "%ld\n%s\n%s\n%f\n",
                a->accountNumber, a->firstName, a->lastName, a->balance);
    }
    fclose(f);
}

static int bank_add(Bank *b, const Account *acc)
{
    if (b->count == b->capacity) {
        size_t newcap = b->capacity ? b->capacity * 2 : 16;
        Account *tmp = realloc(b->accounts, newcap * sizeof(Account));
        if (!tmp) return -1;
        b->accounts = tmp;
        b->capacity = newcap;
    }
    b->accounts[b->count++] = *acc;
    return 0;
}

static void bank_init(Bank *b)
{
    FILE *f;
    Account acc;

    b->accounts = NULL;
    b->count = 0;
    b->capacity = 0;

    f = fopen(DATA_FILE, "r");
    if (!f) return;

    while (fscanf(f, "%ld %49s %49s %f",
                  &acc.accountNumber, acc.firstName,
                  acc.lastName, &acc.balance) == 4) {
        bank_add(b, &acc);
        if (acc.accountNumber > NextAccountNumber)
            NextAccountNumber = acc.accountNumber;
    }
    fclose(f);
}

static void bank_free(Bank *b)
{
    bank_save(b);
    free(b->accounts);
    b->accounts = NULL;
    b->count = b->capacity = 0;
}

/* returns index or -1 if not found */
static long bank_find(const Bank *b, long accountNumber)
{
    size_t i;
    for (i = 0; i < b->count; i++)
        if (b->accounts[i].accountNumber == accountNumber)
            return (long)i;
    return -1;
}

static Account *bank_open_account(Bank *b, const char *fname,
                                  const char *lname, float balance)
{
    Account acc;
    memset(&acc, 0, sizeof acc);
    acc.accountNumber = ++NextAccountNumber;
    strncpy(acc.firstName, fname, NAME_LEN - 1);
    strncpy(acc.lastName, lname, NAME_LEN - 1);
    acc.balance = balance;

    if (bank_add(b, &acc) != 0) {
        NextAccountNumber--;
        return NULL;
    }
    bank_save(b);
    return &b->accounts[b->count - 1];
}

static Account *bank_balance_enquiry(Bank *b, long accountNumber)
{
    long i = bank_find(b, accountNumber);
    return i < 0 ? NULL : &b->accounts[i];
}

static Account *bank_deposit(Bank *b, long accountNumber, float amount)
{
    Account *acc = bank_balance_enquiry(b, accountNumber);
    if (acc) account_deposit(acc, amount);
    return acc;
}

/* *status: 0 ok, -1 not found, -2 insufficient funds */
static Account *bank_withdraw(Bank *b, long accountNumber, float amount,
                              int *status)
{
    Account *acc = bank_balance_enquiry(b, accountNumber);
    if (!acc) { *status = -1; return NULL; }
    if (account_withdraw(acc, amount) != 0) { *status = -2; return NULL; }
    *status = 0;
    return acc;
}

/* returns 0 on success, -1 if not found */
static int bank_close_account(Bank *b, long accountNumber)
{
    long i = bank_find(b, accountNumber);
    if (i < 0) return -1;
    printf("Account Deleted\n");
    account_print(&b->accounts[i]);
    memmove(&b->accounts[i], &b->accounts[i + 1],
            (b->count - (size_t)i - 1) * sizeof(Account));
    b->count--;
    return 0;
}

static void bank_show_all(const Bank *b)
{
    size_t i;
    for (i = 0; i < b->count; i++) {
        printf("Account %ld\n", b->accounts[i].accountNumber);
        account_print(&b->accounts[i]);
        printf("\n");
    }
}

/* ---------- main ---------- */

int main(void)
{
    Bank b;
    Account *acc;
    int choice = 0, status;
    char fname[NAME_LEN], lname[NAME_LEN];
    long accountNumber;
    float balance, amount;

    bank_init(&b);
    printf("***Banking System***\n");

    do {
        printf("\n\tSelect one option below ");
        printf("\n\t1 Open an Account");
        printf("\n\t2 Balance Enquiry");
        printf("\n\t3 Deposit");
        printf("\n\t4 Withdrawal");
        printf("\n\t5 Close an Account");
        printf("\n\t6 Show All Accounts");
        printf("\n\t7 Quit");
        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input\n");
            break;
        }

        switch (choice) {
        case 1:
            printf("Enter First Name: ");
            scanf("%49s", fname);
            printf("Enter Last Name: ");
            scanf("%49s", lname);
            printf("Enter initial Balance: ");
            scanf("%f", &balance);
            acc = bank_open_account(&b, fname, lname, balance);
            if (acc) {
                printf("\nCongratulations, Account is Created\n");
                account_print(acc);
            } else {
                printf("\nCould not create account\n");
            }
            break;

        case 2:
            printf("Enter Account Number:");
            scanf("%ld", &accountNumber);
            acc = bank_balance_enquiry(&b, accountNumber);
            if (acc) {
                printf("\nYour Account Details\n");
                account_print(acc);
            } else {
                printf("\nAccount not found\n");
            }
            break;

        case 3:
            printf("Enter Account Number:");
            scanf("%ld", &accountNumber);
            printf("Enter Amount:");
            scanf("%f", &amount);
            acc = bank_deposit(&b, accountNumber, amount);
            if (acc) {
                printf("\nAmount is Deposited\n");
                account_print(acc);
            } else {
                printf("\nAccount not found\n");
            }
            break;

        case 4:
            printf("Enter Account Number:");
            scanf("%ld", &accountNumber);
            printf("Enter Amount:");
            scanf("%f", &amount);
            acc = bank_withdraw(&b, accountNumber, amount, &status);
            if (status == 0) {
                printf("\nAmount Withdrawn\n");
                account_print(acc);
            } else if (status == -2) {
                printf("\nInsufficient Funds (minimum balance is %d)\n",
                       MIN_BALANCE);
            } else {
                printf("\nAccount not found\n");
            }
            break;

        case 5:
            printf("Enter Account Number:");
            scanf("%ld", &accountNumber);
            if (bank_close_account(&b, accountNumber) == 0)
                printf("\nAccount is Closed\n");
            else
                printf("\nAccount not found\n");
            break;

        case 6:
            bank_show_all(&b);
            break;

        case 7:
            break;

        default:
            printf("\nEnter correct choice\n");
            break;
        }
    } while (choice != 7);

    bank_free(&b);
    return 0;
}
