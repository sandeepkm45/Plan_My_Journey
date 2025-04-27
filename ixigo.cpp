#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <mysql/mysql.h>

using namespace std;

// MySQL connection details
const char* HOST = "localhost";
const char* USER = "root";
const char* PASSWORD = "Sandy@1845";
const char* DATABASE = "travel_booking_system";

// Database connection class
class DBConnection {
private:
    MYSQL* conn;
    bool connected;

public:
    DBConnection() {
        conn = mysql_init(NULL);
        connected = false;

        if (conn == NULL) {
            cout << "MySQL initialization failed" << endl;
            return;
        }

        if (mysql_real_connect(conn, HOST, USER, PASSWORD, DATABASE, 0, NULL, 0) == NULL) {
            cout << "Connection Error: " << mysql_error(conn) << endl;
            return;
        }

        connected = true;
        cout << "Database connection established successfully" << endl;
    }

    ~DBConnection() {
        if (conn) {
            mysql_close(conn);
            cout << "Database connection closed" << endl;
        }
    }

    bool isConnected() const {
        return connected;
    }

    bool executeQuery(const string& query) {
        if (!connected) return false;

        if (mysql_query(conn, query.c_str())) {
            cout << "Query Error: " << mysql_error(conn) << endl;
            return false;
        }
        return true;
    }

    MYSQL_RES* getResult() {
        return mysql_store_result(conn);
    }

    long long getLastInsertId() {
        return mysql_insert_id(conn);
    }
};

// Initialize Database - creates tables if they don't exist
void initializeDatabase() {
    DBConnection db;
    if (!db.isConnected()) {
        cout << "Failed to initialize database." << endl;
        return;
    }

    // Create Users table
    string createUsersTable = "CREATE TABLE IF NOT EXISTS users ("
                             "id INT AUTO_INCREMENT PRIMARY KEY, "
                             "name VARCHAR(100) NOT NULL, "
                             "mobile BIGINT NOT NULL, "
                             "email VARCHAR(100) NOT NULL"
                             ")";
    db.executeQuery(createUsersTable);

    // Create Railway bookings table
    string createRailwayTable = "CREATE TABLE IF NOT EXISTS railway_bookings ("
                               "id INT AUTO_INCREMENT PRIMARY KEY, "
                               "user_id INT NOT NULL, "
                               "start_station VARCHAR(100) NOT NULL, "
                               "end_station VARCHAR(100) NOT NULL, "
                               "journey_date DATE NOT NULL, "
                               "travel_class VARCHAR(20) NOT NULL, "
                               "fare INT NOT NULL, "
                               "booking_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                               "FOREIGN KEY (user_id) REFERENCES users(id)"
                               ")";
    db.executeQuery(createRailwayTable);

    // Create Flight bookings table
    string createFlightTable = "CREATE TABLE IF NOT EXISTS flight_bookings ("
                              "id INT AUTO_INCREMENT PRIMARY KEY, "
                              "user_id INT NOT NULL, "
                              "start_city VARCHAR(100) NOT NULL, "
                              "end_city VARCHAR(100) NOT NULL, "
                              "journey_date DATE NOT NULL, "
                              "travel_class VARCHAR(20) NOT NULL, "
                              "fare INT NOT NULL, "
                              "booking_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                              "FOREIGN KEY (user_id) REFERENCES users(id)"
                              ")";
    db.executeQuery(createFlightTable);

    // Create Hotel bookings table
    string createHotelTable = "CREATE TABLE IF NOT EXISTS hotel_bookings ("
                             "id INT AUTO_INCREMENT PRIMARY KEY, "
                             "user_id INT NOT NULL, "
                             "location VARCHAR(100) NOT NULL, "
                             "hotel_name VARCHAR(100) NOT NULL, "
                             "room_type VARCHAR(20) NOT NULL, "
                             "check_in_date DATE NOT NULL, "
                             "check_out_date DATE NOT NULL, "
                             "num_rooms INT NOT NULL, "
                             "num_guests INT NOT NULL, "
                             "total_fare INT NOT NULL, "
                             "booking_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                             "FOREIGN KEY (user_id) REFERENCES users(id)"
                             ")";
    db.executeQuery(createHotelTable);
    
    // Create Bus bookings table
    string createBusTable = "CREATE TABLE IF NOT EXISTS bus_bookings ("
                           "id INT AUTO_INCREMENT PRIMARY KEY, "
                           "user_id INT NOT NULL, "
                           "start_city VARCHAR(100) NOT NULL, "
                           "end_city VARCHAR(100) NOT NULL, "
                           "journey_date DATE NOT NULL, "
                           "travel_class VARCHAR(20) NOT NULL, "
                           "fare INT NOT NULL, "
                           "booking_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
                           "FOREIGN KEY (user_id) REFERENCES users(id)"
                           ")";
    db.executeQuery(createBusTable);
}

class User
{
protected:
    DBConnection db;
    
public:
    int user_id;
    int day, month, year;
    string name;
    long mobile;
    string email;

    User() {}
    
    void details()
    {
        cout << "Enter Name: ";
        cin >> name;
        cin.ignore();
        cout << "Enter Mobile Number: ";
        cin >> mobile;
        cin.ignore();
        cout << "Enter Email Id: ";
        cin >> email;
        cin.ignore();
        cout << "Enter Your Journey Date (DD MM YYYY): ";
        cin >> day >> month >> year;
        cout << endl;
        
        // Save user to database
        saveUserToDatabase();
    }
    
    void saveUserToDatabase() {
        if (!db.isConnected()) {
            cout << "Database connection failed. User details not saved." << endl;
            return;
        }
        
        // Format query to insert user data
        string query = "INSERT INTO users (name, mobile, email) VALUES ('" + 
                       name + "', " + to_string(mobile) + ", '" + email + "')";
        
        if (db.executeQuery(query)) {
            user_id = db.getLastInsertId();
            cout << "User registered with ID: " << user_id << endl;
        } else {
            cout << "Failed to save user data." << endl;
        }
    }
};

// Function to display user details
void showUserDetails(DBConnection &db, int userId) {
    if (!db.isConnected()) {
        cout << "Database connection failed." << endl;
        return;
    }
    
    string query = "SELECT id, name, mobile, email FROM users WHERE id = " + to_string(userId);
    
    if (db.executeQuery(query)) {
        MYSQL_RES* result = db.getResult();
        if (result) {
            MYSQL_ROW row = mysql_fetch_row(result);
            if (row) {
                cout << "\n--- User Details ---\n";
                cout << "User ID: " << row[0] << endl;
                cout << "Name: " << row[1] << endl;
                cout << "Mobile: " << row[2] << endl;
                cout << "Email: " << row[3] << endl;
                cout << "------------------------\n";
            } else {
                cout << "User not found." << endl;
            }
            mysql_free_result(result);
        }
    }
}

