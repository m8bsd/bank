<?php

/**
 * Simple Banking System
 * PHP conversion of https://github.com/m8bsd/bank (C++ original)
 *
 * Run via CLI:  php bank.php
 * Data is persisted in bank_ledger.txt (same directory as this script).
 */

define('LEDGER_FILE', __DIR__ . '/bank_ledger.txt');

// ---------------------------------------------------------------------------
// Account class
// ---------------------------------------------------------------------------
class Account
{
    private static int $cumulativeAcctNum = 0;

    private int    $acctNum;
    private string $firstName;
    private string $lastName;
    private int    $acctAmt;

    /** @var Account[] */
    public static array $vList = [];

    // -----------------------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------------------
    public function __construct(string $firstName, string $lastName, int $acctAmt, int $acctNum = -1)
    {
        if ($acctNum === -1) {
            self::$cumulativeAcctNum++;
            $this->acctNum = self::$cumulativeAcctNum;
        } else {
            $this->acctNum = $acctNum;
        }
        $this->firstName = $firstName;
        $this->lastName  = $lastName;
        $this->acctAmt   = $acctAmt;
    }

    // -----------------------------------------------------------------------
    // Getters / Setters
    // -----------------------------------------------------------------------
    public function getAccountNumber(): int    { return $this->acctNum;    }
    public function getFirstName(): string     { return $this->firstName;  }
    public function getLastName(): string      { return $this->lastName;   }
    public function getAccountAmount(): int    { return $this->acctAmt;    }

    public function setFirstName(string $fn): void  { $this->firstName = $fn; }
    public function setLastName(string $ln): void   { $this->lastName  = $ln; }
    public function setAccountAmount(int $amt): void { $this->acctAmt  = $amt; }

    // -----------------------------------------------------------------------
    // Static helpers
    // -----------------------------------------------------------------------

    /** Load all accounts from the ledger file into $vList. */
    public static function loadAll(): void
    {
        self::$vList = [];
        if (!file_exists(LEDGER_FILE)) {
            return;
        }

        $lines = array_values(array_filter(
            array_map('trim', file(LEDGER_FILE)),
            fn($l) => $l !== ''
        ));

        // Each account is stored as 4 consecutive lines: acctNum, firstName, lastName, acctAmt
        for ($i = 0; $i + 3 < count($lines); $i += 4) {
            $acctNum   = (int)$lines[$i];
            $firstName = $lines[$i + 1];
            $lastName  = $lines[$i + 2];
            $acctAmt   = (int)$lines[$i + 3];

            self::$vList[] = new Account($firstName, $lastName, $acctAmt, $acctNum);
        }

        // Sync the cumulative counter to the highest existing account number
        if (!empty(self::$vList)) {
            self::$cumulativeAcctNum = max(
                array_map(fn($a) => $a->getAccountNumber(), self::$vList)
            );
        }
    }

    /** Overwrite the ledger file with the current in-memory list. */
    public static function ledgerDump(): void
    {
        $fh = fopen(LEDGER_FILE, 'w');
        foreach (self::$vList as $account) {
            fwrite($fh, $account->getAccountNumber() . "\n");
            fwrite($fh, $account->getFirstName()     . "\n");
            fwrite($fh, $account->getLastName()      . "\n");
            fwrite($fh, $account->getAccountAmount() . "\n");
        }
        fclose($fh);
    }

    /** Search by account number; returns the Account or null. */
    public static function searchByAccountNumber(int $acctNum): ?Account
    {
        foreach (self::$vList as $account) {
            if ($account->getAccountNumber() === $acctNum) {
                return $account;
            }
        }
        return null;
    }

    /** Pretty-print a single account. */
    public function display(): void
    {
        echo "\nAccount Number : " . $this->acctNum   . "\n";
        echo "First Name     : " . $this->firstName  . "\n";
        echo "Last Name      : " . $this->lastName   . "\n";
        echo "Account Amount : " . $this->acctAmt    . "\n";
    }

    // -----------------------------------------------------------------------
    // Menu actions
    // -----------------------------------------------------------------------

