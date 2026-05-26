<?php

/**
 * Simple Banking System
 * PHP port of https://github.com/m8bsd/bank (C++ original by m8bsd)
 *
 * Run via CLI: php bank.php
 */

define('LEDGER_FILE', __DIR__ . '/bank_ledger.txt');

// ---------------------------------------------------------------------------
// Account class
// ---------------------------------------------------------------------------
class Account
{
    private static int $cumulativeAcctNum = 0;
    private int   $acctNum;
    private string $firstName;
    private string $lastName;
    private int   $acctAmt;

    /** @var Account[] */
    public static array $vList = [];

    // ------------------------------------------------------------------
    // Constructors
    // ------------------------------------------------------------------

    public function __construct(string $firstName, string $lastName, int $acctAmt, ?int $acctNum = null)
    {
        $this->firstName = $firstName;
        $this->lastName  = $lastName;
        $this->acctAmt   = $acctAmt;

        if ($acctNum !== null) {
            // Loading from file – use stored number, don't increment counter
            $this->acctNum = $acctNum;
        } else {
            self::$cumulativeAcctNum++;
            $this->acctNum = self::$cumulativeAcctNum;
        }
    }

    // ------------------------------------------------------------------
    // Accessors
    // ------------------------------------------------------------------

    public function getAccountNumber(): int   { return $this->acctNum;    }
    public function getFirstName(): string     { return $this->firstName;  }
    public function getLastName(): string      { return $this->lastName;   }
    public function getAccountAmount(): int    { return $this->acctAmt;    }

    // ------------------------------------------------------------------
    // Mutators
    // ------------------------------------------------------------------

    public function setFirstName(string $fn): void  { $this->firstName = $fn;  }
    public function setLastName(string $ln): void   { $this->lastName  = $ln;  }
    public function setAccountAmount(int $amt): void { $this->acctAmt  = $amt; }

    // ------------------------------------------------------------------
    // toString (equivalent to operator<<)
    // ------------------------------------------------------------------

    public function __toString(): string
    {
        return "\nAccount Number: {$this->acctNum}\n"
             . "First Name: {$this->firstName}\n"
             . "Last Name: {$this->lastName}\n"
             . "Account Amount: {$this->acctAmt}";
    }

    /** Format used when writing to the ledger file */
    public function toLedgerString(): string
    {
        return "\n{$this->acctNum}\n{$this->firstName}\n{$this->lastName}\n{$this->acctAmt}";
    }

    // ------------------------------------------------------------------
    // Static helpers
    // ------------------------------------------------------------------

