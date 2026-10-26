
#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include "bank.h"

using namespace std;

static const string connectionString =
    "host=127.0.0.1 port=5432 dbname=oopsproject user=shezanfaizal";

customer::customer(int accountNumber)
    : accountNumber(accountNumber) {
}

customer::~customer() = default;


// Register a new customer and create their bank account.
bool customer::registerCustomer(
    string name,
    int age,
    int phone,
    string address,
    string password
) {
    if (name.empty() || age < 18 ||
        phone <= 0 || password.empty()) {
        cout << "Invalid customer details.\n";
        return false;
    }

    try {
        pqxx::connection c(connectionString);
        pqxx::work tx(c);

        // Create the customer and securely hash the password.
        auto result = tx.exec_params(
            "INSERT INTO customers "
            "(name, age, phone, address, password_hash) "
            "VALUES ($1, $2, $3, $4, crypt($5, gen_salt('bf'))) "
            "RETURNING customer_id",
            name, age, to_string(phone), address, password
        );

        long long customerId =
            result[0][0].as<long long>();

        // Use the generated customer ID as the account number.
        string newAccountNumber = to_string(customerId);

        tx.exec_params(
            "INSERT INTO accounts "
            "(account_number, customer_id, account_type, balance, status) "
            "VALUES ($1, $2, 'savings', 0.00, 'active')",
            newAccountNumber, customerId
        );

        // Commit both inserts together.
        tx.commit();

        accountNumber = static_cast<int>(customerId);

        cout << "Registration successful!\n";
        cout << "Your account number is: "
             << newAccountNumber << '\n';

        return true;
    }
    catch (const exception& e) {
        cerr << "Registration failed: "
             << e.what() << '\n';
        return false;
    }
}


// Edit the personal details of the current account holder.
bool customer::editCustomer(
    string name,
    int age,
    int phone,
    string address,
    string password
) {
    if (accountNumber <= 0 || name.empty() ||
        age < 18 || phone <= 0) {
        return false;
    }

    try {
        pqxx::connection c(connectionString);
        pqxx::work tx(c);

        pqxx::result result;

        if (password.empty()) {
            // An empty password means keep the existing password.
            result = tx.exec_params(
                "UPDATE customers AS c "
                "SET name = $1, age = $2, phone = $3, address = $4 "
                "FROM accounts AS a "
                "WHERE c.customer_id = a.customer_id "
                "AND a.account_number = $5::text "
                "AND a.status = 'active' "
                "RETURNING c.customer_id",
                name, age, to_string(phone), address, accountNumber
            );
        }
        else {
            // Hash a new password if one was provided.
            result = tx.exec_params(
                "UPDATE customers AS c "
                "SET name = $1, age = $2, phone = $3, address = $4, "
                "password_hash = crypt($5, gen_salt('bf')) "
                "FROM accounts AS a "
                "WHERE c.customer_id = a.customer_id "
                "AND a.account_number = $6::text "
                "AND a.status = 'active' "
                "RETURNING c.customer_id",
                name, age, to_string(phone), address,
                password, accountNumber
            );
        }

        if (result.empty()) {
            return false;
        }

        tx.commit();

        cout << "Customer details updated successfully.\n";
        return true;
    }
    catch (const exception& e) {
        cerr << "Update failed: " << e.what() << '\n';
        return false;
    }
}


// Authenticate a customer using their account number and password.
bool customer::registerlogin(
    int accountNumber,
    string password
) {
    try {
        pqxx::connection c(connectionString);
        pqxx::read_transaction tx(c);

        auto result = tx.exec_params(
            "SELECT c.customer_id "
            "FROM customers AS c "
            "JOIN accounts AS a "
            "ON c.customer_id = a.customer_id "
            "WHERE a.account_number = $1::text "
            "AND c.password_hash = crypt($2, c.password_hash) "
            "AND a.status = 'active'",
            accountNumber, password
        );

        if (result.empty()) {
            cout << "Invalid account number or password.\n";
            return false;
        }

        this->accountNumber = accountNumber;

        cout << "Login successful!\n";
        return true;
    }
    catch (const exception& e) {
        cerr << "Login failed: " << e.what() << '\n';
        return false;
    }
}


// Display account details and return the balance.
int customer::getAccountDetails(int accountNumber) {
    try {
        pqxx::connection c(connectionString);
        pqxx::read_transaction tx(c);

        auto result = tx.exec_params(
            "SELECT c.name, c.phone, a.account_number, "
            "a.account_type, a.balance, a.status "
            "FROM customers AS c "
            "JOIN accounts AS a "
            "ON c.customer_id = a.customer_id "
            "WHERE a.account_number = $1::text",
            accountNumber
        );

        if (result.empty()) {
            cout << "Account not found.\n";
            return -1;
        }

        cout << "\n--- Account Details ---\n";
        cout << "Name: " << result[0]["name"].as<string>() << '\n';
        cout << "Phone: " << result[0]["phone"].as<string>() << '\n';
        cout << "Account Number: "
             << result[0]["account_number"].as<string>() << '\n';
        cout << "Account Type: "
             << result[0]["account_type"].as<string>() << '\n';
        cout << "Balance: "
             << result[0]["balance"].as<string>() << '\n';
        cout << "Status: "
             << result[0]["status"].as<string>() << '\n';

        return result[0]["balance"].as<int>();
    }
    catch (const exception& e) {
        cerr << "Could not retrieve account details: "
             << e.what() << '\n';
        return -1;
    }
}


// Search for an account by account number.
int customer::findAccount(int accountNumber) {
    try {
        pqxx::connection c(connectionString);
        pqxx::read_transaction tx(c);

        auto result = tx.exec_params(
            "SELECT account_number FROM accounts "
            "WHERE account_number = $1::text",
            accountNumber
        );

        if (result.empty()) {
            return 0;
        }

        return result[0][0].as<int>();
    }
    catch (const exception& e) {
        cerr << "Account search failed: "
             << e.what() << '\n';
        return 0;
    }
}
