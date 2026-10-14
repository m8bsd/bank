import { Bank } from "./account.js";
import { Scanner } from "./scanner.js";

const MENU =
  "\nSelect one option below: " +
  "\n1. Open an Account" +
  "\n2. Balance Enquiry" +
  "\n3. Deposit" +
  "\n4. Withdrawal" +
  "\n5. Close an Account" +
  "\n6. Show All Accounts" +
  "\n7. Quit\n";

async function main(): Promise<void> {
  const input = new Scanner(process.stdin);
  const bank = new Bank(input);

  bank.init();
  process.stdout.write("\n*WELCOME TO BANKING SYSTEM*\n");

  let option = 0;
  while (option !== 7) {
    process.stdout.write(MENU);

    const value = await input.readInteger();
    if (value !== null) {
      option = Number(value);
    } else if (input.eof) {
      option = 7; // EOF: save and quit
    } else {
      await input.flushLine();
      option = 0;
    }

    switch (option) {
      case 1:
        await bank.open();
        break;
      case 2:
        await bank.balance();
        break;
      case 3:
        await bank.deposit();
        break;
      case 4:
        await bank.withdraw();
        break;
      case 5:
        await bank.close();
        break;
      case 6:
        bank.showAll();
        break;
      case 7:
        bank.shutdown();
        process.stdout.write("We hope to see you soon! Bye!\n");
        break;
      default:
        process.stdout.write("*Please enter a valid option (1~7)*\n");
        break;
    }
  }

  input.close();
}

main().catch((err: unknown) => {
  console.error(err);
  process.exitCode = 1;
});