// Function to handle booking cancellations for all booking types
void cancelBooking(DBConnection &db) {
    if (!db.isConnected()) {
        cout << "Database connection failed." << endl;
        return;
    }
    
    int userId;
    cout << "Enter your User ID: ";
    cin >> userId;
    
    // Show user details first
    showUserDetails(db, userId);
    
    // Show all bookings for this user
    cout << "\n--- Your Bookings ---\n";
    
    // Railway bookings
    string query = "SELECT id, start_station, end_station, journey_date, travel_class, fare FROM railway_bookings WHERE user_id = " + to_string(userId);
    if (db.executeQuery(query)) {
        MYSQL_RES* result = db.getResult();
        if (result) {
            MYSQL_ROW row;
            cout << "\nRAILWAY BOOKINGS:\n";
            cout << setw(5) << "ID" << setw(20) << "From" << setw(20) << "To" << setw(15) << "Date" 
                 << setw(15) << "Class" << setw(10) << "Fare" << endl;
            cout << string(85, '-') << endl;
            
            bool hasRailwayBookings = false;
            while ((row = mysql_fetch_row(result))) {
                hasRailwayBookings = true;
                cout << setw(5) << row[0] << setw(20) << row[1] << setw(20) << row[2] << setw(15) << row[3] 
                     << setw(15) << row[4] << setw(10) << row[5] << endl;
            }
            
            if (!hasRailwayBookings) {
                cout << "No railway bookings found.\n";
            }
            
            mysql_free_result(result);
        }
    }
    
    // Flight bookings
    query = "SELECT id, start_city, end_city, journey_date, travel_class, fare FROM flight_bookings WHERE user_id = " + to_string(userId);
    if (db.executeQuery(query)) {
        MYSQL_RES* result = db.getResult();
        if (result) {
            MYSQL_ROW row;
            cout << "\nFLIGHT BOOKINGS:\n";
            cout << setw(5) << "ID" << setw(20) << "From" << setw(20) << "To" << setw(15) << "Date" 
                 << setw(15) << "Class" << setw(10) << "Fare" << endl;
            cout << string(85, '-') << endl;
            
            bool hasFlightBookings = false;
            while ((row = mysql_fetch_row(result))) {
                hasFlightBookings = true;
                cout << setw(5) << row[0] << setw(20) << row[1] << setw(20) << row[2] << setw(15) << row[3] 
                     << setw(15) << row[4] << setw(10) << row[5] << endl;
            }
            
            if (!hasFlightBookings) {
                cout << "No flight bookings found.\n";
            }
            
            mysql_free_result(result);
        }
    }
    
    // Bus bookings
    query = "SELECT id, start_city, end_city, journey_date, travel_class, fare FROM bus_bookings WHERE user_id = " + to_string(userId);
    if (db.executeQuery(query)) {
        MYSQL_RES* result = db.getResult();
        if (result) {
            MYSQL_ROW row;
            cout << "\nBUS BOOKINGS:\n";
            cout << setw(5) << "ID" << setw(20) << "From" << setw(20) << "To" << setw(15) << "Date" 
                 << setw(15) << "Class" << setw(10) << "Fare" << endl;
            cout << string(85, '-') << endl;
            
            bool hasBusBookings = false;
            while ((row = mysql_fetch_row(result))) {
                hasBusBookings = true;
                cout << setw(5) << row[0] << setw(20) << row[1] << setw(20) << row[2] << setw(15) << row[3] 
                     << setw(15) << row[4] << setw(10) << row[5] << endl;
            }
            
            if (!hasBusBookings) {
                cout << "No bus bookings found.\n";
            }
            
            mysql_free_result(result);
        }
    }
    
    // Hotel bookings
    query = "SELECT id, location, hotel_name, check_in_date, check_out_date, room_type, total_fare FROM hotel_bookings WHERE user_id = " + to_string(userId);
    if (db.executeQuery(query)) {
        MYSQL_RES* result = db.getResult();
        if (result) {
            MYSQL_ROW row;
            cout << "\nHOTEL BOOKINGS:\n";
            cout << setw(5) << "ID" << setw(15) << "Location" << setw(20) << "Hotel" 
                 << setw(12) << "Check-in" << setw(12) << "Check-out" 
                 << setw(15) << "Room Type" << setw(10) << "Fare" << endl;
            cout << string(90, '-') << endl;
            
            bool hasHotelBookings = false;
            while ((row = mysql_fetch_row(result))) {
                hasHotelBookings = true;
                cout << setw(5) << row[0] << setw(15) << row[1] << setw(20) << row[2] 
                     << setw(12) << row[3] << setw(12) << row[4] 
                     << setw(15) << row[5] << setw(10) << row[6] << endl;
            }
            
            if (!hasHotelBookings) {
                cout << "No hotel bookings found.\n";
            }
            
            mysql_free_result(result);
        }
    }
    
    // Cancellation process
    string bookingType;
    int bookingId;
    char confirm;
    
    cout << "\nEnter booking type to cancel (railway/flight/bus/hotel): ";
    cin >> bookingType;
    cout << "Enter booking ID to cancel: ";
    cin >> bookingId;
    
    string tableName;
    if (bookingType == "railway") {
        tableName = "railway_bookings";
    } else if (bookingType == "flight") {
        tableName = "flight_bookings";
    } else if (bookingType == "bus") {
        tableName = "bus_bookings";
    } else if (bookingType == "hotel") {
        tableName = "hotel_bookings";
    } else {
        cout << "Invalid booking type." << endl;
        return;
    }
    
    // Verify this is the user's booking
    query = "SELECT id FROM " + tableName + " WHERE id = " + to_string(bookingId) + " AND user_id = " + to_string(userId);
    if (db.executeQuery(query)) {
        MYSQL_RES* result = db.getResult();
        if (result) {
            if (mysql_num_rows(result) == 0) {
                cout << "No matching booking found for your user ID." << endl;
                mysql_free_result(result);
                return;
            }
            mysql_free_result(result);
        }
    }
    
    cout << "Are you sure you want to cancel this booking? (y/n): ";
    cin >> confirm;
    
    if (tolower(confirm) == 'y') {
        query = "DELETE FROM " + tableName + " WHERE id = " + to_string(bookingId) + " AND user_id = " + to_string(userId);
        
        if (db.executeQuery(query)) {
            cout << "Booking with ID " << bookingId << " has been cancelled successfully." << endl;
            cout << "Your refund will be processed within 3-5 business days." << endl;
        } else {
            cout << "Failed to cancel booking." << endl;
        }
    } else {
        cout << "Cancellation aborted." << endl;
    }
}

class Railway : public User
{
public:
    string travel_class;
    string start;
    string end;
    int klass;
    map<int, string> state;
    map<int, string> station;

