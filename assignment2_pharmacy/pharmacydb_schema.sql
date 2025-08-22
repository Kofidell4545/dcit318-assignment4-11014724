-- PharmacyDB schema for Assignment 2 (SQLite version)
CREATE TABLE Medicines (
    MedicineID INTEGER PRIMARY KEY AUTOINCREMENT,
    Name TEXT NOT NULL,
    Category TEXT NOT NULL,
    Price REAL NOT NULL,
    Quantity INTEGER NOT NULL
);

CREATE TABLE Sales (
    SaleID INTEGER PRIMARY KEY AUTOINCREMENT,
    MedicineID INTEGER NOT NULL,
    QuantitySold INTEGER NOT NULL,
    SaleDate TEXT NOT NULL,
    FOREIGN KEY (MedicineID) REFERENCES Medicines(MedicineID)
);

-- Sample data
INSERT INTO Medicines (Name, Category, Price, Quantity) VALUES
('Paracetamol', 'Painkiller', 2.5, 100),
('Amoxicillin', 'Antibiotic', 5.0, 50),
('Cetirizine', 'Antihistamine', 3.0, 75);