    /** Load all accounts from the ledger file into $vList. */
    public static function getAll(): void
    {
        self::$vList = [];

        if (!file_exists(LEDGER_FILE)) {
            return;
        }

        $lines = file(LEDGER_FILE, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
        if ($lines === false || count($lines) === 0) {
            return;
        }

        // Each account occupies 4 consecutive lines: acctNum, firstName, lastName, acctAmt
        for ($i = 0; $i + 3 < count($lines); $i += 4) {
            $acctNum   = (int)$lines[$i];
            $firstName = $lines[$i + 1];
            $lastName  = $lines[$i + 2];
            $acctAmt   = (int)$lines[$i + 3];

            self::$vList[] = new Account($firstName, $lastName, $acctAmt, $acctNum);

            // Keep the cumulative counter in sync with the highest loaded number
            if ($acctNum > self::$cumulativeAcctNum) {
                self::$cumulativeAcctNum = $acctNum;
            }
        }
    }

    /** Find an account by account number; returns null if not found. */
    public static function searchByAccountNumber(int $acctNum): ?Account
    {
        foreach (self::$vList as $account) {
            if ($account->getAccountNumber() === $acctNum) {
                return $account;
            }
        }
        return null;
    }

    /** Rewrite the entire ledger file from the in-memory list. */
    public static function ledgerDump(): void
    {
        $fp = fopen(LEDGER_FILE, 'w');
        if ($fp === false) {
            echo "Error: could not open ledger file for writing.\n";
            return;
        }
        foreach (self::$vList as $account) {
            fwrite($fp, $account->toLedgerString());
        }
        fclose($fp);
    }

    // ------------------------------------------------------------------
    // Menu actions  (mirrors the static methods in the C++ original)
    // ------------------------------------------------------------------

    public static function open(): void
    {
        echo "\n*OPEN AN ACCOUNT*\n";
        $firstName = self::prompt("First Name: ");
        $lastName  = self::prompt("Last Name: ");
        $acctAmt   = (int)self::prompt("Account Amount: ");

        $account = new Account($firstName, $lastName, $acctAmt);
        self::$vList[] = $account;

        echo $account . "\n";

        // Append to ledger file
        $fp = fopen(LEDGER_FILE, 'a');
        if ($fp !== false) {
            fwrite($fp, $account->toLedgerString());
            fclose($fp);
        }
    }

    public static function balance(): void
    {
        echo "\n*BALANCE ENQUIRY*\n";
        $acctNum = (int)self::prompt("Enter Account Number: ");
        $account = self::searchByAccountNumber($acctNum);

        if ($account !== null) {
            echo $account . "\n";
        } else {
            echo "Account Not Found.\n";
        }
    }

    public static function deposit(): void
    {
        echo "\n*DEPOSIT*\n";
        $acctNum = (int)self::prompt("Enter Account Number: ");
        $account = self::searchByAccountNumber($acctNum);

        if ($account !== null) {
            echo $account . "\n";
            $depositAmt = (int)self::prompt("\nEnter Deposit Amount: ");
            $account->setAccountAmount($account->getAccountAmount() + $depositAmt);
            echo "Total Amount {$depositAmt} has been deposited into Account Number {$acctNum}\n";
            self::ledgerDump();
            echo $account . "\n";
        } else {
            echo "Account Not Found.\n";
        }
    }

    public static function withdraw(): void
    {
        echo "\n*WITHDRAWAL*\n";
        $acctNum = (int)self::prompt("Enter Account Number: ");
        $account = self::searchByAccountNumber($acctNum);

        if ($account !== null) {
            echo $account . "\n";
            $withdrawAmt = (int)self::prompt("\nEnter Withdrawal Amount: ");
            $account->setAccountAmount($account->getAccountAmount() - $withdrawAmt);
            echo "Total Amount {$withdrawAmt} has been withdrawn from Account Number {$acctNum}\n";
            self::ledgerDump();
            echo $account . "\n";
        } else {
            echo "Account Not Found.\n";
        }
    }

    public static function close(): void
    {
        echo "\n*CLOSE ACCOUNT*\n";
        $acctNum = (int)self::prompt("Enter Account Number: ");
        $account = self::searchByAccountNumber($acctNum);

        if ($account !== null) {
            echo $account . "\n";
            self::$vList = array_values(array_filter(
                self::$vList,
                fn(Account $a) => $a->getAccountNumber() !== $acctNum
            ));
            self::ledgerDump();
            echo "Account Number {$acctNum} has been closed.\n";
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
            echo $account . "\n";
        }
    }

    // ------------------------------------------------------------------
    // Internal helpers
    // ------------------------------------------------------------------

    /** Read a line from STDIN, trimmed. */
    private static function prompt(string $label): string
    {
        echo $label;
        return trim((string)fgets(STDIN));
    }
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

// Load existing accounts from the ledger file
Account::getAll();

echo "\n*WELCOME TO BANKING SYSTEM*\n";

$option = 0;
while ($option !== 7) {
    echo "\nSelect one option below: "
       . "\n1. Open an Account"
       . "\n2. Balance Enquiry"
       . "\n3. Deposit"
       . "\n4. Withdrawal"
       . "\n5. Close an Account"
       . "\n6. Show All Accounts"
       . "\n7. Quit\n";

    $input  = trim((string)fgets(STDIN));
    $option = (int)$input;

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
            break;
        default:
            echo "*Please enter a valid option (1~7)*\n";
            break;
    }
}