    Railway()
    {
        station[1] = "BZA (Vijaywada Jn), VSKP (Visakhapatnam), TPTY (Tirupati), GTL (Guntakal Jn), CKP (Kakinada Port)";

        station[2] = "NLN (Naharlagun)";

        station[3] = "GHY (Guahati), DBR (Dibrugarh Jn), JTTN (Jorhat Town)";

        station[4] = "PNBE (Patna Jn), GAYA (Gaya Jn), MFP (Muzaffarpur Jn), BGP (Bhagalpur), DBR (Darbhanga Jn)";

        station[5] = "R (Raipur Jn), BSP (Bilaspur Jn), KRB (Korba)";

        station[6] = "MAO (Madgaon), VSG (Vasco-da-Gama)";

        station[7] = "ADI (Ahmedabad Jn), ST (Surat), BRC (Vadoara), RJT (Rajkot Jn), BVC (Bhavnagar)";

        station[8] = "GGM (Gurugram), FDB (Faridabad Jn), HSR (Hisar), UMB (Umbala Jn)";

        station[9] = "SML (Shimla), SOL (Solan)";

        station[10] = "RNC (Ranchi), TATA (Jamshedpur), DHN (Dhanbad Jn)";

        station[11] = "SBC (Bengaluru City Jn), MAJN (Manglore Jn), UBL (Hubli Jn)";

        station[12] = "TVC (Thiruvananthapuram Central), ERS (Kochi Jn), CLT (Kozhikode)";

        station[13] = "BPL (Bhopal Jn), GWL (Gwalior), INDB (Indore Jn), UJN (Ujjain Jn)";

        station[14] = "NGP (Nagpur JN), AWB (Aurangabad), PUNE (Pune JN), CSTM (Mumbai CSTM)";

        station[15] = "IMF (Imphal)";

        station[16] = "TURA (Tura)";

        station[17] = "AIZ (Aizwal)";

        station[18] = "DMV (Dimapur)";

        station[19] = "BBS (Bhubaneshwar), CTC (Cuttack)";

        station[20] = "ASR (Amritsar Jn), LDH (Ludhina), JUC (Jalandhar City), PTA (Patiala)";

        station[21] = "JP (Jaipur Jn), AII (Ajmer Jn), JU (Jodhpur Jn)";

        station[22] = "SGUJ (Siliguri Jn)";

        station[23] = "MAS (Chennai Central), TPJ (Trichy)";

        station[24] = "HYD (Hyderabad), WL (Wrangal)";

        station[25] = "AGTL (Agartala)";

        station[26] = "LJN (Lucknow Jn), CNB (Kanpur Central), BSB (Banaras), ALD (Allahabad Jn), AGC (Agra Cantt), ALJN (Aligarh Jn), GKP (Gorakhpur Jn), JHS (Jhansi Jn), ETW (Etawah Jn), VAR (Varanasi City), MTJ (MAthura Jn)";

        station[27] = "DDN (Dehradun Jn), HW (Haridwar Jn), RSK (Rishikesh), RK (Roorkee), NTL (Nanital)";

        station[28] = "DGR (Durgapur), JVP (Jadavpur), HWH (Howrah Jn), ASN (Asansol Jn)";

        station[29] = "NDLS (New Delhi), ANVT (Anand Vihar Terminal), NZM (Hazrat Nizamuddin Railway Station)";

        state[1] = "Andhra Pradesh";
        state[2] = "Arunanchal Pradesh";
        state[3] = "Assam";
        state[4] = "Bihar";
        state[5] = "Chattisgarh";
        state[6] = "Goa";
        state[7] = "Gujarat";
        state[8] = "Haryana";
        state[9] = "Himanchal Pradesh";
        state[10] = "Jharkhand";
        state[11] = "Karnataka";
        state[12] = "Kerala";
        state[13] = "Madhya Pradesh";
        state[14] = "Maharastra";
        state[15] = "Manipur";
        state[16] = "Meghalaya";
        state[17] = "Mizoram";
        state[18] = "Nagaland";
        state[19] = "Odhisa";
        state[20] = "Punjab";
        state[21] = "Rajashtan";
        state[22] = "Sikkim";
        state[23] = "Tamil Nadu";
        state[24] = "Telangana";
        state[25] = "Tripura";
        state[26] = "Uttar Pradesh";
        state[27] = "Uttarakhand";
        state[28] = "West Bengal";
        state[29] = "Delhi";
    }

    int displayStates()
    {
        int no;
        cout << "\t\t\t\t\tAvailable States:\n";
        for (auto &entry : state)
        {
            cout << entry.first << ". " << entry.second << endl;
        }
        cout << "Enter State Number to Continue: ";
        cin >> no;
        return no;
    }

    void displayStations(int stateNumber)
    {
        cout << "\t\t\t\t\tAvailable Stations in " << state[stateNumber] << ":\n";
        cout << station[stateNumber] << endl;
    }

    void journey()
    {
        details();
        int stateNumber = displayStates();
        displayStations(stateNumber);

        cout << "Enter Starting Station Name: ";
        cin.ignore();
        getline(cin, start);

        stateNumber = displayStates();
        displayStations(stateNumber);
        cout << "Enter Destination Station Name: ";
        getline(cin, end);

        if (start == end)
        {
            cout << "Starting and destination stations cannot be the same.\n";
            return;
        }

        travelclass();
        int fare = calculateFare();

        // Displaying the Booking Details
        cout << "\nUser Details:\n";
        cout << "Name: " << name << endl;
        cout << "Mobile Number: " << mobile << endl;
        cout << "Email Id: " << email << endl;
        cout << "\nJourney Details:\n";
        cout << "From: " << start << "\n";
        cout << "To: " << end << "\n";
        cout << "Date : " << day << "/" << month << "/" << year << endl;
        cout << "Class: " << travel_class << "\n";
        cout << "Fare: ₹" << fare << "\n";
        
        // Save booking to database
        saveBookingToDatabase(fare);
    }

    void travelclass()
    {
        cout << "Select Class:\n1. Economy\n2. Sleeper\n3. AC\nEnter choice: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            travel_class = "Economy";
            break;
        case 2:
            travel_class = "Sleeper";
            break;
        case 3:
            travel_class = "AC";
            break;
        default:
            cout << "Invalid choice. Defaulting to Economy.\n";
            travel_class = "Economy";
            break;
        }
    }

    int calculateFare()
    {
        int baseFare = 500;
        if (travel_class == "Economy")
        {
            baseFare += 200;
        }
        else if (travel_class == "Sleeper")
        {
            baseFare += 400;
        }
        else if (travel_class == "AC")
        {
            baseFare += 1000;
        }
        return baseFare;
    }
    
    void saveBookingToDatabase(int fare) {
        if (!db.isConnected()) {
            cout << "Database connection failed. Booking not saved." << endl;
            return;
        }
        
        // Format date for MySQL (YYYY-MM-DD)
        string journey_date = to_string(year) + "-" + 
                             (month < 10 ? "0" : "") + to_string(month) + "-" + 
                             (day < 10 ? "0" : "") + to_string(day);
        
        // Format query to insert booking data
        string query = "INSERT INTO railway_bookings (user_id, start_station, end_station, journey_date, travel_class, fare) VALUES (" + 
                      to_string(user_id) + ", '" + start + "', '" + end + "', '" + journey_date + "', '" + 
                      travel_class + "', " + to_string(fare) + ")";
        
        if (db.executeQuery(query)) {
            int booking_id = db.getLastInsertId();
            cout << "Railway booking saved with ID: " << booking_id << endl;
        } else {
            cout << "Failed to save railway booking." << endl;
        }
    }
};

class Flight : public User
{
public:
    string travel_class;
    string start;
    string end;
    int klass;
    map<int, string> state;
    map<int, string> cities;

