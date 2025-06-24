/*
====================================================
  UNIVERSITY TIMETABLE GENERATOR
  Author: Usman ahmed jan
  Description: Generates a university timetable,
  avoiding time and teacher conflicts.
====================================================
*/

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <sstream>

using namespace std;

void printDivider() {
    cout << "---------------------------------------------------------" << endl;
}

void waitForUser() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void showCredits() {
    clearScreen();
    cout << "===============================" << endl;
    cout << "  Project by: Your Name" << endl;
    cout << "  University Timetable Generator" << endl;
    cout << "  Version: 2.0 (Optimized)" << endl;
    cout << "  Last Updated: June 2025" << endl;
    cout << "===============================" << endl;
    waitForUser();
}

void printLineBreak() {
    cout << endl << "========================================" << endl;
}

string toUpper(string str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}





enum Day { MON, TUE, WED, THU, FRI, NUM_DAYS };

string days_str[NUM_DAYS] = { "Mon", "Tue", "Wed", "Thu", "Fri" };



enum TimeSlot { T9AM, T10AM, T11AM, T12PM, T1PM, T2PM, NUM_TIMESLOTS };


string times_str[NUM_TIMESLOTS] = { "9AM", "10AM", "11AM", "12PM", "1PM", "2PM" };




enum RoomType { LECTURE_HALL, LAB, SEMINAR_ROOM, OTHER };


string room_type_str[] = { "Lecture Hall", "Lab", "Seminar Room", "Other" };




bool global_taken_slots[NUM_DAYS][NUM_TIMESLOTS] = { false };

struct Room {
	
	
	
    string id;
    
    
    int capacity;
    
    
    RoomType type;

    Room() : id(""), capacity(0), type(OTHER) {}
    
    
    
    Room(string id, int capacity, RoomType type) : id(id), capacity(capacity), type(type) {}



    void display() const {
        cout << "ID: " << setw(8) << id << " | Capacity: " << setw(5) << capacity << " | Type: " << room_type_str[type] << endl;
    }
};

struct Course {
	
	
    string code;
    
    
    string name;
    
    
    string teacher;
    
    
    int hoursPerWeek;
    
    
    string department;
    
    
    int credits;
    RoomType preferredRoomType;

    Course() : code(""), name(""), teacher(""), hoursPerWeek(0), department(""), credits(0), preferredRoomType(OTHER) {}
    Course(string code, string name, string teacher, int hoursPerWeek, string dept, int credits, RoomType preferredType)
        : code(code), name(name), teacher(teacher), hoursPerWeek(hoursPerWeek),
        
        
          department(dept), credits(credits), preferredRoomType(preferredType) {}

    void display() const {
        cout << "Code: " << setw(8) << code << " | Name: " << setw(25) << name
             << " | Teacher: " << setw(15) << teacher << " | Hours/Week: " << setw(3) << hoursPerWeek
             << " | Dept: " << setw(10) << department << " | Credits: " << setw(2) << credits
             << " | Preferred Room: " << room_type_str[preferredRoomType] << endl;
    }
};

struct ScheduledCourse {
    Course course;
    Day day;
    TimeSlot time;
    Room room;

    void display() const {
        cout << setw(10) << course.name << setw(15) << course.teacher
             << setw(10) << days_str[day] << setw(10) << times_str[time]
             << setw(10) << room.id << endl;
    }
};

class Timetable {
private:
    vector<Course> courses;
    vector<Room> rooms;
    
    
    vector<ScheduledCourse> schedule;
    map<string, set<string>> teacherBusySlots;
    
    
    map<string, set<string>> roomBusySlots;
    
    

    string getSlotIdentifier(Day d, TimeSlot t) const {
        return days_str[d] + "-" + times_str[t];
    }

    void resetScheduleState() {
        for (int d = 0; d < NUM_DAYS; ++d) {
            for (int t = 0; t < NUM_TIMESLOTS; ++t) {
            	
            	
            	
                global_taken_slots[d][t] = false;
            }
        }
        schedule.clear();
        teacherBusySlots.clear();
        roomBusySlots.clear();
    }

