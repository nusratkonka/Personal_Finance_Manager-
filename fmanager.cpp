#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <iomanip>
#include <string>
#include <limits> // for numeric_limits
using namespace std;

const string DATA_FILE = "finance.data"; // File to store user and transaction data

class Transaction {
private:
    long transactionID;
    string type;
    string category;
    float amount;
    static long nextTransactionID;

public:
    Transaction() : transactionID(0), type(""), category(""), amount(0.0f) {}
    Transaction(string type, string category, float amount);
    long getTransactionID() const { return transactionID; }
    string getType() const { return type; }
    string getCategory() const { return category; }
    float getAmount() const { return amount; }
    static void setLastTransactionID(long id) { nextTransactionID = id; }
    static long getLastTransactionID() { return nextTransactionID; }
    friend ofstream &operator<<(ofstream &ofs, const Transaction &trans);
    friend ifstream &operator>>(ifstream &ifs, Transaction &trans);
    friend ostream &operator<<(ostream &os, const Transaction &trans);
};

long Transaction::nextTransactionID = 0;

Transaction::Transaction(string type, string category, float amount) {
    nextTransactionID++;
    this->transactionID = nextTransactionID;
    this->type = type;
    this->category = category;
    this->amount = amount;
}

ofstream &operator<<(ofstream &ofs, const Transaction &trans) {
    ofs << trans.transactionID << endl;
    ofs << trans.type << endl;
    ofs << trans.category << endl;
    ofs << trans.amount << endl;
    return ofs;
}

ifstream &operator>>(ifstream &ifs, Transaction &trans) {
    ifs >> trans.transactionID;
    ifs.ignore(); // Ignore newline after transactionID
    getline(ifs, trans.type); // Read type
    getline(ifs, trans.category); // Read category
    ifs >> trans.amount;
    ifs.ignore(); // Ignore newline after amount
    return ifs;
}

ostream &operator<<(ostream &os, const Transaction &trans) {
    os << "Transaction ID: " << trans.transactionID << endl;
    os << "Type: " << trans.type << endl;
    os << "Category: " << trans.category << endl;
    os << "Amount: " << fixed << setprecision(2) << trans.amount << endl;
    return os;
}

class User {
private:
    long userID;
    string name;
    map<long, Transaction*> transactions; // Using dynamic memory allocation
    float totalIncome;
    float totalExpense;
    static long nextUserID;

public:
    User() : userID(0), name(""), totalIncome(0.0f), totalExpense(0.0f) {}
    User(string name);
    ~User(); // Destructor to clean up dynamically allocated memory
    long getUserID() const { return userID; }
    string getName() const { return name; }
    void addIncome(string category, float amount);
    void addExpense(string category, float amount);
    void viewTransactionHistory() const;
    void generateSummaryReport() const;
    void deleteTransaction(long transactionID);
    friend ofstream &operator<<(ofstream &ofs, const User &user);
    friend ifstream &operator>>(ifstream &ifs, User &user);
};

long User::nextUserID = 0;

User::User(string name) {
    nextUserID++;
    this->userID = nextUserID;
    this->name = name;
    totalIncome = 0.0f;
    totalExpense = 0.0f;
}

User::~User() {
    for (auto &entry : transactions) {
        delete entry.second; // Free dynamically allocated memory
    }
}

void User::addIncome(string category, float amount) {
    Transaction* trans = new Transaction("Income", category, amount);
    transactions[trans->getTransactionID()] = trans;
    totalIncome += amount;
}

void User::addExpense(string category, float amount) {
    Transaction* trans = new Transaction("Expense", category, amount);
    transactions[trans->getTransactionID()] = trans;
    totalExpense += amount;
}

void User::viewTransactionHistory() const {
    for (const auto &entry : transactions) {
        cout << *(entry.second) << endl;
    }
}

void User::generateSummaryReport() const {
    cout << "Total Income: " << fixed << setprecision(2) << totalIncome << endl;
    cout << "Total Expense: " << fixed << setprecision(2) << totalExpense << endl;
    cout << "Net Savings: " << fixed << setprecision(2) << (totalIncome - totalExpense) << endl;
}

void User::deleteTransaction(long transactionID) {
    auto it = transactions.find(transactionID);
    if (it != transactions.end()) {
        if (it->second->getType() == "Income") {
            totalIncome -= it->second->getAmount();
        } else if (it->second->getType() == "Expense") {
            totalExpense -= it->second->getAmount();
        }
        delete it->second; // Free dynamically allocated memory
        transactions.erase(it);
    }
}

ofstream &operator<<(ofstream &ofs, const User &user) {
    ofs << user.userID << endl;
    ofs << user.name << endl;
    ofs << user.totalIncome << endl;
    ofs << user.totalExpense << endl;
    ofs << user.transactions.size() << endl; // Save number of transactions
    for (const auto &entry : user.transactions) {
        ofs << *(entry.second); // Save each transaction
    }
    return ofs;
}

