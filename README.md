### A Simple Banking System Written in TypeScript

A command-line banking system (open accounts, check balances, deposit, withdraw,
close accounts, list all accounts). Originally written in C, now ported to
TypeScript with identical behaviour and the same ledger file format.

## Requirements

Node.js 18 or newer.

## Build and run

```
git clone https://github.com/m8bsd/bank.git
cd bank
npm install
npm run build
npm start
```

`npm run typecheck` type-checks without emitting any files.

## Project layout

| File | Purpose |
| --- | --- |
| `src/main.ts` | Menu loop |
| `src/account.ts` | `Bank` class: account operations and ledger persistence |
| `src/scanner.ts` | Small `scanf`-style reader for stdin |

## Notes

- Accounts are stored in `bank_ledger.txt` in the current directory. The format
  is unchanged from the C version, so existing ledgers keep working.
- Names are a single word (up to 63 characters), as before.
- Account numbers and amounts are 64-bit integers (`bigint`), matching C's `long`.
