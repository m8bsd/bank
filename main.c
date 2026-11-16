#include <stdio.h>

#include "account.h"

int main(void) {
    int option = 0;

    account_init();
    printf("\n*WELCOME TO BANKING SYSTEM*\n");
    while (option != 7) {
        printf("\nSelect one option below: "
               "\n1. Open an Account"
               "\n2. Balance Enquiry"
               "\n3. Deposit"
               "\n4. Withdrawal"
               "\n5. Close an Account"
               "\n6. Show All Accounts"
               "\n7. Quit\n");
        if (scanf("%d", &option) != 1) {
            if (feof(stdin)) {
                option = 7; /* EOF: save and quit */
            } else {
                int c;
                while ((c = getchar()) != '\n' && c != EOF)
                    ;
                option = 0;
            }
        }
        switch (option) {
            case 1: account_open(); break;
            case 2: account_balance(); break;
            case 3: account_deposit(); break;
            case 4: account_withdraw(); break;
            case 5: account_close(); break;
            case 6: account_show_all(); break;
            case 7:
                account_shutdown();
                printf("We hope to see you soon! Bye!\n");
                break;
            default:
                printf("*Please enter a valid option (1~7)*\n");
                break;
        }
    }
    return 0;
}
