// Assignment 2: Pharmacy Inventory & Sales System (Console-based, C++/SQLite)
// Author: GitHub Copilot
// Compile with: g++ -std=c++17 -lsqlite3 -o pharmacydb_app pharmacydb_app.cpp

#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

sqlite3* openDB(const char* filename) {
    sqlite3* db;
    if (sqlite3_open(filename, &db) != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }
    return db;
}

// Add new medicine
void addMedicine(sqlite3* db) {
    string name, category;
    double price;
    int quantity;
    cout << "\nEnter Medicine Name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter Category: ";
    getline(cin, category);
    cout << "Enter Price: ";
    cin >> price;
    cout << "Enter Quantity: ";
    cin >> quantity;
    const char* sql = "INSERT INTO Medicines (Name, Category, Price, Quantity) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, category.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 3, price);
        sqlite3_bind_int(stmt, 4, quantity);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Medicine added successfully!\n";
        } else {
            cout << "Failed to add medicine.\n";
        }
    }
    sqlite3_finalize(stmt);
}

// Search medicines by name or category
void searchMedicine(sqlite3* db) {
    string term;
    cout << "\nEnter search term (name or category): ";
    cin.ignore();
    getline(cin, term);
    const char* sql = "SELECT MedicineID, Name, Category, Price, Quantity FROM Medicines WHERE Name LIKE ? OR Category LIKE ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        string like = "%" + term + "%";
        sqlite3_bind_text(stmt, 1, like.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, like.c_str(), -1, SQLITE_TRANSIENT);
        cout << "\nSearch Results:\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string name = (const char*)sqlite3_column_text(stmt, 1);
            string cat = (const char*)sqlite3_column_text(stmt, 2);
            double price = sqlite3_column_double(stmt, 3);
            int qty = sqlite3_column_int(stmt, 4);
            cout << "ID: " << id << ", Name: " << name << ", Category: " << cat << ", Price: " << price << ", Quantity: " << qty << endl;
        }
    }
    sqlite3_finalize(stmt);
}

// Update stock
void updateStock(sqlite3* db) {
    int medID, qty;
    cout << "\nEnter MedicineID to update: ";
    cin >> medID;
    cout << "Enter new quantity: ";
    cin >> qty;
    const char* sql = "UPDATE Medicines SET Quantity = ? WHERE MedicineID = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, qty);
        sqlite3_bind_int(stmt, 2, medID);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Stock updated.\n";
        } else {
            cout << "Update failed.\n";
        }
    }
    sqlite3_finalize(stmt);
}

// Record sale
void recordSale(sqlite3* db) {
    int medID, qtySold;
    cout << "\nEnter MedicineID: ";
    cin >> medID;
    cout << "Enter quantity sold: ";
    cin >> qtySold;
    // Get current date
    time_t now = time(0);
    char dateStr[20];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M", localtime(&now));
    // Insert sale
    const char* sql = "INSERT INTO Sales (MedicineID, QuantitySold, SaleDate) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, medID);
        sqlite3_bind_int(stmt, 2, qtySold);
        sqlite3_bind_text(stmt, 3, dateStr, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            // Update stock
            sqlite3_finalize(stmt);
            const char* updSQL = "UPDATE Medicines SET Quantity = Quantity - ? WHERE MedicineID = ?;";
            if (sqlite3_prepare_v2(db, updSQL, -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, qtySold);
                sqlite3_bind_int(stmt, 2, medID);
                if (sqlite3_step(stmt) == SQLITE_DONE) {
                    cout << "Sale recorded and stock updated.\n";
                } else {
                    cout << "Sale recorded but stock update failed.\n";
                }
            }
        } else {
            cout << "Failed to record sale.\n";
        }
    }
    sqlite3_finalize(stmt);
}

// View all medicines
void viewAllMedicines(sqlite3* db) {
    cout << "\nMedicine List:\n";
    const char* sql = "SELECT MedicineID, Name, Category, Price, Quantity FROM Medicines;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string name = (const char*)sqlite3_column_text(stmt, 1);
            string cat = (const char*)sqlite3_column_text(stmt, 2);
            double price = sqlite3_column_double(stmt, 3);
            int qty = sqlite3_column_int(stmt, 4);
            cout << "ID: " << id << ", Name: " << name << ", Category: " << cat << ", Price: " << price << ", Quantity: " << qty << endl;
        }
    }
    sqlite3_finalize(stmt);
}

int main() {
    sqlite3* db = openDB("pharmacydb.sqlite");
    if (!db) return 1;
    int choice;
    while (true) {
        cout << "\n--- Pharmacy Inventory & Sales System ---\n";
        cout << "1. Add Medicine\n2. Search Medicine\n3. Update Stock\n4. Record Sale\n5. View All Medicines\n0. Exit\nChoice: ";
        cin >> choice;
        switch (choice) {
            case 1: addMedicine(db); break;
            case 2: searchMedicine(db); break;
            case 3: updateStock(db); break;
            case 4: recordSale(db); break;
            case 5: viewAllMedicines(db); break;
            case 0: sqlite3_close(db); return 0;
            default: cout << "Invalid choice.\n";
        }
    }
    sqlite3_close(db);
    return 0;
}
