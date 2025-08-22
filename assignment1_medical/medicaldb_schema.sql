-- MedicalDB schema for Assignment 1 (SQLite version)
CREATE TABLE Doctors (
    DoctorID INTEGER PRIMARY KEY AUTOINCREMENT,
    FullName TEXT NOT NULL,
    Specialty TEXT NOT NULL,
    Availability INTEGER NOT NULL -- 1 for available, 0 for not
);

CREATE TABLE Patients (
    PatientID INTEGER PRIMARY KEY AUTOINCREMENT,
    FullName TEXT NOT NULL,
    Email TEXT NOT NULL
);

CREATE TABLE Appointments (
    AppointmentID INTEGER PRIMARY KEY AUTOINCREMENT,
    DoctorID INTEGER NOT NULL,
    PatientID INTEGER NOT NULL,
    AppointmentDate TEXT NOT NULL,
    Notes TEXT,
    FOREIGN KEY (DoctorID) REFERENCES Doctors(DoctorID),
    FOREIGN KEY (PatientID) REFERENCES Patients(PatientID)
);

-- Sample data
INSERT INTO Doctors (FullName, Specialty, Availability) VALUES
('Dr. Alice Smith', 'Cardiology', 1),
('Dr. Bob Johnson', 'Dermatology', 1),
('Dr. Carol Lee', 'Pediatrics', 0);

INSERT INTO Patients (FullName, Email) VALUES
('John Doe', 'john@example.com'),
('Jane Roe', 'jane@example.com');
