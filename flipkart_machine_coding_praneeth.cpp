#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <queue>
#include <algorithm>

using namespace std;

class IDoctor
{
public:
    virtual string getName() const = 0;
    virtual string getSpeciality() const = 0;
    virtual unordered_set<string> &getAvailableSlots() = 0;
    virtual unordered_map<int, string> &getAppointments() = 0;
    virtual ~IDoctor() = default;
};

class IPatient
{
public:
    virtual string getName() const = 0;
    virtual unordered_map<int, string> &getAppointments() = 0;
    virtual ~IPatient() = default;
};

class Doctor : public IDoctor
{
public:
    string name;
    string speciality;
    unordered_set<string> availableSlots;
    unordered_map<int, string> appointments;

    Doctor(string name, string speciality) : name(name), speciality(speciality) {}

    string getName() const override
    {
        return name;
    }

    string getSpeciality() const override
    {
        return speciality;
    }

    unordered_set<string> &getAvailableSlots() override
    {
        return availableSlots;
    }

    unordered_map<int, string> &getAppointments() override
    {
        return appointments;
    }
};

class Patient : public IPatient
{
public:
    string name;
    unordered_map<int, string> appointments;

    Patient(string name) : name(name) {}

    string getName() const override
    {
        return name;
    }

    unordered_map<int, string> &getAppointments() override
    {
        return appointments;
    }
};

class EntityFactory
{
public:
    static IDoctor *createDoctor(const string &name, const string &speciality)
    {
        return new Doctor(name, speciality);
    }

    static IPatient *createPatient(const string &name)
    {
        return new Patient(name);
    }
};

class IObserver
{
public:
    virtual void update(const string &slot) = 0;
    virtual ~IObserver() = default;
};

class Waitlist : public IObserver
{
private:
    queue<string> patientQueue;

public:
    void addPatient(const string &patientName)
    {
        patientQueue.push(patientName);
    }

    void update(const string &slot) override
    {
        if (!patientQueue.empty())
        {
            string nextPatient = patientQueue.front();
            patientQueue.pop();
            cout << "Notifying " << nextPatient << " for slot " << slot << endl;
        }
    }

    bool isEmpty() const
    {
        return patientQueue.empty();
    }

    string getNextPatient()
    {
        string nextPatient = patientQueue.front();
        patientQueue.pop();
        return nextPatient;
    }
};

class IDisplayStrategy
{
public:
    virtual void display(const unordered_map<string, IDoctor *> &doctors, const string &speciality) = 0;
    virtual ~IDisplayStrategy() = default;
};

class DisplayByStartTime : public IDisplayStrategy
{
public:
    void display(const unordered_map<string, IDoctor *> &doctors, const string &speciality) override
    {
        vector<pair<string, string>> slots;
        for (const auto &[name, doctor] : doctors)
        {
            if (doctor->getSpeciality() == speciality)
            {
                for (const string &slot : doctor->getAvailableSlots())
                {
                    slots.emplace_back(slot, name);
                }
            }
        }
        sort(slots.begin(), slots.end(), [](const pair<string, string> &a, const pair<string, string> &b)
             {
            int startHourA = stoi(a.first.substr(0, a.first.find(':')));
            int startMinuteA = stoi(a.first.substr(a.first.find(':') + 1, a.first.find('-') - a.first.find(':') - 1));
            int startHourB = stoi(b.first.substr(0, b.first.find(':')));
            int startMinuteB = stoi(b.first.substr(b.first.find(':') + 1, b.first.find('-') - b.first.find(':') - 1));
            return (startHourA * 60 + startMinuteA) < (startHourB * 60 + startMinuteB); });
        for (const auto &[slot, name] : slots)
        {
            cout << "Dr." << name << ": (" << slot << ")" << endl;
        }
    }
};

class AppointmentSystem
{
private:
    unordered_map<string, IDoctor *> doctors;
    unordered_map<string, IPatient *> patients;
    unordered_map<string, Waitlist *> waitlists;
    unordered_map<int, tuple<string, string, string>> bookedSlots;
    IDisplayStrategy *displayStrategy;
    int bookingCounter = 0;

    bool isValidSlot(const string &slot)
    {
        size_t dashPos = slot.find('-');
        size_t colonPos1 = slot.find(':');
        size_t colonPos2 = slot.find(':', dashPos);

        if (dashPos == string::npos || colonPos1 == string::npos || colonPos2 == string::npos)
        {
            return false;
        }

        int startHour = stoi(slot.substr(0, colonPos1));
        int startMinute = stoi(slot.substr(colonPos1 + 1, dashPos - colonPos1 - 1));
        int endHour = stoi(slot.substr(dashPos + 1, colonPos2 - dashPos - 1));
        int endMinute = stoi(slot.substr(colonPos2 + 1));

        int duration = (endHour * 60 + endMinute) - (startHour * 60 + startMinute);
        return duration == 30;
    }

