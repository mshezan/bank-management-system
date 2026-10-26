
#include <iostream>
#include <string>
#include "bank.h"

using namespace std;

int main() {
    bank bankSystem(0);

    int choice;

    do {
        cout << "\n===== BANK MANAGEMENT SYSTEM =====\n";
        cout << "1. Register New Customer\n";
        cout << "2. Customer Login\n";
        cout << "3. Admin Login\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input.\n";
            continue;
        }

        if (choice == 1) {
            string name, address, password;
            int age, phone;

            cout << "Enter name: ";
            getline(cin >> ws, name);

            cout << "Enter age: ";
            if (!(cin >> age)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid age.\n";
                continue;
            }

            cout << "Enter phone number: ";
            if (!(cin >> phone)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid phone number.\n";
                continue;
            }

            cout << "Enter address: ";
            getline(cin >> ws, address);

            cout << "Enter password: ";
            getline(cin, password);

            customer newCustomer(0);

            newCustomer.registerCustomer(
                name, age, phone, address, password
            );
        }

        else if (choice == 2) {
            int accountNumber;
            string password;

            cout << "Enter account number: ";
            if (!(cin >> accountNumber)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid account number.\n";
                continue;
            }

            cout << "Enter password: ";
            cin >> password;

            customer currentCustomer(accountNumber);

            if (!currentCustomer.registerlogin(
                    accountNumber, password)) {
                continue;
            }

            int userChoice;

            do {
                cout << "\n===== CUSTOMER MENU =====\n";
                cout << "1. Deposit\n";
                cout << "2. Withdraw\n";
                cout << "3. Transfer\n";
                cout << "4. Check Balance\n";
                cout << "5. View Account Details\n";
                cout << "6. Edit Personal Details\n";
                cout << "0. Logout\n";
                cout << "Enter choice: ";

                if (!(cin >> userChoice)) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input.\n";
                    continue;
                }

                if (userChoice == 1) {
                    int amount;

                    cout << "Enter deposit amount: ";
                    if (!(cin >> amount)) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid amount.\n";
                        continue;
                    }

                    if (bankSystem.deposit(accountNumber, amount))
                        cout << "Deposit successful.\n";
                    else
                        cout << "Deposit failed.\n";
                }

                else if (userChoice == 2) {
                    int amount;

                    cout << "Enter withdrawal amount: ";
                    if (!(cin >> amount)) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid amount.\n";
                        continue;
                    }

                    if (bankSystem.withdraw(accountNumber, amount))
                        cout << "Withdrawal successful.\n";
                    else
                        cout << "Withdrawal failed.\n";
                }

                else if (userChoice == 3) {
                    int receiver, amount;

                    cout << "Enter receiver account number: ";
                    if (!(cin >> receiver)) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid account number.\n";
                        continue;
                    }

                    cout << "Enter transfer amount: ";
                    if (!(cin >> amount)) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid amount.\n";
                        continue;
                    }

                    if (bankSystem.transfer(
                            accountNumber, receiver, amount))
                        cout << "Transfer successful.\n";
                    else
                        cout << "Transfer failed.\n";
                }

                else if (userChoice == 4) {
                    int balance =
                        bankSystem.getBalance(accountNumber);

                    if (balance >= 0)
                        cout << "Current balance: "
                             << balance << '\n';
                    else
                        cout << "Could not retrieve balance.\n";
                }

                else if (userChoice == 5) {
                    currentCustomer.getAccountDetails(accountNumber);
                }

                else if (userChoice == 6) {
                    string name, address, password;
                    int age, phone;

                    cout << "Enter updated name: ";
                    getline(cin >> ws, name);

                    cout << "Enter updated age: ";
                    if (!(cin >> age)) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid age.\n";
                        continue;
                    }

                    cout << "Enter updated phone: ";
                    if (!(cin >> phone)) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid phone number.\n";
                        continue;
                    }

                    cout << "Enter updated address: ";
                    getline(cin >> ws, address);

                    cout << "Enter new password "
                            "(leave blank to keep current): ";
                    getline(cin, password);

                    currentCustomer.editCustomer(
                        name, age, phone, address, password
                    );
                }

                else if (userChoice != 0) {
                    cout << "Invalid choice.\n";
                }

            } while (userChoice != 0);
        }

        else if (choice == 3) {
            string adminId, password;

            cout << "Enter admin username: ";
            cin >> adminId;

            cout << "Enter admin password: ";
            cin >> password;

            admin currentAdmin(adminId);

            if (!currentAdmin.login(adminId, password)) {
                continue;
            }

            int adminChoice;

            do {
                cout << "\n===== ADMIN MENU =====\n";
                cout << "1. Search User\n";
                cout << "2. Show All Records\n";
                cout << "3. Edit User\n";
                cout << "4. Close Account\n";
                cout << "0. Logout\n";
                cout << "Enter choice: ";

                if (!(cin >> adminChoice)) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input.\n";
                    continue;
                }

                if (adminChoice == 1) {
                    string accountNumber;

                    cout << "Enter account number: ";
                    cin >> accountNumber;

                    currentAdmin.searchUser(accountNumber);
                }

                else if (adminChoice == 2) {
                    currentAdmin.showRecords();
                }

                else if (adminChoice == 3) {
                    int accountNumber;

                    cout << "Enter account number to edit: ";
                    if (!(cin >> accountNumber)) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid account number.\n";
                        continue;
                    }

                    currentAdmin.editUser(accountNumber);
                }

                else if (adminChoice == 4) {
                    int accountNumber;

                    cout << "Enter account number to close: ";
                    if (!(cin >> accountNumber)) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        cout << "Invalid account number.\n";
                        continue;
                    }

                    currentAdmin.deleteUser(accountNumber);
                }

                else if (adminChoice != 0) {
                    cout << "Invalid choice.\n";
                }

            } while (adminChoice != 0);
        }

        else if (choice != 0) {
            cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    cout << "Thank you for using the Bank Management System.\n";

    return 0;
}
