import java.io.*;
import java.util.*;

public class Bank {

    private static final String LEDGER_FILE = "bank_ledger.txt";

    // ============================================================
    // Account Class
    // ============================================================
    static class Account {

        private static int cumulativeAcctNum = 0;

        private int acctNum;
        private String firstName;
        private String lastName;
        private int acctAmt;

        static List<Account> vList = new ArrayList<>();

        public Account(String firstName, String lastName, int acctAmt) {
            this.firstName = firstName;
            this.lastName = lastName;
            this.acctAmt = acctAmt;
            this.acctNum = ++cumulativeAcctNum;
        }

        public Account(String firstName, String lastName, int acctAmt, int acctNum) {
            this.firstName = firstName;
            this.lastName = lastName;
            this.acctAmt = acctAmt;
            this.acctNum = acctNum;

            if (acctNum > cumulativeAcctNum) {
                cumulativeAcctNum = acctNum;
            }
        }

        public int getAccountNumber() {
            return acctNum;
        }

        public int getAccountAmount() {
            return acctAmt;
        }

        public void setAccountAmount(int amt) {
            this.acctAmt = amt;
        }

        @Override
        public String toString() {
            return "\nAccount Number: " + acctNum +
                    "\nFirst Name: " + firstName +
                    "\nLast Name: " + lastName +
                    "\nAccount Amount: " + acctAmt;
        }

        public String toLedgerString() {
            return acctNum + "\n"
                    + firstName + "\n"
                    + lastName + "\n"
                    + acctAmt + "\n";
        }

        // ============================================================
        // Persistence
        // ============================================================

        public static void getAll() {
            vList.clear();

            File file = new File(LEDGER_FILE);

            if (!file.exists()) {
                return;
            }

            try (BufferedReader br = new BufferedReader(new FileReader(file))) {

                List<String> lines = new ArrayList<>();
                String line;

                while ((line = br.readLine()) != null) {
                    if (!line.trim().isEmpty()) {
                        lines.add(line.trim());
                    }
                }

                for (int i = 0; i + 3 < lines.size(); i += 4) {

                    int acctNum = Integer.parseInt(lines.get(i));
                    String firstName = lines.get(i + 1);
                    String lastName = lines.get(i + 2);
                    int acctAmt = Integer.parseInt(lines.get(i + 3));

                    vList.add(new Account(
                            firstName,
                            lastName,
                            acctAmt,
                            acctNum
                    ));
                }

            } catch (Exception e) {
                System.out.println("Error loading ledger.");
            }
        }

        public static void ledgerDump() {

            try (BufferedWriter bw = new BufferedWriter(
                    new FileWriter(LEDGER_FILE))) {

                for (Account account : vList) {
                    bw.write(account.toLedgerString());
                }

            } catch (Exception e) {
                System.out.println("Error writing ledger.");
            }
        }

        public static Account searchByAccountNumber(int acctNum) {

            for (Account account : vList) {
                if (account.getAccountNumber() == acctNum) {
                    return account;
                }
            }

            return null;
        }

        // ============================================================
        // Menu Actions
        // ============================================================

        public static void open(Scanner sc) {

            System.out.println("\n*OPEN AN ACCOUNT*");

            System.out.print("First Name: ");
            String firstName = sc.nextLine();

            System.out.print("Last Name: ");
            String lastName = sc.nextLine();

            System.out.print("Account Amount: ");
            int amount = Integer.parseInt(sc.nextLine());

            Account account = new Account(
                    firstName,
                    lastName,
                    amount
            );

            vList.add(account);

            System.out.println(account);

            try (BufferedWriter bw = new BufferedWriter(
                    new FileWriter(LEDGER_FILE, true))) {

                bw.write(account.toLedgerString());

            } catch (Exception e) {
                System.out.println("Error writing account.");
            }
        }