    Flight()
    {
        state[1] = "Andhra Pradesh";
        state[2] = "Arunanchal Pradesh";
        state[3] = "Assam";
        state[4] = "Bihar";
        state[5] = "Chattisgarh";
        state[6] = "Goa";
        state[7] = "Gujarat";
        state[8] = "Haryana";
        state[9] = "Himanchal Pradesh";
        state[10] = "Jharkhand";
        state[11] = "Karnataka";
        state[12] = "Kerala";
        state[13] = "Madhya Pradesh";
        state[14] = "Maharastra";
        state[15] = "Manipur";
        state[16] = "Meghalaya";
        state[17] = "Mizoram";
        state[18] = "Nagaland";
        state[19] = "Odhisa";
        state[20] = "Punjab";
        state[21] = "Rajashtan";
        state[22] = "Sikkim";
        state[23] = "Tamil Nadu";
        state[24] = "Telangana";
        state[25] = "Tripura";
        state[26] = "Uttar Pradesh";
        state[27] = "Uttarakhand";
        state[28] = "West Bengal";
        state[29] = "Delhi";

        cities[1] = "Visakhapatnam (VTZ)";
        cities[2] = "Itanagar (IATA)";
        cities[3] = "Guwahati (GAU)";
        cities[4] = "Patna (PAT), Gaya (GAY), Darbhanga (DBR)";
        cities[5] = "Raipur (RPR)";
        cities[6] = "Dabolim (GOI)";
        cities[7] = "Ahmedabad (AMD), Surat (STV), Vadodara (BDQ)";
        cities[8] = "Chandigarh (IXC)";
        cities[9] = "Shimla (SLV), Dharamshala (DHM)";
        cities[10] = "Ranchi (IXR)";
        cities[11] = "Bangalore (BLR), Mangalore (IXE)";
        cities[12] = "Kochi (COK)";
        cities[13] = "Indore (IDR), Bhopal (BHO), Gwalior (GWL)";
        cities[14] = "Mumbai (BOM), Pune (PNQ), Nagpur (NAG)";
        cities[15] = "Imphal (IMF)";
        cities[16] = "Shillong (SHL)";
        cities[17] = "Aizawl (AJL)";
        cities[18] = "Dimapur (DMU)";
        cities[19] = "Bhubaneswar (BBI)";
        cities[20] = "Amritsar (ATQ), Ludhiana (LUH)";
        cities[21] = "Jaipur (JAI)";
        cities[22] = "Pakyong (PYG)";
        cities[23] = "Chennai (MAA)";
        cities[24] = "Hyderabad (HYD)";
        cities[25] = "Agartala (IXA)";
        cities[26] = "Lucknow (LKO), Varanasi (VNS), Kanpur (KNU), Allahabad (IXD)";
        cities[27] = "Dehradun (DED)";
        cities[28] = "Kolkata (CCU)";
        cities[29] = "Indira Gandhi International (DEL)";
    }

    int displayStates()
    {
        int no;
        cout << "\t\t\t\t\tAvailable States:\n";
        for (auto &entry : state)
        {
            cout << entry.first << ". " << entry.second << endl;
        }
        cout << "Enter State Number to Continue: ";
        cin >> no;
        return no;
    }

    void displayCities(int stateNumber)
    {
        cout << "\t\t\t\t\tAvailable Cities in " << state[stateNumber] << ":\n";
        cout << cities[stateNumber] << endl;
    }

    void bookFlight()
    {
        details();
        int stateNumber = displayStates();
        displayCities(stateNumber);

        cout << "Enter Starting City Name: ";
        cin.ignore();
        getline(cin, start);

        stateNumber = displayStates();
        displayCities(stateNumber);
        cout << "Enter Destination City Name: ";
        getline(cin, end);

        if (start == end)
        {
            cout << "Starting and destination Cities cannot be the same.\n";
            return;
        }

        travelclass();
        int fare = calculateFare();

        // Displaying the Booking Details
        cout << "\nUser Details:\n";
        cout << "Name: " << name << endl;
        cout << "Mobile Number: " << mobile << endl;
        cout << "Email Id: " << email << endl;
        cout << "\nJourney Details:\n";
        cout << "From: " << start << "\n";
        cout << "To: " << end << "\n";
        cout << "Date : " << day << "/" << month << "/" << year << endl;
        cout << "Class: " << travel_class << "\n";
        cout << "Fare: ₹" << fare << "\n";
        
        // Save booking to database
        saveBookingToDatabase(fare);
    }

    void travelclass()
    {
        cout << "Select Class:\n1. Economy\n2. Business\n3. First\nEnter choice: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            travel_class = "Economy";
            break;
        case 2:
            travel_class = "Business";
            break;
        case 3:
            travel_class = "First";
            break;
        default:
            cout << "Invalid choice. Defaulting to Economy.\n";
            travel_class = "Economy";
            break;
        }
    }

    int calculateFare()
    {
        int baseFare = 5000;
        if (travel_class == "Economy")
        {
            baseFare += 2500;
        }
        else if (travel_class == "Business")
        {
            baseFare += 9000;
        }
        else if (travel_class == "First")
        {
            baseFare += 1000;
        }
        return baseFare;
    }
    
    void saveBookingToDatabase(int fare) {
        if (!db.isConnected()) {
            cout << "Database connection failed. Booking not saved." << endl;
            return;
        }
        
        // Format date for MySQL (YYYY-MM-DD)
        string journey_date = to_string(year) + "-" + 
                             (month < 10 ? "0" : "") + to_string(month) + "-" + 
                             (day < 10 ? "0" : "") + to_string(day);
        
        // Format query to insert booking data
        string query = "INSERT INTO flight_bookings (user_id, start_city, end_city, journey_date, travel_class, fare) VALUES (" + 
                      to_string(user_id) + ", '" + start + "', '" + end + "', '" + journey_date + "', '" + 
                      travel_class + "', " + to_string(fare) + ")";
        
        if (db.executeQuery(query)) {
            int booking_id = db.getLastInsertId();
            cout << "Flight booking saved with ID: " << booking_id << endl;
        } else {
            cout << "Failed to save flight booking." << endl;
        }
    }
};

// Bus Class Implementation
class Bus : public User
{
public:
    string travel_class;
    string start;
    string end;
    int klass;
    map<int, string> state;
    map<int, string> cities;