    RoomType getRoomTypeFromString(const string& typeStr) {
        string upperTypeStr = toUpper(typeStr);
        
        
        
        if (upperTypeStr == toUpper("LECTURE_HALL") || upperTypeStr == toUpper("LECTURE HALL")) return LECTURE_HALL;
        if (upperTypeStr == toUpper("LAB")) return LAB;
        if (upperTypeStr == toUpper("SEMINAR_ROOM") || upperTypeStr == toUpper("SEMINAR ROOM")) return SEMINAR_ROOM;
        return OTHER;
    }

public:
    void addCourse(string code, string name, string teacher, int hoursPerWeek, string dept, int credits, RoomType preferredType) {
        for (const auto& c : courses) {
            if (c.code == code) {
            	
            	
            	
                cout << "Error: Course with code " << code << " already exists." << endl; return;
            }
        }
        courses.push_back({ code, name, teacher, hoursPerWeek, dept, credits, preferredType });
        cout << "Course '" << name << "' added successfully." << endl;
    }

    void inputCourses() {
    	
        clearScreen(); printLineBreak(); cout << "           ADD NEW COURSE           " << endl; printLineBreak();
        
        string code, name, teacher, department, roomTypeStr; int hours, credits;
        
        
        cout << "Enter Course Code (e.g., CS101): "; cin >> code; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Course Name: "; getline(cin, name);
        
        
        
        cout << "Enter Teacher's Name: "; getline(cin, teacher);
        
        cout << "Enter Hours per Week: "; while (!(cin >> hours) || hours <= 0) { cout << "Invalid. Enter positive hours: "; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); } cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Department: "; getline(cin, department);
        cout << "Enter Credits: "; while (!(cin >> credits) || credits <= 0) { cout << "Invalid. Enter positive credits: "; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); } cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Preferred Room Type (Lecture Hall, Lab, Seminar Room, Other): "; getline(cin, roomTypeStr); RoomType preferredRoomType = getRoomTypeFromString(roomTypeStr);
        addCourse(code, name, teacher, hours, department, credits, preferredRoomType);
        waitForUser();
        
        
        
    }

    void listCourses() const {
        clearScreen(); printLineBreak(); cout << "             REGISTERED COURSES             " << endl; printLineBreak();
        if (courses.empty()) { cout << "No courses registered yet." << endl; waitForUser(); return; }
        cout << "----------------------------------------------------------------------------------------------------------------" << endl;
        cout << setw(10) << "Code" << setw(27) << "Name" << setw(17) << "Teacher" << setw(13) << "Hrs/Week" << setw(12) << "Dept" << setw(9) << "Credits" << setw(18) << "Preferred Room" << endl;
        cout << "----------------------------------------------------------------------------------------------------------------" << endl;
        for (const auto& c : courses) { c.display(); } printDivider(); waitForUser();
    }

    void editCourse() {
        clearScreen(); printLineBreak(); cout << "             EDIT COURSE           " << endl; printLineBreak();
        string code; cout << "Enter code of course to edit: "; cin >> code; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        bool found = false;
        for (auto& c : courses) {
            if (c.code == code) {
                found = true; cout << "Editing Course: " << c.name << endl;
                cout << "New Name (current: " << c.name << "): "; getline(cin, c.name);
                cout << "New Teacher (current: " << c.teacher << "): "; getline(cin, c.teacher);
                cout << "New Hours per Week (current: " << c.hoursPerWeek << "): "; int hours; if (cin >> hours) c.hoursPerWeek = hours; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "New Department (current: " << c.department << "): "; getline(cin, c.department);
                cout << "New Credits (current: " << c.credits << "): "; int credits; if (cin >> credits) c.credits = credits; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "New Preferred Room Type (current: " << room_type_str[c.preferredRoomType] << ") (Lecture Hall, Lab, Seminar Room, Other): "; string roomTypeStr; getline(cin, roomTypeStr); c.preferredRoomType = getRoomTypeFromString(roomTypeStr);
                cout << "Course updated successfully!" << endl; break;
            }
        }
        if (!found) { cout << "Course with code " << code << " not found." << endl; } waitForUser();
    }

    void deleteCourse() {
        clearScreen(); printLineBreak(); cout << "             DELETE COURSE           " << endl; printLineBreak();
        string code; cout << "Enter code of course to delete: "; cin >> code; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        auto it = remove_if(courses.begin(), courses.end(), [&](const Course& c) { return c.code == code; });
        if (it != courses.end()) {
            courses.erase(it, courses.end()); cout << "Course " << code << " deleted successfully." << endl;
            schedule.clear(); cout << "Current schedule cleared due to course deletion. Please regenerate." << endl;
        } else { cout << "Course with code " << code << " not found." << endl; } waitForUser();
    }

    void loadCoursesFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) { cout << "Error: Could not open file " << filename << endl; return; }
        courses.clear(); string line; getline(file, line);
        while (getline(file, line)) {
            stringstream ss(line); string token;
            string code, name, teacher, department, roomTypeStr; int hours, credits;
            getline(ss, token, ','); code = token;
            getline(ss, token, ','); name = token;
            getline(ss, token, ','); teacher = token;
            getline(ss, token, ','); hours = stoi(token);
            getline(ss, token, ','); department = token;
            getline(ss, token, ','); credits = stoi(token);
            getline(ss, token, ','); roomTypeStr = token; RoomType preferredRoomType = getRoomTypeFromString(roomTypeStr);
            addCourse(code, name, teacher, hours, department, credits, preferredRoomType);
        }
        file.close(); cout << "Courses loaded from " << filename << " successfully." << endl; waitForUser();
    }

    void addRoom(string id, int capacity, RoomType type) {
        for (const auto& r : rooms) {
            if (r.id == id) { cout << "Error: Room with ID " << id << " already exists." << endl; return; }
        }
        rooms.push_back({id, capacity, type}); cout << "Room '" << id << "' added successfully." << endl;
    }

    void inputRooms() {
        clearScreen(); printLineBreak(); cout << "           ADD NEW ROOM           " << endl; printLineBreak();
        string id, roomTypeStr; int capacity;
        cout << "Enter Room ID (e.g., LT1): "; cin >> id; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Capacity: "; while (!(cin >> capacity) || capacity <= 0) { cout << "Invalid. Enter positive capacity: "; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); } cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Room Type (Lecture Hall, Lab, Seminar Room, Other): "; getline(cin, roomTypeStr); RoomType type = getRoomTypeFromString(roomTypeStr);
        addRoom(id, capacity, type); waitForUser();
    }

    void listRooms() const {
        clearScreen(); printLineBreak(); cout << "             REGISTERED ROOMS             " << endl; printLineBreak();
        if (rooms.empty()) { cout << "No rooms registered yet." << endl; waitForUser(); return; }
        cout << "--------------------------------------------------------" << endl;
        cout << setw(10) << "ID" << setw(12) << "Capacity" << setw(18) << "Type" << endl;
        cout << "--------------------------------------------------------" << endl;
        for (const auto& r : rooms) { r.display(); } printDivider(); waitForUser();
    }

    void generateSchedule() {
        clearScreen(); printLineBreak(); cout << "           GENERATING TIMETABLE           " << endl; printLineBreak();
        if (courses.empty()) { cout << "No courses to schedule. Please add courses first." << endl; waitForUser(); return; }
        if (rooms.empty()) { cout << "No rooms available. Please add rooms first." << endl; waitForUser(); return; }
        resetScheduleState(); bool success = true;
        for (auto &course : courses) {
            int scheduledHours = 0; cout << "Attempting to schedule: " << course.name << " (" << course.hoursPerWeek << " hours)" << endl;
            for (int d = 0; d < NUM_DAYS && scheduledHours < course.hoursPerWeek; ++d) {
                for (int t = 0; t < NUM_TIMESLOTS && scheduledHours < course.hoursPerWeek; ++t) {
                    string slotIdentifier = getSlotIdentifier((Day)d, (TimeSlot)t);
                    if (teacherBusySlots[course.teacher].count(slotIdentifier)) { cout << "  Conflict: Teacher " << course.teacher << " is busy at " << slotIdentifier << ". Skipping." << endl; continue; }
                    Room selectedRoom; bool roomFound = false;
                    for (auto &room : rooms) {
                        if (room.type == course.preferredRoomType && !roomBusySlots[room.id].count(slotIdentifier)) { selectedRoom = room; roomFound = true; break; }
                    }
                    if (!roomFound) {
                        for (auto &room : rooms) {
                            if (!roomBusySlots[room.id].count(slotIdentifier)) { selectedRoom = room; roomFound = true; break; }
                        }
                    }
                    if (roomFound) {
                        schedule.push_back({course, (Day)d, (TimeSlot)t, selectedRoom});
                        teacherBusySlots[course.teacher].insert(slotIdentifier);
                        roomBusySlots[selectedRoom.id].insert(slotIdentifier);
                        scheduledHours++;
                        cout << "  Scheduled " << course.name << " on " << days_str[d] << " at " << times_str[t] << " in " << selectedRoom.id << endl;
                    } else { cout << "  No available room for " << course.name << " at " << slotIdentifier << ". Trying next slot." << endl; }
                }
            }
            if (scheduledHours < course.hoursPerWeek) {
                cout << "Warning: Could not schedule all " << course.hoursPerWeek << " hours for " << course.name << ". Only " << scheduledHours << " hours scheduled." << endl;
                success = false;
            }
        }
        if (success) { cout << "\nTimetable generation complete. All courses scheduled where possible." << endl; }
        else { cout << "\nTimetable generation completed with some warnings/unassigned hours. Review the schedule." << endl; }
        waitForUser();
    }

    void displaySchedule() const {
        clearScreen(); printLineBreak(); cout << "             GENERATED TIMETABLE             " << endl; printLineBreak();
        if (schedule.empty()) { cout << "Timetable is empty. Please generate it first." << endl; waitForUser(); return; }
        cout << "-------------------------------------------------------------------" << endl;
        
        
        cout << setw(10) << "Course" << setw(15) << "Teacher" << setw(10) << "Day" << setw(10) << "Time" << setw(10) << "Room" << endl;
        cout << "-------------------------------------------------------------------" << endl;
        for (const auto& s : schedule) { s.display(); } printDivider(); waitForUser();
    }

    void saveScheduleToFile(string filename) const {
        clearScreen(); printLineBreak(); cout << "           SAVING TIMETABLE TO FILE           " << endl; printLineBreak();
        
        
        ofstream file(filename);
        
        
        if (!file.is_open()) { cout << "Error: Could not open file " << filename << " for writing." << endl; waitForUser(); return; }
        file << "CourseCode,CourseName,Teacher,Day,Time,RoomID" << endl;
        
        
        
        for (const auto& s : schedule) {
            file << s.course.code << "," << s.course.name << "," << s.course.teacher << ","
            
            
            
                 << days_str[s.day] << "," << times_str[s.time] << "," << s.room.id << endl;
                 
                 
                 
        }
        file.close(); cout << "Schedule saved to " << filename << " successfully." << endl; waitForUser();
        
        
        
    }

    void analyzeSchedule() const {
        clearScreen(); printLineBreak(); cout << "           TIMETABLE ANALYSIS           " << endl; printLineBreak();
        if (schedule.empty()) { cout << "No schedule to analyze. Please generate it first." << endl; waitForUser(); return; }
        map<string, int> teacherWeeklyHours; map<string, map<string, int>> teacherDailyHours;
        cout << "Checking for potential teacher overloads and daily limits:\n"; printDivider();
        for (const auto& s : schedule) { teacherWeeklyHours[s.course.teacher] += 1; teacherDailyHours[s.course.teacher][days_str[s.day]] += 1; }
        for (const auto& entry : teacherWeeklyHours) {
            const string& teacherName = entry.first; int totalHours = entry.second;
            
            
            
            cout << "Teacher: " << setw(15) << teacherName << " | Total Scheduled Hours: " << totalHours << endl;
            if (totalHours > 20) { cout << "  WARNING: " << teacherName << " is scheduled for " << totalHours << " hours this week (high load)." << endl; }
            for (const auto& dailyEntry : teacherDailyHours[teacherName]) {
            	
            	
                const string& dayName = dailyEntry.first; int dailyHours = dailyEntry.second;
                
                
                
                cout << "    - On " << dayName << ": " << dailyHours << " hours" << endl;
                if (dailyHours > 6) { cout << "      WARNING: " << teacherName << " is scheduled for " << dailyHours << " hours on " << dayName << " (high daily load)." << endl; }
            } printDivider();
        }
        cout << "\nChecking for room conflicts:\n"; printDivider();
        map<string, set<string>> actualRoomUsedSlots;
        
        
        
        for (const auto& s : schedule) {
            string slotId = getSlotIdentifier(s.day, s.time);
            
            
            
            if (actualRoomUsedSlots[s.room.id].count(slotId)) { cout << "  CRITICAL CONFLICT: Room " << s.room.id << " is double-booked at " << slotId << "!" << endl; }
            else { actualRoomUsedSlots[s.room.id].insert(slotId); }
        }
        if (actualRoomUsedSlots.empty()) { cout << "No rooms found in schedule." << endl; }
        else {
            bool noConflicts = true;
            for(const auto& entry : actualRoomUsedSlots) { if(entry.second.empty()) noConflicts = false; }
            if(noConflicts) { cout << "No room conflicts detected in the generated schedule." << endl; }
        }
        waitForUser();
    }
};

