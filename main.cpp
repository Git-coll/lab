#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>
#include <map>
using namespace std;

// Product structure
struct Product {
    int id;
    string name;
    double purchasePrice;
    double sellingPrice;
    int stock;
    int sold;
};

// User structure
struct User {
    string username;
    string password;
    string role;
};

// Sales Transaction structure
struct SalesTransaction {
    int id;
    string productName;
    int quantity;
    double pricePerUnit;
    double totalAmount;
    time_t timestamp;
};

// Function to add a user
void addUser(vector<User>& users, const User& adminUser) {
    if (adminUser.role != "ADMIN") {
        cout << "Only administrators can add users.\n";
        return;
    }
    User newUser;
    cout << "Enter new username: ";
    cin >> newUser.username;
    cout << "Enter password: ";
    cin >> newUser.password;
    cout << "Enter role (ADMIN/MANAGER/STAFF): ";
    cin >> newUser.role;
    users.push_back(newUser);
    cout << "User added successfully.\n";
}

// Function to add a product to the inventory
void addProduct(vector<Product>& products, int id, string name, double purchasePrice, double sellingPrice, int stock) {
    products.push_back({ id, name, purchasePrice, sellingPrice, stock, 0 });
    cout << "Added product: " << name << "\n";
}

// Function to update prices
void updatePrice(vector<Product>& products, int id, double newPurchasePrice, double newSellingPrice, const User& user) {
    if (user.role != "MANAGER" && user.role != "ADMIN") {
        cout << "Only managers or administrators can update prices.\n";
        return;
    }
    for (auto& product : products) {
        if (product.id == id) {
            product.purchasePrice = newPurchasePrice;
            product.sellingPrice = newSellingPrice;
            cout << "Updated prices for: " << product.name << "\n";
            return;
        }
    }
    cout << "Product not found.\n";
}

// Function to restock a product
void restock(vector<Product>& products, int id, int quantity) {
    for (auto& product : products) {
        if (product.id == id) {
            product.stock += quantity;
            cout << "Restocked: " << product.name << ". New stock: " << product.stock << "\n";
            return;
        }
    }
    cout << "Product not found.\n";
}

// Function to search for a product
void searchProduct(const vector<Product>& products, const string& query) {
    bool found = false;
    for (const auto& product : products) {
        if (product.name == query || to_string(product.id) == query) {
            found = true;
            cout << "Product found: ID: " << product.id << ", Name: " << product.name
                << ", Purchase Price: " << product.purchasePrice
                << ", Selling Price: " << product.sellingPrice
                << ", Stock: " << product.stock << "\n";
        }
    }
    if (!found) {
        cout << "Product not found.\n";
    }
}

// Function to display the inventory
void showInventory(const vector<Product>& products) {
    if (products.empty()) {
        cout << "The inventory is empty.\n";
        return;
    }
    cout << left << setw(10) << "ID" << setw(20) << "Name" << setw(20) << "Purchase Price"
        << setw(20) << "Selling Price" << setw(10) << "Stock" << setw(10) << "Sold" << setw(20) << "Revenue" << "\n";
    cout << "---------------------------------------------------------------------------------------------------\n";
    for (const auto& product : products) {
        cout << left << setw(10) << product.id << setw(20) << product.name
            << setw(20) << fixed << setprecision(2) << product.purchasePrice
            << setw(20) << fixed << setprecision(2) << product.sellingPrice
            << setw(10) << product.stock << setw(10) << product.sold
            << setw(20) << fixed << setprecision(2) << product.sellingPrice * product.sold << "\n";
    }
}

// Function to sell a product and record the transaction
void sellProduct(vector<Product>& products, int id, int quantity, vector<SalesTransaction>& transactions) {
    for (auto& product : products) {
        if (product.id == id) {
            if (quantity > product.stock) {
                cout << "Not enough stock available for " << product.name << ". Only " << product.stock << " items left.\n";
                return;
            }
            product.stock -= quantity;
            product.sold += quantity;
            double totalAmount = quantity * product.sellingPrice;
            time_t now = time(0);
            transactions.push_back({ id, product.name, quantity, product.sellingPrice, totalAmount, now });
            cout << "Sold " << quantity << " of " << product.name << ". Remaining stock: " << product.stock << "\n";
            return;
        }
    }
    cout << "Product not found.\n";
}

// Function to display sales transactions
void showTransactions(const vector<SalesTransaction>& transactions) {
    if (transactions.empty()) {
        cout << "No transactions available.\n";
        return;
    }
    cout << left << setw(10) << "ID" << setw(20) << "Product" << setw(10) << "Quantity"
        << setw(15) << "Price/Unit" << setw(15) << "Total Amount" << setw(25) << "Timestamp" << "\n";
    cout << "-----------------------------------------------------------------------------------------\n";
    for (const auto& transaction : transactions) {
        tm local_tm;
        localtime_s(&local_tm, &transaction.timestamp);
        cout << left << setw(10) << transaction.id << setw(20) << transaction.productName
            << setw(10) << transaction.quantity << setw(15) << fixed << setprecision(2) << transaction.pricePerUnit
            << setw(15) << fixed << setprecision(2) << transaction.totalAmount
            << put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << "\n";
    }
}