        public static void balance(Scanner sc) {

            System.out.println("\n*BALANCE ENQUIRY*");

            System.out.print("Enter Account Number: ");
            int acctNum = Integer.parseInt(sc.nextLine());

            Account account = searchByAccountNumber(acctNum);

            if (account != null) {
                System.out.println(account);
            } else {
                System.out.println("Account Not Found.");
            }
        }

        public static void deposit(Scanner sc) {

            System.out.println("\n*DEPOSIT*");

            System.out.print("Enter Account Number: ");
            int acctNum = Integer.parseInt(sc.nextLine());

            Account account = searchByAccountNumber(acctNum);

            if (account == null) {
                System.out.println("Account Not Found.");
                return;
            }

            System.out.println(account);

            System.out.print("\nEnter Deposit Amount: ");
            int depositAmt = Integer.parseInt(sc.nextLine());

            account.setAccountAmount(
                    account.getAccountAmount() + depositAmt
            );

            ledgerDump();

            System.out.println(
                    "Total Amount " + depositAmt +
                    " has been deposited into Account Number " +
                    acctNum
            );

            System.out.println(account);
        }

        public static void withdraw(Scanner sc) {

            System.out.println("\n*WITHDRAWAL*");

            System.out.print("Enter Account Number: ");
            int acctNum = Integer.parseInt(sc.nextLine());

            Account account = searchByAccountNumber(acctNum);

            if (account == null) {
                System.out.println("Account Not Found.");
                return;
            }

            System.out.println(account);

            System.out.print("\nEnter Withdrawal Amount: ");
            int withdrawAmt = Integer.parseInt(sc.nextLine());

            account.setAccountAmount(
                    account.getAccountAmount() - withdrawAmt
            );

            ledgerDump();

            System.out.println(
                    "Total Amount " + withdrawAmt +
                    " has been withdrawn from Account Number " +
                    acctNum
            );

            System.out.println(account);
        }

        public static void close(Scanner sc) {

            System.out.println("\n*CLOSE ACCOUNT*");

            System.out.print("Enter Account Number: ");
            int acctNum = Integer.parseInt(sc.nextLine());

            Account account = searchByAccountNumber(acctNum);

            if (account == null) {
                System.out.println("Account Not Found.");
                return;
            }

            System.out.println(account);

            vList.removeIf(
                    a -> a.getAccountNumber() == acctNum
            );

            ledgerDump();

            System.out.println(
                    "Account Number " + acctNum +
                    " has been closed."
            );
        }

        public static void showAll() {

            System.out.println("\n*ALL ACCOUNTS*");

            if (vList.isEmpty()) {
                System.out.println("No accounts found.");
                return;
            }

            for (Account account : vList) {
                System.out.println(account);
            }
        }
    }

    // ============================================================
    // Main
    // ============================================================

    public static void main(String[] args) {

        Scanner sc = new Scanner(System.in);

        Account.getAll();

        System.out.println("\n*WELCOME TO BANKING SYSTEM*");

        int option = 0;

        while (option != 7) {

            System.out.println(
                    "\nSelect one option below:" +
                    "\n1. Open an Account" +
                    "\n2. Balance Enquiry" +
                    "\n3. Deposit" +
                    "\n4. Withdrawal" +
                    "\n5. Close an Account" +
                    "\n6. Show All Accounts" +
                    "\n7. Quit"
            );

            try {
                option = Integer.parseInt(sc.nextLine());
            } catch (Exception e) {
                option = -1;
            }

            switch (option) {

                case 1:
                    Account.open(sc);
                    break;

                case 2:
                    Account.balance(sc);
                    break;

                case 3:
                    Account.deposit(sc);
                    break;

                case 4:
                    Account.withdraw(sc);
                    break;

                case 5:
                    Account.close(sc);
                    break;

                case 6:
                    Account.showAll();
                    break;

                case 7:
                    Account.ledgerDump();
                    System.out.println(
                            "We hope to see you soon! Bye!"
                    );
                    break;

                default:
                    System.out.println(
                            "*Please enter a valid option (1~7)*"
                    );
            }
        }

        sc.close();
    }
}