void printWelcome() {
    clearScreen();
    cout << "======================================================" << endl;
    cout << "       WELCOME TO THE UNIVERSITY TIMETABLE GENERATOR  " << endl;
    cout << "======================================================" << endl;
}

void menu() {
    cout << "\n--------------------- MAIN MENU ----------------------" << endl;
    cout << "1. Course Management" << endl;
    cout << "2. Room Management" << endl;
    cout << "3. Generate Timetable" << endl;
    cout << "4. Display Timetable" << endl;
    cout << "5. Save Timetable to File" << endl;
    cout << "6. Analyze Timetable" << endl;
    cout << "7. Load Courses from File (courses.csv)" << endl;
    cout << "8. Credits" << endl;
    cout << "9. Exit" << endl;
    printDivider();
}

void courseManagementMenu() {
    clearScreen();
    cout << "\n----------------- COURSE MANAGEMENT ------------------" << endl;
    cout << "1. Add New Course" << endl;
    
    
    cout << "2. List All Courses" << endl;
    
    
    cout << "3. Edit Course" << endl;
    
    cout << "4. Delete Course" << endl;
    
    
    cout << "5. Back to Main Menu" << endl;
    printDivider();
}

void roomManagementMenu() {
    clearScreen();
    cout << "\n------------------- ROOM MANAGEMENT ------------------" << endl;
    
    
    cout << "1. Add New Room" << endl;
    
    
    cout << "2. List All Rooms" << endl;
    
    
    cout << "3. Back to Main Menu" << endl;
    printDivider();
}

