# Plan My Journey 🚂✈️🚌🏨

A comprehensive travel booking system built in C++ with MySQL database integration. This console-based application allows users to book railway tickets, flights, bus tickets, and hotel accommodations with full database persistence.

## Features

### 🎫 Booking Services
- **Railway Booking**: Book train tickets across Indian states with multiple travel classes
- **Flight Booking**: Reserve flights between major Indian cities with different service classes  
- **Bus Booking**: Book bus tickets with various bus types (Regular, Deluxe, AC Sleeper)
- **Hotel Booking**: Reserve hotel rooms with different room types and pricing

### 👤 User Management
- User registration with personal details
- Booking history tracking
- Cancellation system with refund processing

### 🔧 Admin Panel
- View all bookings across all services
- Search bookings by user details
- Delete bookings from the system
- Comprehensive booking management

## Prerequisites

### System Requirements
- C++ compiler (GCC 7.0+ or equivalent)
- MySQL Server (5.7+ or 8.0+)
- MySQL C++ Connector library

### Dependencies
- `mysql/mysql.h` - MySQL C++ Connector
- Standard C++ libraries (`iostream`, `string`, `map`, `vector`, etc.)

## Installation

### 1. Install MySQL Server
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install mysql-server

# CentOS/RHEL
sudo yum install mysql-server
```

### 2. Install MySQL C++ Connector
```bash
# Ubuntu/Debian
sudo apt install libmysqlcppconn-dev

# CentOS/RHEL
sudo yum install mysql-connector-c++-devel
```

### 3. Database Setup
1. Start MySQL service:
```bash
sudo systemctl start mysql
sudo systemctl enable mysql
```

2. Create database and user:
```sql
mysql -u root -p

CREATE DATABASE travel_booking_system;
CREATE USER 'travel_user'@'localhost' IDENTIFIED BY 'your_password';
GRANT ALL PRIVILEGES ON travel_booking_system.* TO 'travel_user'@'localhost';
FLUSH PRIVILEGES;
```

### 4. Configure Database Connection
Update the database connection details in the source code:
```cpp
const char* HOST = "localhost";
const char* USER = "travel_user";        // Update with your username
const char* PASSWORD = "your_password";   // Update with your password
const char* DATABASE = "travel_booking_system";
```

## Compilation

### Using g++
```bash
g++ -o plan_my_journey main.cpp -lmysqlclient
```

### Using Makefile (recommended)
Create a `Makefile`:
```makefile
CXX = g++
CXXFLAGS = -std=c++11 -Wall
LIBS = -lmysqlclient
TARGET = plan_my_journey
SOURCE = main.cpp

\$(TARGET): \$(SOURCE)
	\$(CXX) \$(CXXFLAGS) -o \$(TARGET) \$(SOURCE) \$(LIBS)

clean:
	rm -f \$(TARGET)

.PHONY: clean
```

Then compile:
```bash
make
```

## Usage

### Running the Application
```bash
./plan_my_journey
```

### Main Menu Options
1. **Railway Ticket Booking** - Book train tickets
2. **Flight Booking** - Reserve flight tickets
3. **Bus Ticket Booking** - Book bus tickets
4. **Hotel Booking** - Reserve hotel rooms
5. **Cancel Booking** - Cancel existing bookings
6. **Admin Panel** - Administrative functions
7. **Exit** - Close the application

### Admin Access
- Default admin password: `admin123`
- Change this in the source code for production use

## Database Schema

The application automatically creates the following tables:

### Users Table
- `id` (Primary Key)
- `name` (VARCHAR)
- `mobile` (BIGINT)
- `email` (VARCHAR)

### Railway Bookings
- `id`, `user_id`, `start_station`, `end_station`
- `journey_date`, `travel_class`, `fare`, `booking_date`

### Flight Bookings
- `id`, `user_id`, `start_city`, `end_city`
- `journey_date`, `travel_class`, `fare`, `booking_date`

### Bus Bookings
- `id`, `user_id`, `start_city`, `end_city`
- `journey_date`, `travel_class`, `fare`, `booking_date`

### Hotel Bookings
- `id`, `user_id`, `location`, `hotel_name`, `room_type`
- `check_in_date`, `check_out_date`, `num_rooms`, `num_guests`
- `total_fare`, `booking_date`

## Features in Detail

### Railway Booking
- Covers 29 Indian states with major railway stations
- Travel classes: Economy, Sleeper, AC
- Dynamic fare calculation based on class selection

### Flight Booking
- Major airports across Indian cities
- Travel classes: Economy, Business, First
- Premium pricing for air travel

### Bus Booking
- Inter-city bus travel
- Bus types: Regular, Deluxe, AC Sleeper
- Affordable pricing structure

### Hotel Booking
- 10 major Indian cities
- Premium hotels with different room types
- Multi-night stay calculations
- Room and guest capacity management

## Security Considerations

⚠️ **Important Security Notes:**
- Database credentials are hardcoded - move to environment variables for production
- Admin password is hardcoded - implement proper authentication
- SQL queries use string concatenation - consider prepared statements
- Input validation should be enhanced for production use

## Troubleshooting

### Common Issues

1. **MySQL Connection Failed**
   - Verify MySQL service is running
   - Check database credentials
   - Ensure database exists

2. **Compilation Errors**
   - Install MySQL C++ connector development libraries
   - Check compiler version compatibility

3. **Runtime Errors**
   - Verify database permissions
   - Check table creation success
   - Monitor MySQL error logs

### Debug Mode
Add debug output by modifying the `DBConnection` class to print SQL queries before execution.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add appropriate error handling
5. Test thoroughly
6. Submit a pull request

## Future Enhancements

- [ ] Web-based interface
- [ ] Payment gateway integration
- [ ] Email notifications
- [ ] Booking confirmation PDFs
- [ ] Real-time availability checking
- [ ] User authentication system
- [ ] Mobile application
- [ ] API endpoints for third-party integration

## Support

For support and questions:
- Create an issue in the repository
- Check the troubleshooting section
- Review MySQL and C++ documentation

---

**Note**: This is a educational/demonstration project. For production use, implement proper security measures, input validation, and error handling.
```

