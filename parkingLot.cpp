#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <queue>
#include <unordered_map>

using namespace std;

// Vehicle class
class Vehicle {
public:
    string license_plate;
    string type;

    Vehicle(string plate, string t) {
        license_plate = plate;
        type = t;
    }
};

// Ticket class
class Ticket {
public:
    string license_plate;
    string type;
    time_t entryTime;
    int slotNumber;

    Ticket(string plate, string t, time_t time, int slot) {
        license_plate = plate;
        type = t;
        entryTime = time;
        slotNumber = slot;
    }
};

// Slot class
class Slot {
public:
    int number;
    string type;
    bool isAvailable;
    time_t reservedUntil;
    time_t reservedFrom;

    Slot(int num, string t) {
        number = num;
        type = t;
        isAvailable = true;
        reservedUntil = 0;
        reservedFrom = 0;
    }
};

// Parking lot class
class ParkingLot {
    static const int totalSlots = 6;
    Slot* slots[totalSlots];
    Ticket* tickets[totalSlots];
    unordered_map<string, pair<time_t, time_t>> reservations; 
    queue<Vehicle> general;
    queue<Vehicle> handicapped;
    queue<Vehicle> electric;
public:
    ParkingLot() {
        for (int i = 0; i < totalSlots; i++) {
            if (i < 2) {
                slots[i] = new Slot(i, "handicapped");
            } else if (i < 4) {
                slots[i] = new Slot(i, "electric");
            } else {
                slots[i] = new Slot(i, "general");
            }
            tickets[i] = NULL;
        }
    }

    int findAvailableSlot(string vehicletype, time_t currentTime) {
        for (int i = 0; i < totalSlots; i++) {
            if (slots[i]->type == vehicletype && slots[i]->isAvailable &&
                (currentTime < slots[i]->reservedFrom || currentTime > slots[i]->reservedUntil)) {
                return i;
            }
        }
        return -1;
    }

    bool reserveSlot(Vehicle& veh, time_t entryTime, time_t exitTime) {
        if (reservations.count(veh.license_plate)) {
            cout << "Vehicle already has a reservation." << endl;
            return false;
        }

        int slotNumber = findAvailableSlot(veh.type, entryTime);
        if (slotNumber == -1) {
            cout << "No available slots for reservation at this time.\n";
            return false;
        }

        slots[slotNumber]->reservedFrom = entryTime;
        slots[slotNumber]->reservedUntil = exitTime;
        reservations[veh.license_plate] = {entryTime, exitTime};

        cout << "Reservation made for vehicle " << veh.license_plate << " at slot " << slotNumber
             << " from " << ctime(&entryTime) << " to " << ctime(&exitTime);
        return true;
    }

    void addToWaitlist(Vehicle& veh) {
        if(veh.type=="general") general.push(veh);
        if(veh.type=="handicapped") handicapped.push(veh);
        if(veh.type=="electric") electric.push(veh);
        cout << "Vehicle " << veh.license_plate << " added to the waitlist." << endl;
    }

    bool park(Vehicle& veh, time_t currentTime) {
        int slotNumber = findAvailableSlot(veh.type, currentTime);

        if (slotNumber == -1) {
            cout << "No available slots for vehicle " << veh.license_plate << ". Do you want to be added to the waitlist? (y/n): ";
            char choice;
            cin >> choice;
            if (choice == 'y') {
                addToWaitlist(veh);
            }
            return false;
        }
        slots[slotNumber]->isAvailable = false;
        tickets[slotNumber] = new Ticket(veh.license_plate, veh.type, currentTime, slotNumber);
        cout << "Vehicle " << veh.license_plate << " parked at slot " << slotNumber << endl;
        return true;
    }

    void exit(Vehicle& veh, time_t exitTime) {
        int slotNum;
        cout<<"Enter your Slot: ";
        cin>>slotNum;

        slots[slotNum]->isAvailable = true;
        double duration = difftime(exitTime, tickets[slotNum]->entryTime) / 60;
        double fare = duration * 2;
        delete tickets[slotNum];
        tickets[slotNum] = NULL;

        cout << "Vehicle " << veh.license_plate << " exited from slot " << slotNum
             << ". Duration: " << duration << " minutes. Fare: " << fare << " rs.\n";
        if(veh.type=="general"){
        if (!general.empty()) {
            Vehicle nextVeh = general.front();
            general.pop();
            cout << "Assigned slot to general vehicle " << nextVeh.license_plate <<endl;
            park(nextVeh, exitTime);
        }
    }
    if(veh.type=="handicapped"){
        if (!handicapped.empty()) {
            Vehicle nextVeh = handicapped.front();
            handicapped.pop();
            cout << "Assigned slot to handicapped vehicle " << nextVeh.license_plate <<endl;
            park(nextVeh, exitTime);
        }
    }
    if(veh.type=="electric"){
        if (!electric.empty()) {
            Vehicle nextVeh = electric.front();
            electric.pop();
            cout << "Assigned slot to electric vehicle " << nextVeh.license_plate <<endl;
            park(nextVeh, exitTime);
        }
    }
    }
};

int main() {
    ParkingLot lot;
    time_t currentTime = time(0);
    time_t reservationStart1 = currentTime + 3600;
    time_t reservationEnd1 = reservationStart1 + 7200;
    time_t reservationStart2 = currentTime + 1800;
    time_t reservationEnd2 = reservationStart2 + 5400;

    Vehicle v1("ABCD1", "handicapped");
    Vehicle v2("ABCD2", "electric");
    Vehicle v3("ABCD3", "general");
    Vehicle v4("ABCD4", "general");
    Vehicle v5("ABCD5", "handicapped");
    Vehicle v6("ABCD6", "handicapped");

    lot.reserveSlot(v1, reservationStart1, reservationEnd1);
    lot.reserveSlot(v2, reservationStart2, reservationEnd2);

    lot.park(v1, currentTime);
    lot.park(v2, currentTime);
    lot.park(v3, currentTime);
    lot.park(v4, currentTime);
    lot.park(v5, currentTime);
    lot.park(v6, currentTime); 

    lot.exit(v1, time(0) + 3600);
    lot.exit(v2, time(0) + 1800);

    return 0;
}
