
#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include "bank.h"

using namespace std;
static const string connectionString =
    "host=127.0.0.1 port=5432 dbname=oopsproject user=shezanfaizal";

bank::bank(int accountNumber) : accountNumber(accountNumber) {
    pqxx::connection c(connectionString);

    if (c.is_open()) {
        cout << "Connected to database successfully: "
             << c.dbname() << endl;
    }
}

bank::~bank() = default;

int bank::findAccount(int accountNumber) {
    pqxx::connection c(connectionString);
    pqxx::read_transaction tx(c);

    auto result = tx.exec_params(
        "SELECT account_number FROM accounts "
        "WHERE account_number = $1::text",
        accountNumber
    );

    if (result.empty())
        return 0;

    return result[0][0].as<int>();
}


// Login using an account number and password hash.
// The password argument must already be securely hashed.
bool bank::login(int accountNumber, string password) {
    pqxx::connection c(connectionString);
    pqxx::read_transaction tx(c);

    auto result = tx.exec_params(
        "SELECT c.customer_id "
        "FROM customers c "
        "JOIN accounts a ON c.customer_id = a.customer_id "
        "WHERE a.account_number = $1::text "
        "AND c.password_hash = $2 "
        "AND a.status = 'active'",
        accountNumber, password
    );

    return !result.empty();
}


// Deposit money into an account.
bool bank::deposit(int accountNumber, int amount) {
    if (amount <= 0)
        return false;

    pqxx::connection c(connectionString);
    pqxx::work tx(c);

    auto result = tx.exec_params(
        "UPDATE accounts "
        "SET balance = balance + $1 "
        "WHERE account_number = $2::text "
        "AND status = 'active' "
        "RETURNING account_number",
        amount, accountNumber
    );

    if (result.empty())
        return false;

    tx.exec_params(
        "INSERT INTO transactions "
        "(transaction_type, amount, to_account) "
        "VALUES ('deposit', $1, $2::text)",
        amount, accountNumber
    );

    tx.commit();
    return true;
}


// Withdraw money from an account.
bool bank::withdraw(int accountNumber, int amount) {
    if (amount <= 0)
        return false;

    pqxx::connection c(connectionString);
    pqxx::work tx(c);

    auto result = tx.exec_params(
        "UPDATE accounts "
        "SET balance = balance - $1 "
        "WHERE account_number = $2::text "
        "AND status = 'active' "
        "AND balance >= $1 "
        "RETURNING account_number",
        amount, accountNumber
    );

    if (result.empty())
        return false;

    tx.exec_params(
        "INSERT INTO transactions "
        "(transaction_type, amount, from_account) "
        "VALUES ('withdrawal', $1, $2::text)",
        amount, accountNumber
    );

    tx.commit();
    return true;
}


// Transfer money between two accounts.
bool bank::transfer(
    int senderAccountNumber,
    int recieverAccountNumber,
    int amount
) {
    if (amount <= 0 ||
        senderAccountNumber == recieverAccountNumber) {
        return false;
    }

    pqxx::connection c(connectionString);
    pqxx::work tx(c);

    // Debit the sender only if sufficient funds exist.
    auto sender = tx.exec_params(
        "UPDATE accounts "
        "SET balance = balance - $1 "
        "WHERE account_number = $2::text "
        "AND status = 'active' "
        "AND balance >= $1 "
        "RETURNING account_number",
        amount, senderAccountNumber
    );

    if (sender.empty())
        return false;

    // Credit the receiver.
    auto receiver = tx.exec_params(
        "UPDATE accounts "
        "SET balance = balance + $1 "
        "WHERE account_number = $2::text "
        "AND status = 'active' "
        "RETURNING account_number",
        amount, recieverAccountNumber
    );

    // If the receiver is invalid, returning without committing
    // causes the sender's debit to roll back.
    if (receiver.empty())
        return false;

    tx.exec_params(
        "INSERT INTO transactions "
        "(transaction_type, amount, from_account, to_account) "
        "VALUES ('transfer', $1, $2::text, $3::text)",
        amount, senderAccountNumber, recieverAccountNumber
    );

    tx.commit();
    return true;
}


// Return the current balance.
// Returns -1 if the account does not exist.
int bank::getBalance(int accountNumber) {
    pqxx::connection c(connectionString);
    pqxx::read_transaction tx(c);

    auto result = tx.exec_params(
        "SELECT balance FROM accounts "
        "WHERE account_number = $1::text",
        accountNumber
    );

    if (result.empty())
        return -1;

    return result[0][0].as<int>();
}
