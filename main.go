package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// ─── Data model ────────────────────────────────────────────────────────────────

type Account struct {
	Number    int64
	FirstName string
	LastName  string
	Amount    int64
}

// ─── Ledger file helpers ────────────────────────────────────────────────────────

const ledgerFile = "bank_ledger.txt"

// loadAll reads every account from the ledger file.
// File format (one account per 4 lines, blank line separating groups):
//
//	<account_number>
//	<first_name>
//	<last_name>
//	<amount>
func loadAll() []*Account {
	f, err := os.Open(ledgerFile)
	if err != nil {
		return nil // file doesn't exist yet – that's fine
	}
	defer f.Close()

	var accounts []*Account
	scanner := bufio.NewScanner(f)

	for {
		// Skip blank / whitespace-only lines between records
		var numStr string
		for scanner.Scan() {
			line := strings.TrimSpace(scanner.Text())
			if line != "" {
				numStr = line
				break
			}
		}
		if numStr == "" {
			break // EOF
		}

		readLine := func() string {
			for scanner.Scan() {
				line := strings.TrimSpace(scanner.Text())
				if line != "" {
					return line
				}
			}
			return ""
		}

		num, _ := strconv.ParseInt(numStr, 10, 64)
		first := readLine()
		last := readLine()
		amtStr := readLine()
		amt, _ := strconv.ParseInt(amtStr, 10, 64)

		accounts = append(accounts, &Account{
			Number:    num,
			FirstName: first,
			LastName:  last,
			Amount:    amt,
		})
	}
	return accounts
}

// saveLedger rewrites the entire ledger file from the in-memory slice.
func saveLedger(accounts []*Account) {
	f, err := os.Create(ledgerFile)
	if err != nil {
		fmt.Println("Error writing ledger:", err)
		return
	}
	defer f.Close()

	w := bufio.NewWriter(f)
	for _, a := range accounts {
		fmt.Fprintf(w, "\n%d\n%s\n%s\n%d", a.Number, a.FirstName, a.LastName, a.Amount)
	}
	w.Flush()
}

// appendAccount appends a single account to the ledger file.
func appendAccount(a *Account) {
	f, err := os.OpenFile(ledgerFile, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		fmt.Println("Error opening ledger:", err)
		return
	}
	defer f.Close()
	fmt.Fprintf(f, "\n%d\n%s\n%s\n%d", a.Number, a.FirstName, a.LastName, a.Amount)
}

// ─── Display ────────────────────────────────────────────────────────────────────

func printAccount(a *Account) {
	fmt.Printf("\nAccount Number : %d\n", a.Number)
	fmt.Printf("First Name     : %s\n", a.FirstName)
	fmt.Printf("Last Name      : %s\n", a.LastName)
	fmt.Printf("Account Amount : %d\n", a.Amount)
}

// ─── Search ─────────────────────────────────────────────────────────────────────

func findByNumber(accounts []*Account, num int64) *Account {
	for _, a := range accounts {
		if a.Number == num {
			return a
		}
	}
	return nil
}

// ─── Input helpers ───────────────────────────────────────────────────────────────

var reader = bufio.NewReader(os.Stdin)

func readString(prompt string) string {
	fmt.Print(prompt)
	text, _ := reader.ReadString('\n')
	return strings.TrimSpace(text)
}

func readInt64(prompt string) int64 {
	for {
		s := readString(prompt)
		n, err := strconv.ParseInt(s, 10, 64)
		if err == nil {
			return n
		}
		fmt.Println("Please enter a valid number.")
	}
}

// ─── Operations ─────────────────────────────────────────────────────────────────

func openAccount(accounts *[]*Account, nextNum *int64) {
	fmt.Println("\n*OPEN AN ACCOUNT*")
	first := readString("First Name: ")
	last := readString("Last Name: ")
	amt := readInt64("Account Amount: ")

	*nextNum++
	a := &Account{
		Number:    *nextNum,
		FirstName: first,
		LastName:  last,
		Amount:    amt,
	}
	*accounts = append(*accounts, a)
	printAccount(a)
	appendAccount(a)
}

func balanceEnquiry(accounts []*Account) {
	fmt.Println("\n*BALANCE ENQUIRY*")
	num := readInt64("Enter Account Number: ")
	a := findByNumber(accounts, num)
	if a != nil {
		printAccount(a)
	} else {
		fmt.Println("Account Not Found.")
	}
}

func deposit(accounts []*Account) {
	fmt.Println("\n*DEPOSIT*")
	num := readInt64("Enter Account Number: ")
	a := findByNumber(accounts, num)
	if a == nil {
		fmt.Println("Account Not Found.")
		return
	}
	printAccount(a)
	amt := readInt64("\nEnter Deposit Amount: ")
	a.Amount += amt
	fmt.Printf("Total Amount %d has been deposited into Account Number %d\n", amt, num)
	saveLedger(accounts)
	printAccount(a)
}

func withdraw(accounts []*Account) {
	fmt.Println("\n*WITHDRAWAL*")
	num := readInt64("Enter Account Number: ")
	a := findByNumber(accounts, num)
	if a == nil {
		fmt.Println("Account Not Found.")
		return
	}
	printAccount(a)
	amt := readInt64("\nEnter Withdrawal Amount: ")
	a.Amount -= amt
	fmt.Printf("Total Amount %d has been withdrawn from Account Number %d\n", amt, num)
	saveLedger(accounts)
	printAccount(a)
}

func closeAccount(accounts *[]*Account) {
	fmt.Println("\n*CLOSE ACCOUNT*")
	num := readInt64("Enter Account Number: ")
	list := *accounts
	idx := -1
	for i, a := range list {
		if a.Number == num {
			printAccount(a)
			idx = i
			break
		}
	}
	if idx == -1 {
		fmt.Println("Account Not Found.")
		return
	}
	*accounts = append(list[:idx], list[idx+1:]...)
	saveLedger(*accounts)
	fmt.Printf("Account Number %d has been closed.\n", num)
}

func showAll(accounts []*Account) {
	fmt.Println("\n*ALL ACCOUNTS*")
	for _, a := range accounts {
		printAccount(a)
	}
}

// ─── Main ────────────────────────────────────────────────────────────────────────

func main() {
	accounts := loadAll()

	// Determine the highest existing account number
	var nextNum int64
	for _, a := range accounts {
		if a.Number > nextNum {
			nextNum = a.Number
		}
	}

	fmt.Println("\n*WELCOME TO BANKING SYSTEM*")

	for {
		fmt.Println(`
Select one option below:
1. Open an Account
2. Balance Enquiry
3. Deposit
4. Withdrawal
5. Close an Account
6. Show All Accounts
7. Quit`)

		choice := readInt64("")

		switch choice {
		case 1:
			openAccount(&accounts, &nextNum)
		case 2:
			balanceEnquiry(accounts)
		case 3:
			deposit(accounts)
		case 4:
			withdraw(accounts)
		case 5:
			closeAccount(&accounts)
		case 6:
			showAll(accounts)
		case 7:
			saveLedger(accounts)
			fmt.Println("We hope to see you soon! Bye!")
			return
		default:
			fmt.Println("*Please enter a valid option (1~7)*")
		}
	}
}
