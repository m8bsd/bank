import * as fs from "node:fs";

import { Scanner } from "./scanner.js";

export const NAME_MAX_LEN = 64;
export const LEDGER_FILE = "bank_ledger.txt";

/** Longest name we keep, matching the C version's "%63s" / char[64]. */
const NAME_MAX_CHARS = NAME_MAX_LEN - 1;

const INTEGER_RE = /^[+-]?\d+$/;

export interface Account {
  acctNum: bigint;
  firstName: string;
  lastName: string;
  acctAmt: bigint;
}

function truncateName(name: string): string {
  return name.slice(0, NAME_MAX_CHARS);
}

function formatRecord(a: Account): string {
  return `\n${a.acctNum}\n${a.firstName}\n${a.lastName}\n${a.acctAmt}`;
}

function reportFsError(path: string, err: unknown): void {
  const message = err instanceof Error ? err.message : String(err);
  console.error(`${path}: ${message}`);
}

function out(text: string): void {
  process.stdout.write(text);
}

export class Bank {
  private accounts: Account[] = [];
  private cumulativeAcctNum = 0n;

  constructor(
    private readonly input: Scanner,
    private readonly ledgerPath: string = LEDGER_FILE,
  ) {}

  /** Load the ledger file (if any) and initialise the account counter. */
  init(): void {
    let text = "";
    try {
      text = fs.readFileSync(this.ledgerPath, "utf8");
    } catch {
      // No ledger yet: start empty.
    }

    const tokens = text.split(/\s+/).filter((t) => t !== "");
    for (let i = 0; i + 3 < tokens.length; i += 4) {
      const [num, first, last, amt] = tokens.slice(i, i + 4);
      if (!INTEGER_RE.test(num) || !INTEGER_RE.test(amt)) break;
      this.accounts.push({
        acctNum: BigInt(num),
        firstName: truncateName(first),
        lastName: truncateName(last),
        acctAmt: BigInt(amt),
      });
    }

    const lastAccount = this.accounts[this.accounts.length - 1];
    this.cumulativeAcctNum = lastAccount ? lastAccount.acctNum : 0n;
  }

  /** Write the ledger and drop every account. */
  shutdown(): void {
    this.ledgerDump();
    this.accounts = [];
  }

  search(acctNum: bigint): Account | undefined {
    return this.accounts.find((a) => a.acctNum === acctNum);
  }

  print(a: Account): void {
    out(
      `\nAccount Number: ${a.acctNum}\nFirst Name: ${a.firstName}\n` +
        `Last Name: ${a.lastName}\nAccount Amount: ${a.acctAmt}`,
    );
  }

  /** Rewrite the whole ledger file from memory. */
  ledgerDump(): void {
    try {
      fs.writeFileSync(this.ledgerPath, this.accounts.map(formatRecord).join(""));
    } catch (err) {
      reportFsError(this.ledgerPath, err);
    }
  }

  async open(): Promise<void> {
    out("\n*OPEN AN ACCOUNT*\n");
    out("First Name: \n");
    const first = await this.input.readWord(NAME_MAX_CHARS);
    if (first === null) return;
    out("Last Name: \n");
    const last = await this.input.readWord(NAME_MAX_CHARS);
    if (last === null) return;
    out("Account Amount: \n");
    const amt = await this.readAmount();
    if (amt === null) {
      out("Invalid amount.\n");
      return;
    }

    const account: Account = {
      acctNum: this.cumulativeAcctNum + 1n,
      firstName: first,
      lastName: last,
      acctAmt: amt,
    };
    this.accounts.push(account);
    this.cumulativeAcctNum++;
    this.print(account);
    out("\n");

    try {
      fs.appendFileSync(this.ledgerPath, formatRecord(account));
    } catch (err) {
      reportFsError(this.ledgerPath, err);
    }
  }

  async balance(): Promise<void> {
    out("\n*BALANCE ENQUIRY*\n");
    out("Enter Account Number: \n");
    const num = await this.readAmount();
    const account = num === null ? undefined : this.search(num);
    if (!account) {
      out("Account Not Found. \n");
      return;
    }
    this.print(account);
    out("\n");
  }

  async deposit(): Promise<void> {
    out("\n*DEPOSIT*\n");
    out("Enter Account Number: \n");
    const num = await this.readAmount();
    const account = num === null ? undefined : this.search(num);
    if (num === null || !account) {
      out("Account Not Found. \n");
      return;
    }
    this.print(account);
    out("\n\nEnter Deposit Amount: \n");
    const amt = await this.readAmount();
    if (amt === null) {
      out("Invalid amount.\n");
      return;
    }
    account.acctAmt += amt;
    out(`Total Amount ${amt} has been deposited into Account Number ${num}\n`);
    this.ledgerDump();
    this.print(account);
    out("\n");
  }

  async withdraw(): Promise<void> {
    out("\n*WITHDRAWAL*\n");
    out("Enter Account Number: \n");
    const num = await this.readAmount();
    const account = num === null ? undefined : this.search(num);
    if (num === null || !account) {
      out("Account Not Found. \n");
      return;
    }
    this.print(account);
    out("\n\nEnter Withdrawal Amount: \n");
    const amt = await this.readAmount();
    if (amt === null) {
      out("Invalid amount.\n");
      return;
    }
    account.acctAmt -= amt;
    out(`Total Amount ${amt} has been withdrawn into Account Number ${num}\n`);
    this.ledgerDump();
    this.print(account);
    out("\n");
  }

  async close(): Promise<void> {
    out("\n *CLOSE ACCOUNT* \n");
    out("Enter Account Number: \n");
    const num = await this.readAmount();
    const index = num === null ? -1 : this.accounts.findIndex((a) => a.acctNum === num);
    if (num === null || index < 0) {
      out("Account Not Found. \n");
      return;
    }
    this.print(this.accounts[index]);
    out("\n");
    this.accounts.splice(index, 1);
    this.ledgerDump();
    out(`Account Number ${num} has been closed.\n`);
  }

  showAll(): void {
    out("\n*ALL ACCOUNTS*");
    for (const account of this.accounts) {
      this.print(account);
      out("\n");
    }
  }

  /** Read an integer; on bad input discard the rest of the line (unless at EOF). */
  private async readAmount(): Promise<bigint | null> {
    const value = await this.input.readInteger();
    if (value === null && !this.input.eof) await this.input.flushLine();
    return value;
  }
}
