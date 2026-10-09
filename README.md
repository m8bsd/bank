### A Simple Banking System Written in C
## 1. Install the tools (one time)
```
xcode-select --install     # Apple's C compiler (clang)
brew install cmake         # needs Homebrew: https://brew.sh
```
## 2. Clone and build
```
git clone https://github.com/m8bsd/bank.git
cd bank
mkdir build && cd build
cmake ..
make
```
## 3. Run
```
./c_banking_system
```
## Without CMake
Since there are only a few files, you can compile directly:
```
git clone https://github.com/m8bsd/bank.git
cd bank
cc -std=c99 -Wall -Wextra main.c account.c -o bank
./bank
```
Accounts are stored in `bank_ledger.txt` in the current directory.
