# 🏦 Simple Banking System — PHP

A CLI-based banking system written in PHP, converted from the original [C++ project](https://github.com/m8bsd/bank) by m8bsd.

---

## Requirements

- PHP **8.0** or higher
- CLI access (Terminal, Command Prompt, etc.)
- The `readline` extension (included by default on most PHP installations)

Verify your PHP version:

```bash
php --version
```

---

## Installation

1. **Download** `bank.php` and place it in any directory of your choice.

2. That's it — no dependencies, no composer, no setup.

---

## Usage

Run the script from your terminal:

```bash
php bank.php
```

You'll be greeted with the main menu:

```
*WELCOME TO BANKING SYSTEM*

Select one option below:
1. Open an Account
2. Balance Enquiry
3. Deposit
4. Withdrawal
5. Close an Account
6. Show All Accounts
7. Quit
```

Enter the number of the action you want and follow the prompts.

---

## Features

| Option | Description |
|--------|-------------|
| **1. Open an Account** | Create a new account with a first name, last name, and opening balance. An account number is assigned automatically. |
| **2. Balance Enquiry** | Look up an account by its number and display its current details. |
| **3. Deposit** | Add funds to an existing account. |
| **4. Withdrawal** | Remove funds from an existing account. Insufficient-funds are rejected. |
| **5. Close an Account** | Permanently delete an account from the system. |
| **6. Show All Accounts** | Display every account currently in the system. |
| **7. Quit** | Save all data and exit. |

---

## Data Persistence

Account data is saved to **`bank_ledger.txt`** in the same directory as `bank.php`. The file is created automatically on first use.

- Every new account is **appended** to the file immediately after creation.
- Deposits, withdrawals, and closures **rewrite** the file in full to keep it consistent.
- On quit (option 7), the file is written one final time before the program exits.

**Do not edit `bank_ledger.txt` manually** unless you follow the exact format below (one value per line, four lines per account):

```
<account_number>
<first_name>
<last_name>
<balance>
```

Example:

```
1
John
Doe
5000
2
Jane
Smith
12000
```

---

## Example Session

```
*WELCOME TO BANKING SYSTEM*

Select one option below: > 1

*OPEN AN ACCOUNT*
First Name: John
Last Name:  Doe
Account Amount: 5000

Account Number : 1
First Name     : John
Last Name      : Doe
Account Amount : 5000

> 3

*DEPOSIT*
Enter Account Number: 1

Account Number : 1
First Name     : John
Last Name      : Doe
Account Amount : 5000

Enter Deposit Amount: 2000
Total Amount 2000 has been deposited into Account Number 1

Account Number : 1
First Name     : John
Last Name      : Doe
Account Amount : 7000

> 7
We hope to see you soon! Bye!
```

---

## Project Structure

```
.
├── bank.php          # Main application (all logic in one file)
└── bank_ledger.txt   # Auto-generated data file (do not delete while running)
```

---

## Original Project

This is a PHP port of the C++ banking system at:
**https://github.com/m8bsd/bank**

---

## License

MIT