    Bus()
    {
        state[1] = "Andhra Pradesh";
        state[2] = "Arunanchal Pradesh";
        state[3] = "Assam";
        state[4] = "Bihar";
        state[5] = "Chattisgarh";
        state[6] = "Goa";
        state[7] = "Gujarat";
        state[8] = "Haryana";
        state[9] = "Himanchal Pradesh";
        state[10] = "Jharkhand";
        state[11] = "Karnataka";
        state[12] = "Kerala";
        state[13] = "Madhya Pradesh";
        state[14] = "Maharastra";
        state[15] = "Manipur";
        state[16] = "Meghalaya";
        state[17] = "Mizoram";
        state[18] = "Nagaland";
        state[19] = "Odhisa";
        state[20] = "Punjab";
        state[21] = "Rajashtan";
        state[22] = "Sikkim";
        state[23] = "Tamil Nadu";
        state[24] = "Telangana";
        state[25] = "Tripura";
        state[26] = "Uttar Pradesh";
        state[27] = "Uttarakhand";
        state[28] = "West Bengal";
        state[29] = "Delhi";

        cities[1] = "Visakhapatnam, Vijayawada, Tirupati";
        cities[2] = "Itanagar, Tawang";
        cities[3] = "Guwahati, Silchar, Dibrugarh";
        cities[4] = "Patna, Gaya, Muzaffarpur";
        cities[5] = "Raipur, Bilaspur, Bhilai";
        cities[6] = "Panaji, Margao, Vasco da Gama";
        cities[7] = "Ahmedabad, Surat, Vadodara, Rajkot";
        cities[8] = "Gurugram, Faridabad, Hisar";
        cities[9] = "Shimla, Dharamshala, Manali";
        cities[10] = "Ranchi, Jamshedpur, Dhanbad";
        cities[11] = "Bangalore, Mysore, Mangalore";
        cities[12] = "Kochi, Thiruvananthapuram, Kozhikode";
        cities[13] = "Indore, Bhopal, Gwalior";
        cities[14] = "Mumbai, Pune, Nagpur";
        cities[15] = "Imphal";
        cities[16] = "Shillong";
        cities[17] = "Aizawl";
        cities[18] = "Kohima, Dimapur";
        cities[19] = "Bhubaneswar, Cuttack, Puri";
        cities[20] = "Amritsar, Ludhiana, Jalandhar";
        cities[21] = "Jaipur, Jodhpur, Udaipur";
        cities[22] = "Gangtok";
        cities[23] = "Chennai, Coimbatore, Madurai";
        cities[24] = "Hyderabad, Warangal, Karimnagar";
        cities[25] = "Agartala";
        cities[26] = "Lucknow, Kanpur, Varanasi";
        cities[27] = "Dehradun, Haridwar, Rishikesh";
        cities[28] = "Kolkata, Siliguri, Durgapur";
        cities[29] = "Delhi, New Delhi";
    }

    int displayStates()
    {
        int no;
        cout << "\t\t\t\t\tAvailable States:\n";
        for (auto &entry : state)
        {
            cout << entry.first << ". " << entry.second << endl;
        }
        cout << "Enter State Number to Continue: ";
        cin >> no;
        return no;
    }

    void displayCities(int stateNumber)
    {
        cout << "\t\t\t\t\tAvailable Cities in " << state[stateNumber] << ":\n";
        cout << cities[stateNumber] << endl;
    }

    void bookBus()
    {
        details();
        int stateNumber = displayStates();
        displayCities(stateNumber);

        cout << "Enter Starting City Name: ";
        cin.ignore();
        getline(cin, start);

        stateNumber = displayStates();
        displayCities(stateNumber);
        cout << "Enter Destination City Name: ";
        getline(cin, end);

        if (start == end)
        {
            cout << "Starting and destination cities cannot be the same.\n";
            return;
        }

        busClass();
        int fare = calculateFare();

        // Displaying the Booking Details
        cout << "\nUser Details:\n";
        cout << "Name: " << name << endl;
        cout << "Mobile Number: " << mobile << endl;
        cout << "Email Id: " << email << endl;
        cout << "\nJourney Details:\n";
        cout << "From: " << start << "\n";
        cout << "To: " << end << "\n";
        cout << "Date : " << day << "/" << month << "/" << year << endl;
        cout << "Class: " << travel_class << "\n";
        cout << "Fare: ₹" << fare << "\n";
        
        // Save booking to database
        saveBookingToDatabase(fare);
    }

    void busClass()
    {
        cout << "Select Bus Type:\n1. Regular\n2. Deluxe\n3. AC Sleeper\nEnter choice: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            travel_class = "Regular";
            break;
        case 2:
            travel_class = "Deluxe";
            break;
        case 3:
            travel_class = "AC Sleeper";
            break;
        default:
            cout << "Invalid choice. Defaulting to Regular.\n";
            travel_class = "Regular";
            break;
        }
    }

    int calculateFare()
    {
        int baseFare = 300; // Base fare for bus travel
        if (travel_class == "Regular")
        {
            baseFare += 100;
        }
        else if (travel_class == "Deluxe")
        {
            baseFare += 300;
        }
        else if (travel_class == "AC Sleeper")
        {
            baseFare += 600;
        }
        return baseFare;
    }
    
    void saveBookingToDatabase(int fare) {
        if (!db.isConnected()) {
            cout << "Database connection failed. Booking not saved." << endl;
            return;
        }
        
        // Format date for MySQL (YYYY-MM-DD)
        string journey_date = to_string(year) + "-" + 
                             (month < 10 ? "0" : "") + to_string(month) + "-" + 
                             (day < 10 ? "0" : "") + to_string(day);
        
        // Format query to insert booking data
        string query = "INSERT INTO bus_bookings (user_id, start_city, end_city, journey_date, travel_class, fare) VALUES (" + 
                      to_string(user_id) + ", '" + start + "', '" + end + "', '" + journey_date + "', '" + 
                      travel_class + "', " + to_string(fare) + ")";
        
        if (db.executeQuery(query)) {
            int booking_id = db.getLastInsertId();
            cout << "Bus booking saved with ID: " << booking_id << endl;
        } else {
            cout << "Failed to save bus booking." << endl;
        }
    }
};

class Hotel : public User
{
private:
    string location;
    string hotelName;
    string roomType;
    int checkInDate;
    int checkOutDate;
    int numOfRooms;
    int numOfGuests;
    map<int, string> cities;
    map<string, map<string, int>> hotels;

public:
    Hotel()
    {
        cities[1] = "Delhi";
        cities[2] = "Mumbai";
        cities[3] = "Bangalore";
        cities[4] = "Chennai";
        cities[5] = "Kolkata";
        cities[6] = "Hyderabad";
        cities[7] = "Jaipur";
        cities[8] = "Goa";
        cities[9] = "Shimla";
        cities[10] = "Manali";

        // Delhi hotels
        hotels["Delhi"]["Taj Palace"] = 5000;
        hotels["Delhi"]["The Leela"] = 4500;
        hotels["Delhi"]["The Imperial"] = 6000;

        // Mumbai hotels
        hotels["Mumbai"]["Taj Mahal Palace"] = 7000;
        hotels["Mumbai"]["The Oberoi"] = 6500;
        hotels["Mumbai"]["Trident Nariman Point"] = 5500;

        // other cities hotels
        hotels["Bangalore"]["ITC Gardenia"] = 4800;
        hotels["Chennai"]["ITC Grand Chola"] = 5200;
        hotels["Kolkata"]["The Park"] = 4000;
        hotels["Hyderabad"]["Taj Falaknuma Palace"] = 8000;
        hotels["Jaipur"]["Rambagh Palace"] = 9000;
        hotels["Goa"]["Taj Exotica"] = 7500;
        hotels["Shimla"]["The Oberoi Cecil"] = 6000;
        hotels["Manali"]["The Himalayan"] = 5500;
    }

    void displayCities()
    {
        cout << "\n\t\t\t\t\tAvailable Cities for Hotel Booking:\n";
        for (auto &entry : cities)
        {
            cout << entry.first << ". " << entry.second << endl;
        }
    }

    void displayHotels(string city)
    {
        cout << "\n\t\t\t\t\tAvailable Hotels in " << city << ":\n";
        int i = 1;
        for (auto &hotel : hotels[city])
        {
            cout << i << ". " << hotel.first << " - ₹" << hotel.second << " per night" << endl;
            i++;
        }
    }