    int generateBookingId()
    {
        return ++bookingCounter;
    }

public:
    AppointmentSystem() : displayStrategy(nullptr) {}

    ~AppointmentSystem()
    {
        for (auto &[name, doctor] : doctors)
        {
            delete doctor;
        }
        for (auto &[name, patient] : patients)
        {
            delete patient;
        }
        for (auto &[slot, waitlist] : waitlists)
        {
            delete waitlist;
        }
    }

    void setDisplayStrategy(IDisplayStrategy *strategy)
    {
        displayStrategy = strategy;
    }

    void registerDoctor(const string &name, const string &speciality)
    {
        cout << "Registering Doctor: " << name << ", Speciality: " << speciality << endl;
        if (doctors.find(name) == doctors.end())
        {
            doctors[name] = EntityFactory::createDoctor(name, speciality);
            cout << "Welcome Dr. " << name << " !!" << endl;
        }
        else
        {
            cout << "Doctor already registered." << endl;
        }
        cout << endl;
    }

    void markDoctorAvailability(const string &name, const vector<string> &slots)
    {
        cout << "Marking availability for Dr. " << name << " with slots: ";
        for (const string &slot : slots)
        {
            cout << slot << " ";
        }
        cout << endl;

        if (doctors.find(name) != doctors.end())
        {
            for (const string &slot : slots)
            {
                if (isValidSlot(slot))
                {
                    doctors[name]->getAvailableSlots().insert(slot);
                }
                else
                {
                    cout << "Sorry Dr. " << name << ", slots are 30 mins only. Invalid slot: " << slot << endl;
                }
            }
            cout << "Done Doc!" << endl;
        }
        else
        {
            cout << "Doctor not found." << endl;
        }
        cout << endl;
    }

    void registerPatient(const string &name)
    {
        cout << "Registering Patient: " << name << endl;
        if (patients.find(name) == patients.end())
        {
            patients[name] = EntityFactory::createPatient(name);
            cout << "Registration successful" << endl;
        }
        else
        {
            cout << "Patient already registered." << endl;
        }
        cout << endl;
    }

    void showAvailableSlotsBySpeciality(const string &speciality)
    {
        cout << "Showing available slots for speciality: " << speciality << endl;
        if (displayStrategy)
        {
            displayStrategy->display(doctors, speciality);
        }
        else
        {
            cout << "No display strategy set." << endl;
        }
        cout << endl;
    }

    int bookAppointment(const string &patientName, const string &doctorName, const string &slot)
    {
        cout << "Booking appointment for Patient: " << patientName << " with Dr. " << doctorName << " for slot: " << slot << endl;
        if (patients.find(patientName) != patients.end() && doctors.find(doctorName) != doctors.end())
        {
            for (const auto &[bookingId, bookedSlot] : patients[patientName]->getAppointments())
            {
                if (bookedSlot == slot)
                {
                    cout << "Patient already has an appointment in the same slot." << endl;
                    return -1;
                }
            }

            if (doctors[doctorName]->getAvailableSlots().find(slot) != doctors[doctorName]->getAvailableSlots().end())
            {
                int bookingId = generateBookingId();
                bookedSlots[bookingId] = make_tuple(slot, patientName, doctorName);
                patients[patientName]->getAppointments()[bookingId] = slot;
                doctors[doctorName]->getAppointments()[bookingId] = slot;
                doctors[doctorName]->getAvailableSlots().erase(slot);
                cout << "Booked. Booking id: " << bookingId << endl;
                return bookingId;
            }
            else
            {
                if (waitlists.find(slot) == waitlists.end())
                {
                    waitlists[slot] = new Waitlist();
                }
                waitlists[slot]->addPatient(patientName);
                cout << "Added patient to waitlist for slot " << slot << endl;
                return -1;
            }
        }
        else
        {
            cout << "Patient or Doctor not found." << endl;
            return -1;
        }
        cout << endl;
    }

