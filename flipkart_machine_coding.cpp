#include <bits/stdc++.h>
using namespace std;

class Slot
{
public:
    string startTime;
    string endTime;
    bool isCurrSlotAvailable;
    queue<string> slotWaitListQ;
    Slot(string startTime, string endTime)
    {
        this->startTime = startTime;
        this->endTime = endTime;
        this->isCurrSlotAvailable = true;
    }
};

class Doctor
{
public:
    string doctorName;
    string doctorSpecialization;
    vector<Slot *> doctorSlots;
    int doctorAppointmentCount;
    Doctor(string doctorName, string doctorSpecialization)
    {
        this->doctorName = doctorName;
        this->doctorSpecialization = doctorSpecialization;
        this->doctorAppointmentCount = 0;
    }
    bool isValid(string startTime, string endTime)
    {
        int startHour, startMin, endHour, endMin;
        startHour = stoi(startTime.substr(0, startTime.find(":")));
        startMin = stoi(startTime.substr(startTime.find(":") + 1));
        endHour = stoi(endTime.substr(0, endTime.find(":")));
        endMin = stoi(endTime.substr(endTime.find(":") + 1));
        if (startHour < 0 || startHour > 23 || endHour < 0 || endHour > 23 || startMin < 0 || startMin > 59 || endMin < 0 || endMin > 59)
        {
            return false;
        }
        if (startHour > endHour || (startHour == endHour && startMin >= endMin))
        {
            return false;
        }
        if (startMin % 30 != 0 || endMin % 30 != 0)
        {
            return false;
        }
        int startTimeVal = (startHour * 60) + startMin;
        int endTimeVal = (endHour * 60) + endMin;
        if ((endTimeVal - startTimeVal) != 30)
        {
            return false;
        }
        return true;
    }
    int markAvailability(vector<string> times)
    {
        int findDashPos, invalidTimeSlotCount;
        string currStartTime, currEndTime;
        invalidTimeSlotCount = 0;
        for (int i = 0; i < times.size(); i++)
        {
            auto it = find(times[i].begin(), times[i].end(), '-');
            findDashPos = distance(times[i].begin(), it);
            currStartTime = times[i].substr(0, findDashPos);
            currEndTime = times[i].substr(findDashPos + 1);
            if (isValid(currStartTime, currEndTime))
            {
                doctorSlots.push_back(new Slot(currStartTime, currEndTime));
            }
            else
            {
                invalidTimeSlotCount++;
            }
        }
        return invalidTimeSlotCount;
    }

    bool isSlotAvailable(string startTime)
    {
        for (int i = 0; i < doctorSlots.size(); i++)
        {
            if (doctorSlots[i]->startTime == startTime)
            {
                return doctorSlots[i]->isCurrSlotAvailable;
            }
        }
        return false;
    }

    void bookSlot(string startTime, string patientName)
    {
        for (int i = 0; i < doctorSlots.size(); i++)
        {
            if (doctorSlots[i]->startTime == startTime)
            {
                if (doctorSlots[i]->isCurrSlotAvailable)
                {
                    doctorSlots[i]->isCurrSlotAvailable = false;
                    doctorAppointmentCount++;
                }
                else
                {
                    doctorSlots[i]->slotWaitListQ.push(patientName);
                }
                break;
            }
        }
    }

    void cancelSlot(string startTime)
    {
        for (int i = 0; i < doctorSlots.size(); i++)
        {
            if (doctorSlots[i]->startTime == startTime)
            {
                if (!doctorSlots[i]->isCurrSlotAvailable)
                {
                    doctorSlots[i]->isCurrSlotAvailable = true;
                    doctorAppointmentCount--;
                    if (!doctorSlots[i]->slotWaitListQ.empty())
                    {
                        string patientName = doctorSlots[i]->slotWaitListQ.front();
                        doctorSlots[i]->slotWaitListQ.pop();
                        bookSlot(startTime, patientName); // Book the slot for the patient in the waitlist
                    }
                }
                else
                {
                    cout << "Slot is already available\n";
                }
                break;
            }
        }
    }
};

class Patient
{
public:
    string patientName;
    vector<pair<string, string>> patientAppointments;
    Patient(string patientName)
    {
        this->patientName = patientName;
    }

    void bookAppointment(string doctorName, string time)
    {
        patientAppointments.push_back({doctorName, time});
    }

    void cancelAppointment(string doctorName, string time)
    {
        for (int i = 0; i < patientAppointments.size(); i++)
        {
            if (patientAppointments[i].first == doctorName && patientAppointments[i].second == time)
            {
                patientAppointments.erase(patientAppointments.begin() + i);
                break;
            }
        }
    }
};