ifstream &operator>>(ifstream &ifs, User &user) {
    ifs >> user.userID;
    ifs.ignore(); // Ignore newline after userID
    getline(ifs, user.name);
    ifs >> user.totalIncome >> user.totalExpense;
    long numTransactions;
    ifs >> numTransactions;
    ifs.ignore(); // Ignore newline after numTransactions
    for (long i = 0; i < numTransactions; i++) {
        Transaction* trans = new Transaction();
        ifs >> *trans;
        user.transactions[trans->getTransactionID()] = trans;
    }
    return ifs;
}

class FinanceManager {
private:
    map<long, User*> users; // Using dynamic memory allocation

public:
    FinanceManager();
    ~FinanceManager(); // Destructor to clean up dynamically allocated memory
    void addUser(const string &name);
    User* getUser(long userID);
    void listUsers() const;
    void saveData() const;
    void loadData();
};

FinanceManager::FinanceManager() {
    loadData(); // Load existing data at startup
}

FinanceManager::~FinanceManager() {
    saveData(); // Save data before exiting
    for (auto &entry : users) {
        delete entry.second; // Free dynamically allocated memory
    }
}

void FinanceManager::addUser(const string &name) {
    User* newUser = new User(name);
    users[newUser->getUserID()] = newUser;
    saveData(); // Save data after adding a user
    cout << "\nUser added successfully!\n";
}

User* FinanceManager::getUser(long userID) {
    auto it = users.find(userID);
    if (it != users.end()) {
        return it->second;
    }
    return nullptr; // Return nullptr if user not found
}

void FinanceManager::listUsers() const {
    if (users.empty()) {
        cout << "\nNo users to display.\n";
    } else {
        for (const auto &entry : users) {
            cout << "User ID: " << entry.first << " | Name: " << entry.second->getName() << endl;
        }
    }
}

void FinanceManager::saveData() const {
    ofstream ofs(DATA_FILE, ios::trunc);
    if (!ofs) {
        cerr << "Error: Unable to open file for saving data." << endl;
        return;
    }
    ofs << users.size() << endl; // Save number of users
    for (const auto &entry : users) {
        ofs << *(entry.second); // Save each user
    }
    ofs.close();
}

void FinanceManager::loadData() {
    ifstream ifs(DATA_FILE);
    if (!ifs) {
        cerr << "No existing data file found. Starting fresh." << endl;
        return;
    }
    long numUsers;
    ifs >> numUsers;
    ifs.ignore(); // Ignore newline after numUsers
    for (long i = 0; i < numUsers; i++) {
        User* user = new User();
        ifs >> *user;
        users[user->getUserID()] = user;
    }
    ifs.close();
}

int main() {
    FinanceManager fm;

    int choice;
    string name, category;
    float amount;
    long userID, transactionID;

    do {
        cout << "\n*** Personal Finance Manager ***" << endl;
        cout << "1. Add User" << endl;
        cout << "2. Select User" << endl;
        cout << "3. List Users" << endl;
        cout << "4. Quit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

        switch (choice) {
            case 1:
                cout << "Enter User Name: ";
                getline(cin, name);
                fm.addUser(name);
                break;
            case 2:
                cout << "Enter User ID: ";
                cin >> userID;
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
                {
                    User* user = fm.getUser(userID);
                    if (user) {
                        int userChoice;
                        do {
                            cout << "\nUser: " << user->getName() << endl;
                            cout << "1. Add Income" << endl;
                            cout << "2. Add Expense" << endl;
                            cout << "3. View Transaction History" << endl;
                            cout << "4. Generate Summary Report" << endl;
                            cout << "5. Delete a Transaction" << endl;
                            cout << "6. Back to Main Menu" << endl;
                            cout << "Enter choice: ";
                            cin >> userChoice;
                            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer

                            switch (userChoice) {
                                case 1:
                                    cout << "Enter Income Category: ";
                                    getline(cin, category);
                                    cout << "Enter Amount: ";
                                    cin >> amount;
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
                                    user->addIncome(category, amount);
                                    break;
                                case 2:
                                    cout << "Enter Expense Category: ";
                                    getline(cin, category);
                                    cout << "Enter Amount: ";
                                    cin >> amount;
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
                                    user->addExpense(category, amount);
                                    break;
                                case 3:
                                    user->viewTransactionHistory();
                                    break;
                                case 4:
                                    user->generateSummaryReport();
                                    break;
                                case 5:
                                    cout << "Enter Transaction ID to Delete: ";
                                    cin >> transactionID;
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
                                    user->deleteTransaction(transactionID);
                                    break;
                                case 6:
                                    break;
                                default:
                                    cout << "Invalid choice. Please try again." << endl;
                            }
                        } while (userChoice != 6);
                    } else {
                        cout << "User not found!" << endl;
                    }
                }
                break;
            case 3:
                fm.listUsers();
                break;
            case 4:
                cout << "Exiting program." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 4);

    return 0;
}
