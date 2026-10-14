#ifndef C_BANKING_SYSTEM_ACCOUNT_H
#define C_BANKING_SYSTEM_ACCOUNT_H

#include <stddef.h>

#define NAME_MAX_LEN 64
#define LEDGER_FILE "bank_ledger.txt"

typedef struct Account {
    long acct_num;
    char first_name[NAME_MAX_LEN];
    char last_name[NAME_MAX_LEN];
    long acct_amt;
} Account;

/* Load the ledger file (if any) and initialise the account counter. */
void account_init(void);
/* Write the ledger and free every account. */
void account_shutdown(void);

Account *account_search(long acct_num);
void account_print(const Account *a);

/* Interactive operations */
void account_open(void);
void account_balance(void);
void account_deposit(void);
void account_withdraw(void);
void account_close(void);
void account_show_all(void);

/* Rewrite the whole ledger file from memory. */
void account_ledger_dump(void);

#endif /* C_BANKING_SYSTEM_ACCOUNT_H */