    void selectRoomType()
    {
        cout << "\nSelect Room Type:\n";
        cout << "1. Standard Room - Base price\n";
        cout << "2. Deluxe Room - Base price + 30%\n";
        cout << "3. Suite - Base price + 70%\n";
        cout << "Enter choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            roomType = "Standard";
            break;
        case 2:
            roomType = "Deluxe";
            break;
        case 3:
            roomType = "Suite";
            break;
        default:
            cout << "Invalid choice. Defaulting to Standard Room.\n";
            roomType = "Standard";
            break;
        }
    }

    int calculateFare(int basePrice)
    {
        int fare = basePrice;

        if (roomType == "Deluxe")
        {
            fare += basePrice * 0.3;
        }
        else if (roomType == "Suite")
        {
            fare += basePrice * 0.7;
        }

        int nights = calculateNights();
        return fare * numOfRooms * nights;
    }

    int calculateNights()
    {
        int checkInDay = checkInDate / 1000000;
        int checkInMonth = (checkInDate / 10000) % 100;
        int checkInYear = checkInDate % 10000;

        int checkOutDay = checkOutDate / 1000000;
        int checkOutMonth = (checkOutDate / 10000) % 100;
        int checkOutYear = checkOutDate % 10000;

        if (checkInMonth == checkOutMonth && checkInYear == checkOutYear)
        {
            return checkOutDay - checkInDay;
        }
        else
        {
            return 1;
        }
    }

    void bookHotel()
    {
        details();

        cout << "\n---Booking Hotel---\n";

        displayCities();
        cout << "Enter city number: ";
        int cityChoice;
        cin >> cityChoice;

        if (cityChoice < 1 || cityChoice > 10)
        {
            cout << "Invalid city choice. Defaulting to Delhi.\n";
            location = "Delhi";
        }
        else
        {
            location = cities[cityChoice];
        }

        displayHotels(location);
        cout << "Enter hotel number: ";
        int hotelChoice;
        cin >> hotelChoice;

        int i = 1;
        for (auto &hotel : hotels[location])
        {
            if (i == hotelChoice)
            {
                hotelName = hotel.first;
                break;
            }
            i++;
        }

        if (hotelName.empty())
        {
            hotelName = hotels[location].begin()->first;
            cout << "Invalid hotel choice. Defaulting to " << hotelName << ".\n";
        }

        cout << "Enter check-in date (DDMMYYYY): ";
        cin >> checkInDate;
        cout << "Enter check-out date (DDMMYYYY): ";
        cin >> checkOutDate;

        cout << "Enter number of rooms: ";
        cin >> numOfRooms;
        cout << "Enter number of guests: ";
        cin >> numOfGuests;

        selectRoomType();

        int basePrice = hotels[location][hotelName];
        int totalFare = calculateFare(basePrice);

        cout << "\n---Hotel Booking Confirmation---\n";
        cout << "Name: " << name << endl;
        cout << "Mobile: " << mobile << endl;
        cout << "Email: " << email << endl;
        cout << "City: " << location << endl;
        cout << "Hotel: " << hotelName << endl;
        cout << "Room Type: " << roomType << endl;
        cout << "Check-in Date: " << checkInDate << endl;
        cout << "Check-out Date: " << checkOutDate << endl;
        cout << "Number of Rooms: " << numOfRooms << endl;
        cout << "Number of Guests: " << numOfGuests << endl;
        cout << "Total Fare: ₹" << totalFare << " for " << calculateNights() << " night(s)" << endl;
        cout << "\nBooking Confirmed! Thank you for choosing Plan My Journey.\n";
        
        // Save booking to database
        saveBookingToDatabase(totalFare);
    }
    
    void saveBookingToDatabase(int totalFare) {
        if (!db.isConnected()) {
            cout << "Database connection failed. Booking not saved." << endl;
            return;
        }
        
        // Format date strings for MySQL (YYYY-MM-DD)
        int checkInDay = checkInDate / 1000000;
        int checkInMonth = (checkInDate / 10000) % 100;
        int checkInYear = checkInDate % 10000;
        
        int checkOutDay = checkOutDate / 1000000;
        int checkOutMonth = (checkOutDate / 10000) % 100;
        int checkOutYear = checkOutDate % 10000;
        
        string check_in_date = to_string(checkInYear) + "-" + 
                              (checkInMonth < 10 ? "0" : "") + to_string(checkInMonth) + "-" + 
                              (checkInDay < 10 ? "0" : "") + to_string(checkInDay);
        
        string check_out_date = to_string(checkOutYear) + "-" + 
                               (checkOutMonth < 10 ? "0" : "") + to_string(checkOutMonth) + "-" + 
                               (checkOutDay < 10 ? "0" : "") + to_string(checkOutDay);
        
        // Format query to insert booking data
        string query = "INSERT INTO hotel_bookings (user_id, location, hotel_name, room_type, check_in_date, check_out_date, "
                      "num_rooms, num_guests, total_fare) VALUES (" + 
                      to_string(user_id) + ", '" + location + "', '" + hotelName + "', '" + roomType + "', '" + 
                      check_in_date + "', '" + check_out_date + "', " + to_string(numOfRooms) + ", " +
                      to_string(numOfGuests) + ", " + to_string(totalFare) + ")";
        
        if (db.executeQuery(query)) {
            int booking_id = db.getLastInsertId();
            cout << "Hotel booking saved with ID: " << booking_id << endl;
        } else {
            cout << "Failed to save hotel booking." << endl;
        }
    }
};

// Admin class for managing bookings
class Admin {
private:
    DBConnection db;
    
public:
    void viewAllBookings() {
        if (!db.isConnected()) {
            cout << "Database connection failed." << endl;
            return;
        }
        
        displayRailwayBookings();
        displayFlightBookings();
        displayBusBookings();
        displayHotelBookings();
    }
    
    void displayRailwayBookings() {
        cout << "\n--- Railway Bookings ---\n";
        string query = "SELECT rb.id, u.name, u.mobile, rb.start_station, rb.end_station, "
                      "rb.journey_date, rb.travel_class, rb.fare, rb.booking_date "
                      "FROM railway_bookings rb "
                      "JOIN users u ON rb.user_id = u.id "
                      "ORDER BY rb.booking_date DESC";
                      
        if (!db.executeQuery(query)) {
            cout << "Failed to retrieve railway bookings." << endl;
            return;
        }
        
        MYSQL_RES* result = db.getResult();
        if (result == NULL) {
            cout << "No railway bookings found." << endl;
            return;
        }
        
        MYSQL_ROW row;
        cout << setw(5) << "ID" << setw(20) << "Name" << setw(15) << "Mobile" 
             << setw(20) << "From" << setw(20) << "To" << setw(15) << "Date" 
             << setw(15) << "Class" << setw(10) << "Fare" << endl;
        cout << string(120, '-') << endl;
        
        while ((row = mysql_fetch_row(result))) {
            cout << setw(5) << row[0] << setw(20) << row[1] << setw(15) << row[2] 
                 << setw(20) << row[3] << setw(20) << row[4] << setw(15) << row[5] 
                 << setw(15) << row[6] << setw(10) << row[7] << endl;
        }
        
        mysql_free_result(result);
    }
    
