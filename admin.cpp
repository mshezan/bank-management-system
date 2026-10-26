
#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include "bank.h"

using namespace std;

static const string connectionString =
    "host=127.0.0.1 port=5432 dbname=oopsproject user=shezanfaizal";

admin::admin(string adminId) {
    // The supplied ID is used when login() is called.
    (void)adminId;
}

admin::~admin() = default;


// Authenticate the administrator.
bool admin::login(string adminId, string password) {
    try {
        pqxx::connection c(connectionString);
        pqxx::read_transaction tx(c);

        auto result = tx.exec_params(
            "SELECT admin_id FROM admins "
            "WHERE username = $1 "
            "AND password_hash = crypt($2, password_hash)",
            adminId, password
        );

        if (result.empty()) {
            cout << "Invalid administrator credentials.\n";
            return false;
        }

        cout << "Admin login successful!\n";
        return true;
    }
    catch (const exception& e) {
        cerr << "Admin login failed: " << e.what() << '\n';
        return false;
    }
}


// Search for a customer using their account number.
bool admin::searchUser(string accountNumber) {
    try {
        pqxx::connection c(connectionString);
        pqxx::read_transaction tx(c);

        auto result = tx.exec_params(
            "SELECT c.customer_id, c.name, c.age, c.phone, "
            "c.address, a.account_number, a.balance, a.status "
            "FROM customers AS c "
            "JOIN accounts AS a "
            "ON c.customer_id = a.customer_id "
            "WHERE a.account_number = $1",
            accountNumber
        );

        if (result.empty()) {
            cout << "User not found.\n";
            return false;
        }

        cout << "\n--- User Details ---\n";
        cout << "Customer ID: "
             << result[0]["customer_id"].as<string>() << '\n';
        cout << "Name: "
             << result[0]["name"].as<string>() << '\n';
        cout << "Age: "
             << result[0]["age"].as<string>() << '\n';
        cout << "Phone: "
             << result[0]["phone"].as<string>() << '\n';
        cout << "Address: "
             << result[0]["address"].as<string>() << '\n';
        cout << "Account Number: "
             << result[0]["account_number"].as<string>() << '\n';
        cout << "Balance: "
             << result[0]["balance"].as<string>() << '\n';
        cout << "Status: "
             << result[0]["status"].as<string>() << '\n';

        return true;
    }
    catch (const exception& e) {
        cerr << "Search failed: " << e.what() << '\n';
        return false;
    }
}


// Display all customer accounts.
void admin::showRecords() {
    try {
        pqxx::connection c(connectionString);
        pqxx::read_transaction tx(c);

        auto result = tx.exec(
            "SELECT c.customer_id, c.name, a.account_number, "
            "a.balance, a.status "
            "FROM customers AS c "
            "JOIN accounts AS a "
            "ON c.customer_id = a.customer_id "
            "ORDER BY c.customer_id"
        );

        cout << "\n--- All Bank Records ---\n";

        if (result.empty()) {
            cout << "No records found.\n";
            return;
        }

        for (const auto& row : result) {
            cout << "Customer ID: "
                 << row["customer_id"].as<string>()
                 << " | Name: "
                 << row["name"].as<string>()
                 << " | Account: "
                 << row["account_number"].as<string>()
                 << " | Balance: "
                 << row["balance"].as<string>()
                 << " | Status: "
                 << row["status"].as<string>()
                 << '\n';
        }
    }
    catch (const exception& e) {
        cerr << "Could not retrieve records: "
             << e.what() << '\n';
    }
}


// Edit customer details using the account number.
bool admin::editUser(int accountNumber) {
    string name, address;
    int age, phone;

    cout << "Enter new name: ";
    getline(cin >> ws, name);

    cout << "Enter new age: ";
    if (!(cin >> age)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return false;
    }

    cout << "Enter new phone number: ";
    if (!(cin >> phone)) {
        cin.clear();
        cin.ignore(10000, '\n');
        return false;
    }

    cout << "Enter new address: ";
    getline(cin >> ws, address);

    if (name.empty() || age < 18 || phone <= 0) {
        cout << "Invalid details.\n";
        return false;
    }

    try {
        pqxx::connection c(connectionString);
        pqxx::work tx(c);

        auto result = tx.exec_params(
            "UPDATE customers AS c "
            "SET name = $1, age = $2, phone = $3, address = $4 "
            "FROM accounts AS a "
            "WHERE c.customer_id = a.customer_id "
            "AND a.account_number = $5::text "
            "RETURNING c.customer_id",
            name, age, to_string(phone), address, accountNumber
        );

        if (result.empty()) {
            cout << "Account not found.\n";
            return false;
        }

        tx.commit();

        cout << "User details updated successfully.\n";
        return true;
    }
    catch (const exception& e) {
        cerr << "Edit failed: " << e.what() << '\n';
        return false;
    }
}


// Close an account while preserving its transaction history.
bool admin::deleteUser(int accountNumber) {
    try {
        pqxx::connection c(connectionString);
        pqxx::work tx(c);

        auto result = tx.exec_params(
            "UPDATE accounts "
            "SET status = 'closed' "
            "WHERE account_number = $1::text "
            "AND status = 'active' "
            "AND balance = 0 "
            "RETURNING account_number",
            accountNumber
        );

        if (result.empty()) {
            cout << "Account not found, already closed, "
                    "or balance is not zero.\n";
            return false;
        }

        tx.commit();

        cout << "Account closed successfully.\n";
        return true;
    }
    catch (const exception& e) {
        cerr << "Account closure failed: "
             << e.what() << '\n';
        return false;
    }
}