class FlipCare
{
private:
    static FlipCare *instance;
    unordered_map<string, Doctor *> doctors;
    unordered_map<string, Patient *> patients;
    unordered_map<int, vector<string>> bookingIdToPatientDoctorMap;
    int bookingIdCounter;

    FlipCare()
    {
        bookingIdCounter = 1;
    }

public:
    static FlipCare *getInstance()
    {
        if (!instance)
        {
            instance = new FlipCare();
        }
        return instance;
    }

    void registerDoctor(string doctorName, string doctorSpecialization)
    {
        if (doctors.find(doctorName) == doctors.end())
        {
            doctors[doctorName] = new Doctor(doctorName, doctorSpecialization);
            cout << "Welcome Dr. " << doctorName << " !!\n";
        }
        else
        {
            cout << "Doctor already exists\n";
        }
    }

    void markDoctorAvailability(string doctorName, vector<string> times)
    {
        if (doctors.find(doctorName) != doctors.end())
        {
            int invalidTimeSlotCount = doctors[doctorName]->markAvailability(times);
            if (invalidTimeSlotCount == 0)
            {
                cout << "Done Doc!\n";
            }
            else
            {
                cout << "Sorry Dr. " << doctorName << " slots are 30 mins only\n";
                cout << "There are " << invalidTimeSlotCount << " invalid slots out of " << times.size() << " slots\n";
            }
        }
        else
        {
            cout << "Doctor not found\n";
        }
    }

    void registerPatient(string patientName)
    {
        if (patients.find(patientName) == patients.end())
        {
            patients[patientName] = new Patient(patientName);
            cout << "Registration successful\n";
        }
        else
        {
            cout << "Patient already exists\n";
        }
    }

    void bookAppointment(string doctorName, string patientName, string time)
    {
        if (patients.find(patientName) == patients.end())
        {
            cout << "Patient not found\n";
            return;
        }
        if (doctors.find(doctorName) == doctors.end())
        {
            cout << "Doctor not found\n";
            return;
        }
        doctors[doctorName]->bookSlot(time, patientName);
        patients[patientName]->bookAppointment(doctorName, time);
        bookingIdToPatientDoctorMap.insert({bookingIdCounter, {patientName, doctorName, time}});
        cout << "Booked. Booking id: " << bookingIdCounter++ << "\n";
    }

    void cancelBookingId(int bookingId)
    {
        if (bookingIdToPatientDoctorMap.find(bookingId) == bookingIdToPatientDoctorMap.end())
        {
            cout << "Booking not found\n";
            return;
        }
        string patientName = bookingIdToPatientDoctorMap[bookingId][0];
        string doctorName = bookingIdToPatientDoctorMap[bookingId][1];
        string time = bookingIdToPatientDoctorMap[bookingId][2];
        doctors[doctorName]->cancelSlot(time);
        patients[patientName]->cancelAppointment(doctorName, time);
        cout << "Booking ID " << bookingId << " is cancelled\n";
    }

    void showAvailableSlotsBySpeciality(string speciality)
    {
        vector<pair<string, string>> availableSlots;
        for (auto it = doctors.begin(); it != doctors.end(); it++)
        {
            if (it->second->doctorSpecialization == speciality)
            {
                for (int i = 0; i < it->second->doctorSlots.size(); i++)
                {
                    if (it->second->doctorSlots[i]->isCurrSlotAvailable)
                    {
                        availableSlots.push_back({it->first, it->second->doctorSlots[i]->startTime + "-" + it->second->doctorSlots[i]->endTime});
                    }
                }
            }
        }
        sort(availableSlots.begin(), availableSlots.end(), [](pair<string, string> a, pair<string, string> b)
             { return a.second < b.second; });
        for (int i = 0; i < availableSlots.size(); i++)
        {
            cout << "Dr. " << availableSlots[i].first << " : " << availableSlots[i].second << "\n";
        }
    }
};

FlipCare *FlipCare::instance = nullptr;

int main()
{
    FlipCare *flipCare = FlipCare::getInstance();
    flipCare->registerDoctor("Curious", "Cardiologist");
    flipCare->markDoctorAvailability("Curious", {"9:30-10:30"});
    flipCare->markDoctorAvailability("Curious", {"09:30-10:00", "12:30-13:00", "16:00-16:30"});
    flipCare->showAvailableSlotsBySpeciality("Cardiologist");
    flipCare->registerPatient("PatientA");
    flipCare->bookAppointment("Curious", "PatientA", "12:30");
    flipCare->showAvailableSlotsBySpeciality("Cardiologist");
    flipCare->cancelBookingId(1);
    flipCare->showAvailableSlotsBySpeciality("Cardiologist");
    return 0;
}