    void displayFlightBookings() {
        cout << "\n--- Flight Bookings ---\n";
        string query = "SELECT fb.id, u.name, u.mobile, fb.start_city, fb.end_city, "
                      "fb.journey_date, fb.travel_class, fb.fare, fb.booking_date "
                      "FROM flight_bookings fb "
                      "JOIN users u ON fb.user_id = u.id "
                      "ORDER BY fb.booking_date DESC";
                      
        if (!db.executeQuery(query)) {
            cout << "Failed to retrieve flight bookings." << endl;
            return;
        }
        
        MYSQL_RES* result = db.getResult();
        if (result == NULL) {
            cout << "No flight bookings found." << endl;
            return;
        }
        
        MYSQL_ROW row;
        cout << setw(5) << "ID" << setw(20) << "Name" << setw(15) << "Mobile" 
             << setw(20) << "From" << setw(20) << "To" << setw(15) << "Date" 
             << setw(15) << "Class" << setw(10) << "Fare" << endl;
        cout << string(120, '-') << endl;
        
        while ((row = mysql_fetch_row(result))) {
            cout << setw(5) << row[0] << setw(20) << row[1] << setw(15) << row[2] 
                 << setw(20) << row[3] << setw(20) << row[4] << setw(15) << row[5] 
                 << setw(15) << row[6] << setw(10) << row[7] << endl;
        }
        
        mysql_free_result(result);
    }
    
    void displayBusBookings() {
        cout << "\n--- Bus Bookings ---\n";
        string query = "SELECT bb.id, u.name, u.mobile, bb.start_city, bb.end_city, "
                      "bb.journey_date, bb.travel_class, bb.fare, bb.booking_date "
                      "FROM bus_bookings bb "
                      "JOIN users u ON bb.user_id = u.id "
                      "ORDER BY bb.booking_date DESC";
                      
        if (!db.executeQuery(query)) {
            cout << "Failed to retrieve bus bookings." << endl;
            return;
        }
        
        MYSQL_RES* result = db.getResult();
        if (result == NULL) {
            cout << "No bus bookings found." << endl;
            return;
        }
        
        MYSQL_ROW row;
        cout << setw(5) << "ID" << setw(20) << "Name" << setw(15) << "Mobile" 
             << setw(20) << "From" << setw(20) << "To" << setw(15) << "Date" 
             << setw(15) << "Class" << setw(10) << "Fare" << endl;
        cout << string(120, '-') << endl;
        
        while ((row = mysql_fetch_row(result))) {
            cout << setw(5) << row[0] << setw(20) << row[1] << setw(15) << row[2] 
                 << setw(20) << row[3] << setw(20) << row[4] << setw(15) << row[5] 
                 << setw(15) << row[6] << setw(10) << row[7] << endl;
        }
        
        mysql_free_result(result);
    }
    
    void displayHotelBookings() {
        cout << "\n--- Hotel Bookings ---\n";
        string query = "SELECT hb.id, u.name, u.mobile, hb.location, hb.hotel_name, "
                      "hb.room_type, hb.check_in_date, hb.check_out_date, "
                      "hb.num_rooms, hb.num_guests, hb.total_fare, hb.booking_date "
                      "FROM hotel_bookings hb "
                      "JOIN users u ON hb.user_id = u.id "
                      "ORDER BY hb.booking_date DESC";
                      
        if (!db.executeQuery(query)) {
            cout << "Failed to retrieve hotel bookings." << endl;
            return;
        }
        
        MYSQL_RES* result = db.getResult();
        if (result == NULL) {
            cout << "No hotel bookings found." << endl;
            return;
        }
        
        MYSQL_ROW row;
        cout << setw(5) << "ID" << setw(20) << "Name" << setw(15) << "Mobile" 
             << setw(15) << "Location" << setw(20) << "Hotel" << setw(10) << "Room Type" 
             << setw(12) << "Check-in" << setw(12) << "Check-out" 
             << setw(5) << "Rms" << setw(5) << "Gsts" << setw(10) << "Fare" << endl;
        cout << string(130, '-') << endl;
        
        while ((row = mysql_fetch_row(result))) {
            cout << setw(5) << row[0] << setw(20) << row[1] << setw(15) << row[2] 
                 << setw(15) << row[3] << setw(20) << row[4] << setw(10) << row[5] 
                 << setw(12) << row[6] << setw(12) << row[7] 
                 << setw(5) << row[8] << setw(5) << row[9] << setw(10) << row[10] << endl;
        }
        
        mysql_free_result(result);
    }
    