    void cancelBooking(int bookingId)
    {
        cout << "Cancelling booking with ID: " << bookingId << endl;
        if (bookedSlots.find(bookingId) != bookedSlots.end())
        {
            string slot = get<0>(bookedSlots[bookingId]);
            string patientName = get<1>(bookedSlots[bookingId]);
            string doctorName = get<2>(bookedSlots[bookingId]);
            bookedSlots.erase(bookingId);
            patients[patientName]->getAppointments().erase(bookingId);
            doctors[doctorName]->getAppointments().erase(bookingId);
            doctors[doctorName]->getAvailableSlots().insert(slot);

            cout << "Booking Cancelled" << endl;

            if (waitlists.find(slot) != waitlists.end() && !waitlists[slot]->isEmpty())
            {
                string nextPatient = waitlists[slot]->getNextPatient();
                cout << "Notifying " << nextPatient << " for slot " << slot << endl;
                int newBookingId = bookAppointment(nextPatient, doctorName, slot);
                if (newBookingId != -1)
                {
                    cout << "Appointment booked for " << nextPatient << " with booking ID " << newBookingId << endl;
                }
            }
        }
        else
        {
            cout << "No booking found for the given booking ID." << endl;
        }
        cout << endl;
    }

    void showPatientAppointments(const string &patientName)
    {
        cout << "Showing appointments for Patient: " << patientName << endl;
        if (patients.find(patientName) != patients.end())
        {
            cout << "Appointments for " << patientName << ":" << endl;
            for (const auto &[bookingId, slot] : patients[patientName]->getAppointments())
            {
                cout << "Booking ID: " << bookingId << ", Slot: " << slot << endl;
            }
        }
        else
        {
            cout << "Patient not found." << endl;
        }
        cout << endl;
    }

    void showDoctorAppointments(const string &doctorName)
    {
        cout << "Showing appointments for Dr. " << doctorName << endl;
        if (doctors.find(doctorName) != doctors.end())
        {
            cout << "Appointments for Dr. " << doctorName << ":" << endl;
            for (const auto &[bookingId, slot] : doctors[doctorName]->getAppointments())
            {
                cout << "Booking ID: " << bookingId << ", Slot: " << slot << endl;
            }
        }
        else
        {
            cout << "Doctor not found." << endl;
        }
        cout << endl;
    }
};

int main()
{
    AppointmentSystem system;

    cout << endl;

    system.registerDoctor("Mahesh", "Cardiologist");
    system.markDoctorAvailability("Mahesh", {"10:00-10:30", "10:30-11:00", "11:00-11:30", "11:30-12:00"});

    system.registerDoctor("devansh", "Cardiologist");
    system.markDoctorAvailability("devansh", {"9:00-9:30", "9:30-10:00"});

    system.registerDoctor("raj", "Ortho");
    system.markDoctorAvailability("raj", {"8:30-9:00", "9:00-9:30"});

    system.setDisplayStrategy(new DisplayByStartTime());
    system.showAvailableSlotsBySpeciality("Cardiologist");

    system.registerPatient("Sneha");
    int bookingIdA = system.bookAppointment("Sneha", "devansh", "9:00-9:30");

    system.registerPatient("praneeth");
    int Praneeth = system.bookAppointment("praneeth", "devansh", "9:00-9:30");

    system.cancelBooking(bookingIdA);

    int Praneeth2 = system.bookAppointment("praneeth", "raj", "9:00-9:30");
    /*
    system.registerDoctor("Curious", "Cardiologist");
    system.markDoctorAvailability("Curious", {"9:30-10:30"});

    system.markDoctorAvailability("Curious", {"9:30-10:00", "12:30-13:00", "16:00-16:30"});
    system.registerDoctor("Dreadful", "Dermatologist");
    system.markDoctorAvailability("Dreadful", {"9:30-10:00", "12:30-13:00", "16:00-16:30"});

    system.setDisplayStrategy(new DisplayByStartTime());
    system.showAvailableSlotsBySpeciality("Cardiologist");

    system.registerPatient("PatientA");
    int bookingIdA = system.bookAppointment("PatientA", "Curious", "12:30-13:00");

    int bookingIdA2 = system.bookAppointment("PatientA", "Dreadful", "12:30-13:00");

    system.registerPatient("PatientC");
    int bookingIdC = system.bookAppointment("PatientC", "Curious", "12:30-13:00");

    system.showAvailableSlotsBySpeciality("Cardiologist");

    system.cancelBooking(bookingIdA);

    system.showAvailableSlotsBySpeciality("Cardiologist");

    system.registerPatient("PatientB");
    system.bookAppointment("PatientB", "Curious", "12:30-13:00");

    system.registerDoctor("Daring", "Dermatologist");
    system.markDoctorAvailability("Daring", {"11:30-12:00", "14:00-14:30"});

    system.setDisplayStrategy(new DisplayByStartTime());
    system.showAvailableSlotsBySpeciality("Dermatologist");

    system.showPatientAppointments("PatientC");

    system.showDoctorAppointments("Curious");
    */
    return 0;
}