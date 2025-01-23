#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <iomanip>
#include <string>
using namespace std;

class Transaction {
private:
    long transactionID;
    string type;
    string category;
    float amount;
    static long nextTransactionID;

public:
    Transaction() {}
    Transaction(string type, string category, float amount);
    long getTransactionID() { return transactionID; }
    string getType() { return type; }
    string getCategory() { return category; }
    float getAmount() { return amount; }
    static void setLastTransactionID(long id) { nextTransactionID = id; }
    static long getLastTransactionID() { return nextTransactionID; }
    friend ofstream &operator<<(ofstream &ofs, Transaction &trans);
    friend ifstream &operator>>(ifstream &ifs, Transaction &trans);
    friend ostream &operator<<(ostream &os, Transaction &trans);
};

long Transaction::nextTransactionID = 0;

Transaction::Transaction(string type, string category, float amount) {
    nextTransactionID++;
    this->transactionID = nextTransactionID;
    this->type = type;
    this->category = category;
    this->amount = amount;
}

ofstream &operator<<(ofstream &ofs, Transaction &trans) {
    ofs << trans.transactionID << endl;
    ofs << trans.type << endl;
    ofs << trans.category << endl;
    ofs << trans.amount << endl;
    return ofs;
}

ifstream &operator>>(ifstream &ifs, Transaction &trans) {
    ifs >> trans.transactionID;
    ifs >> trans.type;
    ifs >> trans.category;
    ifs >> trans.amount;
    return ifs;
}

ostream &operator<<(ostream &os, Transaction &trans) {
    os << "Transaction ID: " << trans.transactionID << endl;
    os << "Type: " << trans.type << endl;
    os << "Category: " << trans.category << endl;
    os << "Amount: " << fixed << setprecision(2) << trans.amount << endl;
    return os;
}

class FinanceManager {
private:
    map<long, Transaction> transactions;
    float totalIncome;
    float totalExpense;

public:
    FinanceManager();
    void addIncome(string category, float amount);
    void addExpense(string category, float amount);
    void viewTransactionHistory();
    void generateSummaryReport();
    void deleteTransaction(long transactionID);
    ~FinanceManager();
};

FinanceManager::FinanceManager() {
    Transaction trans;
    totalIncome = 0.0;
    totalExpense = 0.0;
    ifstream infile("FinanceManager.data");
    if (!infile) {
        return;
    }
    while (!infile.eof()) {
        infile >> trans;
        if (infile) {
            transactions.insert({trans.getTransactionID(), trans});
            if (trans.getType() == "Income") {
                totalIncome += trans.getAmount();
            } else if (trans.getType() == "Expense") {
                totalExpense += trans.getAmount();
            }
        }
    }
    Transaction::setLastTransactionID(trans.getTransactionID());
    infile.close();
}

void FinanceManager::addIncome(string category, float amount) {
    Transaction trans("Income", category, amount);
    transactions.insert({trans.getTransactionID(), trans});
    totalIncome += amount;
}

void FinanceManager::addExpense(string category, float amount) {
    Transaction trans("Expense", category, amount);
    transactions.insert({trans.getTransactionID(), trans});
    totalExpense += amount;
}

void FinanceManager::viewTransactionHistory() {
    for (auto &entry : transactions) {
        cout << entry.second << endl;
    }
}

void FinanceManager::generateSummaryReport() {
    cout << "Total Income: " << fixed << setprecision(2) << totalIncome << endl;
    cout << "Total Expense: " << fixed << setprecision(2) << totalExpense << endl;
    cout << "Net Savings: " << fixed << setprecision(2) << (totalIncome - totalExpense) << endl;
}

void FinanceManager::deleteTransaction(long transactionID) {
    auto it = transactions.find(transactionID);
    if (it != transactions.end()) {
        if (it->second.getType() == "Income") {
            totalIncome -= it->second.getAmount();
        } else if (it->second.getType() == "Expense") {
            totalExpense -= it->second.getAmount();
        }
        transactions.erase(it);
    }
}

FinanceManager::~FinanceManager() {
    ofstream outfile("FinanceManager.data", ios::trunc);
    for (auto &entry : transactions) {
        outfile << entry.second;
    }
    outfile.close();
}

int main() {
    FinanceManager fm;
    int choice;
    string category;
    float amount;
    long transactionID;

    do {
        cout << "\n*** Personal Finance Manager ***" << endl;
        cout << "1. Add Income" << endl;
        cout << "2. Add Expense" << endl;
        cout << "3. View Transaction History" << endl;
        cout << "4. Generate Summary Report" << endl;
        cout << "5. Delete a Transaction" << endl;
        cout << "6. Quit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter Income Category: ";
                cin >> category;
                cout << "Enter Amount: ";
                cin >> amount;
                fm.addIncome(category, amount);
                break;
            case 2:
                cout << "Enter Expense Category: ";
                cin >> category;
                cout << "Enter Amount: ";
                cin >> amount;
                fm.addExpense(category, amount);
                break;
            case 3:
                fm.viewTransactionHistory();
                break;
            case 4:
                fm.generateSummaryReport();
                break;
            case 5:
                cout << "Enter Transaction ID to Delete: ";
                cin >> transactionID;
                fm.deleteTransaction(transactionID);
                break;
            case 6:
                cout << "Exiting the program." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 6);

    return 0;
}