    void searchBookings() {
        string searchTerm;
        cout << "Enter name, mobile number or email to search: ";
        cin.ignore();
        getline(cin, searchTerm);
        
        if (!db.isConnected()) {
            cout << "Database connection failed." << endl;
            return;
        }
        
        // Search and display railway bookings
        string query = "SELECT rb.id, u.name, u.mobile, u.email, rb.start_station, rb.end_station, "
                      "rb.journey_date, rb.travel_class, rb.fare, rb.booking_date "
                      "FROM railway_bookings rb "
                      "JOIN users u ON rb.user_id = u.id "
                      "WHERE u.name LIKE '%" + searchTerm + "%' "
                      "OR u.mobile LIKE '%" + searchTerm + "%' "
                      "OR u.email LIKE '%" + searchTerm + "%'";
                      
        if (db.executeQuery(query)) {
            MYSQL_RES* result = db.getResult();
            if (result) {
                MYSQL_ROW row;
                cout << "\n--- Railway Bookings Found ---\n";
                cout << setw(5) << "ID" << setw(20) << "Name" << setw(15) << "Mobile" << setw(30) << "Email"
                     << setw(20) << "From" << setw(20) << "To" << setw(15) << "Date" 
                     << setw(15) << "Class" << setw(10) << "Fare" << endl;
                cout << string(150, '-') << endl;
                
                while ((row = mysql_fetch_row(result))) {
                    cout << setw(5) << row[0] << setw(20) << row[1] << setw(15) << row[2] << setw(30) << row[3]
                         << setw(20) << row[4] << setw(20) << row[5] << setw(15) << row[6] 
                         << setw(15) << row[7] << setw(10) << row[8] << endl;
                }
                mysql_free_result(result);
            }
        }
        
        // Search and display flight bookings
        query = "SELECT fb.id, u.name, u.mobile, u.email, fb.start_city, fb.end_city, "
               "fb.journey_date, fb.travel_class, fb.fare, fb.booking_date "
               "FROM flight_bookings fb "
               "JOIN users u ON fb.user_id = u.id "
               "WHERE u.name LIKE '%" + searchTerm + "%' "
               "OR u.mobile LIKE '%" + searchTerm + "%' "
               "OR u.email LIKE '%" + searchTerm + "%'";
                      
        if (db.executeQuery(query)) {
            MYSQL_RES* result = db.getResult();
            if (result) {
                MYSQL_ROW row;  {
            MYSQL_RES* result = db.getResult();
            if (result) {
                MYSQL_ROW row;
                cout << "\n--- Flight Bookings Found ---\n";
                cout << setw(5) << "ID" << setw(20) << "Name" << setw(15) << "Mobile" << setw(30) << "Email"
                     << setw(20) << "From" << setw(20) << "To" << setw(15) << "Date" 
                     << setw(15) << "Class" << setw(10) << "Fare" << endl;
                cout << string(150, '-') << endl;
                
                while ((row = mysql_fetch_row(result))) {
                    cout << setw(5) << row[0] << setw(20) << row[1] << setw(15) << row[2] << setw(30) << row[3]
                         << setw(20) << row[4] << setw(20) << row[5] << setw(15) << row[6] 
                         << setw(15) << row[7] << setw(10) << row[8] << endl;
                }
                mysql_free_result(result);
            }
        }
        
        // Search and display bus bookings
        query = "SELECT bb.id, u.name, u.mobile, u.email, bb.start_city, bb.end_city, "
               "bb.journey_date, bb.travel_class, bb.fare, bb.booking_date "
               "FROM bus_bookings bb "
               "JOIN users u ON bb.user_id = u.id "
               "WHERE u.name LIKE '%" + searchTerm + "%' "
               "OR u.mobile LIKE '%" + searchTerm + "%' "
               "OR u.email LIKE '%" + searchTerm + "%'";
                      
        if (db.executeQuery(query)) {
            MYSQL_RES* result = db.getResult();
            if (result) {
                MYSQL_ROW row;
                cout << "\n--- Bus Bookings Found ---\n";
                cout << setw(5) << "ID" << setw(20) << "Name" << setw(15) << "Mobile" << setw(30) << "Email"
                     << setw(20) << "From" << setw(20) << "To" << setw(15) << "Date" 
                     << setw(15) << "Class" << setw(10) << "Fare" << endl;
                cout << string(150, '-') << endl;
                
                while ((row = mysql_fetch_row(result))) {
                    cout << setw(5) << row[0] << setw(20) << row[1] << setw(15) << row[2] << setw(30) << row[3]
                         << setw(20) << row[4] << setw(20) << row[5] << setw(15) << row[6] 
                         << setw(15) << row[7] << setw(10) << row[8] << endl;
                }
                mysql_free_result(result);
            }
        }
        
        // Search and display hotel bookings
        query = "SELECT hb.id, u.name, u.mobile, u.email, hb.location, hb.hotel_name, "
               "hb.room_type, hb.check_in_date, hb.check_out_date, "
               "hb.num_rooms, hb.num_guests, hb.total_fare, hb.booking_date "
               "FROM hotel_bookings hb "
               "JOIN users u ON hb.user_id = u.id "
               "WHERE u.name LIKE '%" + searchTerm + "%' "
               "OR u.mobile LIKE '%" + searchTerm + "%' "
               "OR u.email LIKE '%" + searchTerm + "%'";
                      
        if (db.executeQuery(query)) {
            MYSQL_RES* result = db.getResult();
            if (result) {
                MYSQL_ROW row;
                cout << "\n--- Hotel Bookings Found ---\n";
                cout << setw(5) << "ID" << setw(20) << "Name" << setw(15) << "Mobile" << setw(30) << "Email"
                     << setw(15) << "Location" << setw(20) << "Hotel" << setw(10) << "Room Type" 
                     << setw(12) << "Check-in" << setw(12) << "Check-out" 
                     << setw(5) << "Rms" << setw(5) << "Gsts" << setw(10) << "Fare" << endl;
                cout << string(160, '-') << endl;
                
                while ((row = mysql_fetch_row(result))) {
                    cout << setw(5) << row[0] << setw(20) << row[1] << setw(15) << row[2] << setw(30) << row[3]
                         << setw(15) << row[4] << setw(20) << row[5] << setw(10) << row[6] 
                         << setw(12) << row[7] << setw(12) << row[8] 
                         << setw(5) << row[9] << setw(5) << row[10] << setw(10) << row[11] << endl;
                }
                mysql_free_result(result);
            }
        }
    }
    }
}
    
    void deleteBooking() {
        string bookingType;
        int bookingId;
        
        cout << "Enter booking type (railway/flight/bus/hotel): ";
        cin >> bookingType;
        cout << "Enter booking ID to delete: ";
        cin >> bookingId;
        
        if (!db.isConnected()) {
            cout << "Database connection failed." << endl;
            return;
        }
        
        string tableName;
        if (bookingType == "railway") {
            tableName = "railway_bookings";
        } else if (bookingType == "flight") {
            tableName = "flight_bookings";
        } else if (bookingType == "bus") {
            tableName = "bus_bookings";
        } else if (bookingType == "hotel") {
            tableName = "hotel_bookings";
        } else {
            cout << "Invalid booking type." << endl;
            return;
        }
        
        string query = "DELETE FROM " + tableName + " WHERE id = " + to_string(bookingId);
        
        if (db.executeQuery(query)) {
            cout << "Booking with ID " << bookingId << " deleted successfully." << endl;
        } else {
            cout << "Failed to delete booking." << endl;
        }
    }
};

// Main menu function
void displayMainMenu() {
    cout << "\n\n\t\t\t\t\tWelcome to Plan My Journey🤗\n"
             << endl;
             cout << "-------------------------------------------- Select Booking Type --------------------------------------------" << endl;
             cout << "1. Railway Ticket Booking" << endl;
             cout << "2. Flight Booking" << endl;
             cout << "3. Bus Ticket Booking" << endl;
             cout << "4. Hotel Booking" << endl;
             cout << "5. Cancel Booking" << endl;
             cout << "6. Admin Panel" << endl;
             cout << "7. Exit" << endl;
    cout << "\t\t\t\t\t     Enter your choice: ";
}

// Admin menu function
void displayAdminMenu() {
    cout << "\n\t\t\t\t\tADMIN PANEL\n\n";
    cout << "\t\t\t\t\t     1. View All Bookings\n";
    cout << "\t\t\t\t\t     2. Search Bookings\n";
    cout << "\t\t\t\t\t     3. Delete Booking\n";
    cout << "\t\t\t\t\t     4. Back to Main Menu\n\n";
    cout << "\t\t\t\t\t     Enter your choice: ";
}

// Main function
int main() {
    // Initialize database tables
    initializeDatabase();
    
    int choice;
    
    while (true) {
        displayMainMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                Railway railway;
                railway.journey();
                break;
            }
            case 2: {
                Flight flight;
                flight.bookFlight();
                break;
            }
            case 3: {
                Bus bus;
                bus.bookBus();
                break;
            }
            case 4: {
                Hotel hotel;
                hotel.bookHotel();
                break;
            }
            case 5: {
                DBConnection db;
                cancelBooking(db);
                break;
            }
            case 6: {
                Admin admin;
                int adminChoice;
                
                // Simple admin authentication
                string password;
                cout << "Enter admin password: ";
                cin >> password;
                
                if (password != "admin123") {
                    cout << "Invalid password!" << endl;
                    break;
                }
                
                while (true) {
                    displayAdminMenu();
                    cin >> adminChoice;
                    
                    switch (adminChoice) {
                        case 1:
                            admin.viewAllBookings();
                            break;
                        case 2:
                            admin.searchBookings();
                            break;
                        case 3:
                            admin.deleteBooking();
                            break;
                        case 4:
                            goto exitAdminMenu;
                        default:
                            cout << "Invalid choice. Please try again." << endl;
                    }
                }
                exitAdminMenu:
                break;
            }
            case 7:
                cout << "\nThank you for using Plan My Journey!" << endl;
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
    
    return 0;
}