// Function to calculate and display revenue
void calculateRevenue(const vector<SalesTransaction>& transactions, const string& period) {
    map<int, double> revenue;
    for (const auto& transaction : transactions) {
        tm local_tm;
        localtime_s(&local_tm, &transaction.timestamp);
        int periodKey;
        if (period == "month") {
            periodKey = local_tm.tm_year * 12 + local_tm.tm_mon;
        }
        else if (period == "quarter") {
            periodKey = local_tm.tm_year * 4 + local_tm.tm_mon / 3;
        }
        else if (period == "year") {
            periodKey = local_tm.tm_year;
        }
        else {
            cout << "Invalid period specified.\n";
            return;
        }
        revenue[periodKey] += transaction.totalAmount;
    }

    cout << "Revenue (" << period << "):\n";
    for (const auto& rev : revenue) {
        cout << "Period: " << rev.first << ", Revenue: " << fixed << setprecision(2) << rev.second << "\n";
    }
}

// Function to display receipt for a specific transaction
void displayReceipt(const SalesTransaction& transaction) {
    tm local_tm;
    localtime_s(&local_tm, &transaction.timestamp);
    cout << "\nReceipt\n";
    cout << "------------------------\n";
    cout << "Product: " << transaction.productName << "\n";
    cout << "Quantity: " << transaction.quantity << "\n";
    cout << "Price per Unit: " << fixed << setprecision(2) << transaction.pricePerUnit << "\n";
    cout << "Total Amount: " << fixed << setprecision(2) << transaction.totalAmount << "\n";
    cout << "Timestamp: " << put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << "\n";
    cout << "------------------------\n";
}

int main() {
    vector<Product> products;
    vector<User> users = {
        {"admin", "adminpass", "ADMIN"},
        {"manager", "managerpass", "MANAGER"},
        {"staff", "staffpass", "STAFF"}
    };
    vector<SalesTransaction> transactions;

    string currentUser;
    string currentPassword;
    User* loggedInUser = nullptr;

    while (true) {
        cout << "Enter username (or 'exit' to quit): ";
        cin >> currentUser;

        if (currentUser == "exit") break;

        cout << "Enter password: ";
        cin >> currentPassword;

        for (auto& user : users) {
            if (user.username == currentUser && user.password == currentPassword) {
                loggedInUser = &user;
                break;
            }
        }

        if (!loggedInUser) {
            cout << "Invalid username or password. Access denied.\n";
            continue;
        }

        cout << "Welcome, " << loggedInUser->username << "!\nRole: " << loggedInUser->role << "\n";

        int choice;
        while (true) {
            cout << "\n=====Inventory Management System=====\n";
            cout << "1. Add Product\n";
            cout << "2. Update Prices\n";
            cout << "3. Restock Product\n";
            cout << "4. Show Inventory\n";
            cout << "5. Sell Product\n";
            cout << "6. Show Transactions\n";
            cout << "7. Display Receipt\n";
            cout << "8. Search Product\n";
            cout << "9. Calculate Revenue\n";
            cout << "10. Add User\n";
            cout << "11. Logout\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 11) break;

            switch (choice) {
            case 1: {
                if (loggedInUser->role != "ADMIN") {
                    cout << "Only administrators can add products.\n";
                    break;
                }
                int id, stock;
                string name;
                double purchasePrice, sellingPrice;
                cout << "Enter Product ID: ";
                cin >> id;
                cout << "Enter Product Name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter Purchase Price: ";
                cin >> purchasePrice;
                cout << "Enter Selling Price: ";
                cin >> sellingPrice;
                cout << "Enter Stock Quantity: ";
                cin >> stock;
                addProduct(products, id, name, purchasePrice, sellingPrice, stock);
                break;
            }
            case 2: {
                int id;
                double newPurchasePrice, newSellingPrice;
                cout << "Enter Product ID to Update Prices: ";
                cin >> id;
                cout << "Enter New Purchase Price: ";
                cin >> newPurchasePrice;
                cout << "Enter New Selling Price: ";
                cin >> newSellingPrice;
                updatePrice(products, id, newPurchasePrice, newSellingPrice, *loggedInUser);
                break;
            }
            case 3: {
                int id, quantity;
                cout << "Enter Product ID to Restock: ";
                cin >> id;
                cout << "Enter Quantity to Add: ";
                cin >> quantity;
                restock(products, id, quantity);
                break;
            }
            case 4:
                showInventory(products);
                break;
            case 5: {
                int id, quantity;
                cout << "Enter Product ID to Sell: ";
                cin >> id;
                cout << "Enter Quantity to Sell: ";
                cin >> quantity;
                sellProduct(products, id, quantity, transactions);
                break;
            }
            case 6:
                showTransactions(transactions);
                break;
            case 7: {
                if (transactions.empty()) {
                    cout << "No transactions available.\n";
                    break;
                }
                int transactionId;
                cout << "Enter Transaction ID to Display Receipt: ";
                cin >> transactionId;
                if (transactionId < 1 || transactionId > transactions.size()) {
                    cout << "Invalid transaction ID.\n";
                }
                else {
                    displayReceipt(transactions[transactionId - 1]);
                }
                break;
            }
            case 8: {
                string query;
                cout << "Enter Product Name or ID to Search: ";
                cin.ignore();
                getline(cin, query);
                searchProduct(products, query);
                break;
            }
            case 9: {
                string period;
                cout << "Enter period to calculate revenue (month/quarter/year): ";
                cin >> period;
                calculateRevenue(transactions, period);
                break;
            }
            case 10:
                addUser(users, *loggedInUser);
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
            }
        }

        loggedInUser = nullptr; // Reset logged in user after logout
    }

    return 0;
}
