

#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>

using namespace std;

// Helper function to open DB
sqlite3* openDB(const char* filename) {
    sqlite3* db;
    if (sqlite3_open(filename, &db) != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }
    return db;
}

// List all doctors
void listDoctors(sqlite3* db) {
    cout << "\nAvailable Doctors:\n";
    const char* sql = "SELECT DoctorID, FullName, Specialty, Availability FROM Doctors;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string name = (const char*)sqlite3_column_text(stmt, 1);
            string spec = (const char*)sqlite3_column_text(stmt, 2);
            int avail = sqlite3_column_int(stmt, 3);
            cout << "ID: " << id << ", Name: " << name << ", Specialty: " << spec << ", Available: " << (avail ? "Yes" : "No") << endl;
        }
    }
    sqlite3_finalize(stmt);
}

// List all patients
void listPatients(sqlite3* db) {
    cout << "\nRegistered Patients:\n";
    const char* sql = "SELECT PatientID, FullName, Email FROM Patients;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string name = (const char*)sqlite3_column_text(stmt, 1);
            string email = (const char*)sqlite3_column_text(stmt, 2);
            cout << "ID: " << id << ", Name: " << name << ", Email: " << email << endl;
        }
    }
    sqlite3_finalize(stmt);
}

// Book an appointment
void bookAppointment(sqlite3* db) {
    int doctorID, patientID;
    string date, notes;
    cout << "\nEnter DoctorID: ";
    cin >> doctorID;
    cout << "Enter PatientID: ";
    cin >> patientID;
    cout << "Enter Appointment Date (YYYY-MM-DD HH:MM): ";
    cin.ignore();
    getline(cin, date);
    cout << "Enter Notes: ";
    getline(cin, notes);

    // Check doctor availability
    const char* availSQL = "SELECT Availability FROM Doctors WHERE DoctorID = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, availSQL, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, doctorID);
        if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) == 1) {
            sqlite3_finalize(stmt);
            // Book appointment
            const char* insSQL = "INSERT INTO Appointments (DoctorID, PatientID, AppointmentDate, Notes) VALUES (?, ?, ?, ?);";
            if (sqlite3_prepare_v2(db, insSQL, -1, &stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt, 1, doctorID);
                sqlite3_bind_int(stmt, 2, patientID);
                sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(stmt, 4, notes.c_str(), -1, SQLITE_TRANSIENT);
                if (sqlite3_step(stmt) == SQLITE_DONE) {
                    cout << "Appointment booked successfully!\n";
                } else {
                    cout << "Failed to book appointment.\n";
                }
            }
        } else {
            cout << "Doctor not available or not found.\n";
        }
    }
    sqlite3_finalize(stmt);
}

// View all appointments
void viewAppointments(sqlite3* db) {
    cout << "\nAppointments List:\n";
    const char* sql = "SELECT a.AppointmentID, d.FullName, p.FullName, a.AppointmentDate, a.Notes FROM Appointments a JOIN Doctors d ON a.DoctorID = d.DoctorID JOIN Patients p ON a.PatientID = p.PatientID;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string doc = (const char*)sqlite3_column_text(stmt, 1);
            string pat = (const char*)sqlite3_column_text(stmt, 2);
            string date = (const char*)sqlite3_column_text(stmt, 3);
            string notes = (const char*)sqlite3_column_text(stmt, 4);
            cout << "ID: " << id << ", Doctor: " << doc << ", Patient: " << pat << ", Date: " << date << ", Notes: " << notes << endl;
        }
    }
    sqlite3_finalize(stmt);
}

// Search appointments by patient name
void searchAppointments(sqlite3* db) {
    string search;
    cout << "\nEnter patient name to search: ";
    cin.ignore();
    getline(cin, search);
    const char* sql = "SELECT a.AppointmentID, d.FullName, p.FullName, a.AppointmentDate, a.Notes FROM Appointments a JOIN Doctors d ON a.DoctorID = d.DoctorID JOIN Patients p ON a.PatientID = p.PatientID WHERE p.FullName LIKE ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        string like = "%" + search + "%";
        sqlite3_bind_text(stmt, 1, like.c_str(), -1, SQLITE_TRANSIENT);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string doc = (const char*)sqlite3_column_text(stmt, 1);
            string pat = (const char*)sqlite3_column_text(stmt, 2);
            string date = (const char*)sqlite3_column_text(stmt, 3);
            string notes = (const char*)sqlite3_column_text(stmt, 4);
            cout << "ID: " << id << ", Doctor: " << doc << ", Patient: " << pat << ", Date: " << date << ", Notes: " << notes << endl;
        }
    }
    sqlite3_finalize(stmt);
}

// Update appointment date
void updateAppointment(sqlite3* db) {
    int apptID;
    string newDate;
    cout << "\nEnter AppointmentID to update: ";
    cin >> apptID;
    cout << "Enter new date (YYYY-MM-DD HH:MM): ";
    cin.ignore();
    getline(cin, newDate);
    const char* sql = "UPDATE Appointments SET AppointmentDate = ? WHERE AppointmentID = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, newDate.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, apptID);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Appointment updated.\n";
        } else {
            cout << "Update failed.\n";
        }
    }
    sqlite3_finalize(stmt);
}

// Delete appointment
void deleteAppointment(sqlite3* db) {
    int apptID;
    cout << "\nEnter AppointmentID to delete: ";
    cin >> apptID;
    const char* sql = "DELETE FROM Appointments WHERE AppointmentID = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, apptID);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Appointment deleted.\n";
        } else {
            cout << "Delete failed.\n";
        }
    }
    sqlite3_finalize(stmt);
}

int main() {
    sqlite3* db = openDB("medicaldb.sqlite");
    if (!db) return 1;
    int choice;
    while (true) {
        cout << "\n--- Medical Appointment Booking System ---\n";
        cout << "1. List Doctors\n2. List Patients\n3. Book Appointment\n4. View Appointments\n5. Search Appointments\n6. Update Appointment\n7. Delete Appointment\n0. Exit\nChoice: ";
        cin >> choice;
        switch (choice) {
            case 1: listDoctors(db); break;
            case 2: listPatients(db); break;
            case 3: bookAppointment(db); break;
            case 4: viewAppointments(db); break;
            case 5: searchAppointments(db); break;
            case 6: updateAppointment(db); break;
            case 7: deleteAppointment(db); break;
            case 0: sqlite3_close(db); return 0;
            default: cout << "Invalid choice.\n";
        }
    }
    sqlite3_close(db);
    return 0;
}