int main() {
    Timetable t;
    int choice;

    printWelcome();
    waitForUser();

    t.addCourse("CS101", "Data Structures", "Dr. Ali", 3, "CS", 3, LECTURE_HALL);
    
    t.addCourse("CS102", "Object Oriented Prog.", "Dr. Sara", 2, "CS", 3, LECTURE_HALL);
    
    t.addCourse("CS103", "Database Systems", "Prof. Ahmed", 2, "CS", 3, LAB);
    
    t.addCourse("CS104", "Computer Networks", "Dr. Zain", 2, "CS", 3, LECTURE_HALL);
    
    t.addCourse("CS105", "Artificial Intelligence", "Dr. Fatima", 3, "CS", 4, LECTURE_HALL);
    
    cout << "\nInitial courses loaded." << endl;
    
    waitForUser();

    t.addRoom("LT1", 100, LECTURE_HALL);
    t.addRoom("LabA", 30, LAB);
    t.addRoom("SR1", 20, SEMINAR_ROOM);
    cout << "Initial rooms loaded." << endl;
    waitForUser();

    while (true) {
        clearScreen(); menu(); cout << "Enter your choice: ";
        while (!(cin >> choice)) { cout << "Invalid input. Enter number: "; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); }

        switch (choice) {
            case 1: {
                int courseChoice;
                while (true) {
                    clearScreen(); courseManagementMenu(); cout << "Enter course management choice: ";
                    
                    
                    
                    while (!(cin >> courseChoice)) { cout << "Invalid input. Enter number: "; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    switch (courseChoice) {
                    	
                        case 1: t.inputCourses(); break; case 2: t.listCourses(); break; case 3: t.editCourse(); break; case 4: t.deleteCourse(); break; case 5: break;
                        default: cout << "Invalid choice!" << endl; waitForUser(); break;
                    } if (courseChoice == 5) break;
                } break;
            }
            case 2: {
                int roomChoice;
                
                
                
                while (true) {
                	
                	
                	
                	
                    clearScreen(); roomManagementMenu(); cout << "Enter room management choice: ";
                    
                    
                    while (!(cin >> roomChoice)) { cout << "Invalid input. Enter number: "; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    
                    
                    switch (roomChoice) {
                        case 1: t.inputRooms(); break; case 2: t.listRooms(); break; case 3: break;
                        default: cout << "Invalid choice!" << endl; waitForUser(); break;
                    } if (roomChoice == 3) break;
                } break;
            }
            case 3: t.generateSchedule(); break;
            
            
            case 4: t.displaySchedule(); break;
            
            
            
            
            case 5: { string filename; cout << "Enter filename to save (e.g., my_schedule.csv): "; cin.ignore(numeric_limits<streamsize>::max(), '\n'); getline(cin, filename); t.saveScheduleToFile(filename); break; }
            case 6: t.analyzeSchedule(); break;
            
            
            case 7: { string filename = "courses.csv"; cout << "Loading courses from 'courses.csv'..." << endl; t.loadCoursesFromFile(filename); break; }
            case 8: showCredits(); break;
            
            
            case 9: cout << "Exiting Timetable Generator. Goodbye!" << endl; return 0;
            
            
            
            default: cout << "Invalid choice! Please try again." << endl; waitForUser(); break;
        }
    } return 0;
}