    public static function open(): void
    {
        echo "\n*OPEN AN ACCOUNT*\n";
        $firstName = trim(readline("First Name: "));
        $lastName  = trim(readline("Last Name: "));
        $acctAmt   = (int)trim(readline("Account Amount: "));

        $account = new Account($firstName, $lastName, $acctAmt);
        self::$vList[] = $account;
        $account->display();

        // Append to ledger
        $fh = fopen(LEDGER_FILE, 'a');
        fwrite($fh, $account->getAccountNumber() . "\n");
        fwrite($fh, $account->getFirstName()     . "\n");
        fwrite($fh, $account->getLastName()      . "\n");
        fwrite($fh, $account->getAccountAmount() . "\n");
        fclose($fh);
    }

    public static function balance(): void
    {
        echo "\n*BALANCE ENQUIRY*\n";
        $acctNum = (int)trim(readline("Enter Account Number: "));
        $account = self::searchByAccountNumber($acctNum);

        if ($account !== null) {
            $account->display();
        } else {
            echo "Account Not Found.\n";
        }
    }

    public static function deposit(): void
    {
        echo "\n*DEPOSIT*\n";
        $acctNum = (int)trim(readline("Enter Account Number: "));
        $account = self::searchByAccountNumber($acctNum);

        if ($account !== null) {
            $account->display();
            $depositAmt = (int)trim(readline("\nEnter Deposit Amount: "));
            $account->setAccountAmount($account->getAccountAmount() + $depositAmt);
            echo "Total Amount $depositAmt has been deposited into Account Number $acctNum\n";
            self::ledgerDump();
            $account->display();
        } else {
            echo "Account Not Found.\n";
        }
    }

    public static function withdraw(): void
    {
        echo "\n*WITHDRAWAL*\n";
        $acctNum = (int)trim(readline("Enter Account Number: "));
        $account = self::searchByAccountNumber($acctNum);

        if ($account !== null) {
            $account->display();
            $withdrawAmt = (int)trim(readline("\nEnter Withdrawal Amount: "));

            if ($withdrawAmt > $account->getAccountAmount()) {
                echo "Insufficient funds.\n";
                return;
            }

            $account->setAccountAmount($account->getAccountAmount() - $withdrawAmt);
            echo "Total Amount $withdrawAmt has been withdrawn from Account Number $acctNum\n";
            self::ledgerDump();
            $account->display();
        } else {
            echo "Account Not Found.\n";
        }
    }

    public static function close(): void
    {
        echo "\n*CLOSE ACCOUNT*\n";
        $acctNum = (int)trim(readline("Enter Account Number: "));
        $account = self::searchByAccountNumber($acctNum);

        if ($account !== null) {
            $account->display();
            self::$vList = array_values(array_filter(
                self::$vList,
                fn($a) => $a->getAccountNumber() !== $acctNum
            ));
            self::ledgerDump();
            echo "Account Number $acctNum has been closed.\n";
        } else {
            echo "Account Not Found.\n";
        }
    }

    public static function showAll(): void
    {
        echo "\n*ALL ACCOUNTS*\n";
        if (empty(self::$vList)) {
            echo "No accounts found.\n";
            return;
        }
        foreach (self::$vList as $account) {
            $account->display();
        }
    }
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

Account::loadAll();

echo "\n*WELCOME TO BANKING SYSTEM*\n";

while (true) {
    echo "\nSelect one option below:\n";
    echo "1. Open an Account\n";
    echo "2. Balance Enquiry\n";
    echo "3. Deposit\n";
    echo "4. Withdrawal\n";
    echo "5. Close an Account\n";
    echo "6. Show All Accounts\n";
    echo "7. Quit\n";

    $option = (int)trim(readline("> "));

    switch ($option) {
        case 1: Account::open();    break;
        case 2: Account::balance(); break;
        case 3: Account::deposit(); break;
        case 4: Account::withdraw(); break;
        case 5: Account::close();   break;
        case 6: Account::showAll(); break;
        case 7:
            Account::ledgerDump();
            echo "We hope to see you soon! Bye!\n";
            exit(0);
        default:
            echo "*Please enter a valid option (1~7)*\n";
    }
}
