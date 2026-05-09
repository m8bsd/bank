//
// Created by 0xboz on 8/24/19.
//

#ifndef CPP_BANKING_SYSTEM_ACCOUNT_H
#define CPP_BANKING_SYSTEM_ACCOUNT_H

#include <vector>
#include <string>

using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ostream;

class Account {
private:
    static long cumulative_acct_num;
    long acct_num;
    mutable string first_name;
    mutable string last_name;
    mutable long acct_amt;

public:
    static vector<Account *> v_list;

    Account(string first_name, string last_name, const long &acct_amt);

    Account(const long &account_num, string first_name, string last_name, const long &acct_amt);

    Account() : Account(getAccountNumber(), "", "", 0L) {};

    Account(const Account &a);

    void setFirstName(const string &fn) const;

    void setLastName(const string &ln) const;

    void setAccountAmount(const long &account_amt) const;

    const long &getAccountNumber() const;

    const string &getFirstName() const;

    const string &getLastName() const;

    const long &getAccountAmount() const;

    static long getLastAccountNumber(); // Retrieve account info and get the latest account number

    static vector<Account *> getAll();

    friend const ifstream &operator>>(ifstream &ifs, vector<Account *> &v_list);

    friend ostream &operator<<(ostream &out, const Account &a);

    friend const ofstream &operator<<(ofstream &ofs, const Account &a);

    static void open();

    static bool compareAccountNumber(const Account *acct_ptr, const long &acct_num) {
        return (*acct_ptr).getAccountNumber() == acct_num;
    }

    static Account *searchByAccountNumber(const long &acct_num);

    static void balance();

    static void ledgerDump();

    static void deposit();

    static void withdraw();

    static void close();

    static void showAll();

    ~Account();
};


#endif //CPP_BANKING_SYSTEM_ACCOUNT_H
