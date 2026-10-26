#include <string>
using namespace std;
class bank{
    private:
        int accountNumber;

    public:
        bank(int accountNumber);
        int findAccount(int accountNumber);
        bool login(int accountNumber, string password);
        bool deposit(int accountNumber,int amount);
        bool withdraw(int accountNumber,int amount);
        bool transfer(int senderAccountNumber,int recieverAccountNumber,int amount);
        int getBalance(int accountNumber);
        ~bank();
};

class customer {
private:
    int accountNumber;

public:
    customer(int accountNumber);

    bool registerCustomer(
        string name, int age, int phone,
        string address, string password
    );

    bool editCustomer(
        string name, int age, int phone,
        string address, string password
    );

    bool registerlogin(int accountNumber, string password);

    int getAccountDetails(int accountNumber);

    int findAccount(int accountNumber);

    ~customer();
};

class admin{
    public:
        admin(string adminId);
        bool login(string adminId,string password);
        bool searchUser(string accountNumber);
        void showRecords();
        bool editUser(int accountNumber);
        bool deleteUser(int accountNumber);
        ~admin();
};
