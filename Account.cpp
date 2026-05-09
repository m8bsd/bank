//
// Created by 0xboz on 8/24/19.
//

#include "Account.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using std::endl;
using std::cout;
using std::cin;
using std::ios;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::string;
using std::ostream;

vector<Account *> Account::v_list = Account::getAll();               // Get all account info
long Account::cumulative_acct_num = Account::getLastAccountNumber(); // Get the latest account number

// Constructor
// Use member initialization list instead of assignment
Account::Account(string first_name, string last_name, const long &acct_amt) :
        first_name(std::move(first_name)),  // https://stackoverflow.com/questions/16724657/why-do-we-copy-then-move
        last_name(std::move(last_name)),
        acct_amt(acct_amt),
        acct_num(++cumulative_acct_num) {
}
Account::Account(const long &account_num, string first_name, string last_name, const long &acct_amt) :
        first_name(std::move(first_name)),
        last_name(std::move(last_name)),
        acct_amt(acct_amt),
        acct_num(account_num) {
}
Account::Account(const Account &a) :
        first_name(a.getFirstName()),
        last_name(a.getLastName()),
        acct_amt(a.getAccountAmount()),
        acct_num(++cumulative_acct_num) {
}
// Mutator
void Account::setFirstName(const string &fn) const {
    this->first_name = fn;
}

void Account::setLastName(const string &ln) const {
    this->last_name = ln;
}

void Account::setAccountAmount(const long &account_amt) const {
    this->acct_amt = account_amt;
}

// Accessor
const long &Account::getAccountNumber() const {
    return acct_num;
}

const string &Account::getFirstName() const {
    return first_name;
}

const string &Account::getLastName() const {
    return last_name;
}

const long &Account::getAccountAmount() const {
    return acct_amt;
}

vector<Account *> Account::getAll() {
    vector<Account *> list;
    ifstream ifs;
    ifs.open("bank_ledger.txt");
    if (!ifs)
        return {};
    // Load all account info to a vector of Account class pointers
    while (!ifs.eof())
        ifs >> list;
    return list;
}

long Account::getLastAccountNumber() {
    // Return the latest account number in the vector
    return Account::v_list.empty() ? 0 : (*Account::v_list.back()).getAccountNumber();
}

// Facilitators
void Account::open() {
    string first_name;
    string last_name;
    cout << "\n*OPEN AN ACCOUNT*" << endl;
    cout << "First Name: " << endl;
    cin >> first_name;
    cout << "Last Name: " << endl;
    cin >> last_name;
    long acct_amt;
    cout << "Account Amount: " << endl;
    cin >> acct_amt;
    v_list.push_back(new Account("", "", 0));
    v_list.back()->setFirstName(first_name);
    v_list.back()->setLastName(last_name);
    v_list.back()->setAccountAmount(acct_amt);
    cout << *v_list.back() << endl;
    // Save to a txt file
    ofstream ofs("bank_ledger.txt", ios::app);
    ofs << *v_list.back();
    ofs.close();
}

Account *Account::searchByAccountNumber(const long &acct_num) {
    vector<Account *>::iterator itr;
    //    https://stackoverflow.com/questions/15598607/when-should-i-use-stdbind#15598734
    itr = find_if(v_list.begin(), v_list.end(),
                  [&](auto acct_ptr) { return compareAccountNumber(acct_ptr, acct_num); });
    if (itr != v_list.end())
        return *itr;
    else {
        Account *ptr = nullptr;
        return ptr;
    }
}

void Account::balance() {
    long balance_account_number;
    cout << "\n*BALANCE ENQUIRY*" << endl;
    cout << "Enter Account Number: " << endl;
    cin >> balance_account_number;
    Account *balance_ptr = searchByAccountNumber(balance_account_number);
    if (balance_ptr != nullptr)
        cout << *balance_ptr << endl;
    else
        cout << "Account Not Found. " << endl;
}

void Account::ledgerDump() {
    ofstream ofs("bank_ledger.txt", ios::trunc);
    for (auto x : v_list)
        ofs << *x;
    ofs.close();
}

void Account::deposit() {
    long deposit_account_number;
    long deposit_amt;
    cout << "\n*DEPOSIT*" << endl;
    cout << "Enter Account Number: " << endl;
    cin >> deposit_account_number;
    Account *deposit_ptr = searchByAccountNumber(deposit_account_number);
    if (deposit_ptr != nullptr) {
        cout << *deposit_ptr << endl;
        cout << "\nEnter Deposit Amount: " << endl;
        cin >> deposit_amt;
        deposit_ptr->setAccountAmount(deposit_amt + deposit_ptr->getAccountAmount());
        cout << "Total Amount "
             << deposit_amt
             << " has been deposited into Account Number "
             << deposit_account_number
             << endl;
        // Update the ledger file
        ledgerDump();
        cout << *deposit_ptr << endl;
    } else
        cout << "Account Not Found. " << endl;
}

void Account::withdraw() {
    long withdraw_account_number;
    long withdraw_amt;
    cout << "\n*WITHDRAWAL*" << endl;
    cout << "Enter Account Number: " << endl;
    cin >> withdraw_account_number;
    Account *withdraw_ptr = searchByAccountNumber(withdraw_account_number);
    if (withdraw_ptr != nullptr) {
        cout << *withdraw_ptr << endl;
        cout << "\nEnter Withdrawal Amount: " << endl;
        cin >> withdraw_amt;
        withdraw_ptr->setAccountAmount(withdraw_ptr->getAccountAmount() - withdraw_amt);
        cout << "Total Amount "
             << withdraw_amt
             << " has been withdrawn into Account Number "
             << withdraw_account_number
             << endl;

        // Update the ledger file
        ledgerDump();
        cout << *withdraw_ptr << endl;
    } else
        cout << "Account Not Found. " << endl;
}

void Account::close() {
    cout << "\n *CLOSE ACCOUNT* " << endl;
    long close_account_number;
    cout << "Enter Account Number: " << endl;
    cin >> close_account_number;
    Account *close_ptr = searchByAccountNumber(close_account_number);
    if (close_ptr != nullptr) {
        vector<Account *>::iterator iter;
        for (iter = v_list.begin(); iter < v_list.end(); ++iter) {
            if ((*iter)->getAccountNumber() == close_account_number) {
                cout << **iter << endl;
                v_list.erase(iter);
            }
        }
        ledgerDump();
        cout << "Account Number " << close_account_number
             << " has been closed." << endl;
    } else
        cout << "Account Not Found. " << endl;
}

void Account::showAll() {
    cout << "\n*ALL ACCOUNTS*";
    for (auto acct_ptr : v_list)
        cout << *acct_ptr << endl;
}

// Destructors
Account::~Account() = default;

// Friend Functions
ifstream const &operator>>(ifstream &ifs, vector<Account *> &v_list) {
    long account_num;
    string first_name;
    string last_name;
    long acct_amt;
    ifs >> account_num >> first_name >> last_name >> acct_amt;
    v_list.push_back(new Account(account_num, first_name, last_name, acct_amt));
    return ifs;
}

ostream &operator<<(ostream &out, const Account &a) {
    out << "\nAccount Number: " << a.getAccountNumber() << endl
        << "First Name: " << a.getFirstName() << endl
        << "Last Name: " << a.getLastName() << endl
        << "Account Amount: " << a.getAccountAmount();
    return out;
}

const ofstream &operator<<(ofstream &ofs, const Account &a) {
    ofs << "\n"
        << a.getAccountNumber() << endl
        << a.getFirstName() << endl
        << a.getLastName() << endl
        << a.getAccountAmount();
    return ofs;
}