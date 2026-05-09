#include <iostream>
#include <vector>
#include "Account.h"

using std::cout;
using std::cin;
using std::endl;
using std::vector;

int main() {
    int option = 0;
    int *const option_ptr = &option;
    cout << "\n*WELCOME TO BANKING SYSTEM*" << endl;
    while (*option_ptr != 7) {
        cout << "\nSelect one option below: "
             << "\n1. Open an Account"
             << "\n2. Balance Enquiry"
             << "\n3. Deposit"
             << "\n4. Withdrawal"
             << "\n5. Close an Account"
             << "\n6. Show All Accounts"
             << "\n7. Quit" << endl;
        cin >> *option_ptr;
        switch (*option_ptr) {
            case 1:
                Account::open();
                break;
            case 2:
                Account::balance();
                break;
            case 3:
                Account::deposit();
                break;
            case 4:
                Account::withdraw();
                break;
            case 5:
                Account::close();
                break;
            case 6:
                Account::showAll();
                break;
            case 7: {
                Account::ledgerDump();

                // Delete all vector pointers.
                vector<Account *>::iterator itr;
                for (itr = Account::v_list.begin(); itr != Account::v_list.end(); itr++) {
                    delete *itr;
                }
                cout << "We hope to see you soon! Bye!" << endl;
                break;
            }
            default:
                cout << "*Please enter a valid option (1~7)*" << endl;
                break;
        }
    }
    delete option_ptr;
    return 0;
}