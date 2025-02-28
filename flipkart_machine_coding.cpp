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

    bool bookSlot(string startTime, string patientName)
    {
        bool slotBookingDone = false;
        for (int i = 0; i < doctorSlots.size(); i++)
        {
            if (doctorSlots[i]->startTime == startTime)
            {
                if (doctorSlots[i]->isCurrSlotAvailable)
                {
                    slotBookingDone = true;
                    doctorSlots[i]->isCurrSlotAvailable = false;
                    doctorAppointmentCount++;
                }
                // If the patient wishes to book a slot for a particular doctor that is already booked, then add this patient to the waitlist
                else
                {
                    slotBookingDone = false;
                    doctorSlots[i]->slotWaitListQ.push(patientName);
                }
                break;
            }
        }
        return slotBookingDone;
    }

    string cancelSlot(string startTime)
    {
        string newPatient = "";
        for (int i = 0; i < doctorSlots.size(); i++)
        {
            if (doctorSlots[i]->startTime == startTime)
            {
                if (!doctorSlots[i]->isCurrSlotAvailable)
                {
                    doctorSlots[i]->isCurrSlotAvailable = true;
                    doctorAppointmentCount--;
                    // If the patient with whom the appointment is booked originally, cancels the appointment, then the first in the waitlist gets the appointment.
                    if (!doctorSlots[i]->slotWaitListQ.empty())
                    {
                        newPatient = doctorSlots[i]->slotWaitListQ.front();
                        doctorSlots[i]->slotWaitListQ.pop();
                        bool slotBooked = bookSlot(startTime, newPatient); // Book the slot for the patient in the waitlist
                    }
                }
                else
                {
                    cout << "Slot is already available\n";
                }
                break;
            }
        }
        return newPatient;
    }
};

class Patient
{
public:
    string patientName;
    vector<vector<string>> patientAppointments;
    Patient(string patientName)
    {
        this->patientName = patientName;
    }

    void bookAppointment(string doctorName, string time, string bookingStatus)
    {
        patientAppointments.push_back({doctorName, time, bookingStatus});
    }

    void cancelAppointment(string doctorName, string time)
    {
        for (int i = 0; i < patientAppointments.size(); i++)
        {
            if (patientAppointments[i][0] == doctorName && patientAppointments[i][1] == time)
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

    // A new doctor should be able to register, and mention his/her speciality among (Cardiologist, Dermatologist, Orthopedic, General Physician)
    void registerDoctor(string doctorName, string doctorSpecialization)
    {
        if (doctors.find(doctorName) == doctors.end())
        {
            doctors[doctorName] = new Doctor(doctorName, doctorSpecialization);
            cout << "Welcome Dr. " << doctorName << " !!\n";
        }
        else
        {
            cout << "Doctor already exists\n\n";
        }
        cout << '\n';
    }

    // A doctor should be able to declare his/her availability in each slot for the day. For example, the slots will be of 30 mins like 9am-9.30am, 9.30am-10am
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
        cout << '\n';
    }

    // Patients should be able to login
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

    // Patients should be able to book appointments with a doctor for an available slot.A patient can book multiple appointments in a day.
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
        // Need to check whether the patient has already made the appointment at this time
        for (int i = 0; i < patients[patientName]->patientAppointments.size(); i++)
        {
            if (patients[patientName]->patientAppointments[i][1] == time)
            {
                cout << "Patient " << patientName << " already has an appointment at this time with Dr. " << patients[patientName]->patientAppointments[i][0] << "\n";
                cout << "Hence cannot book appointment with Dr. " << doctorName << " at this time\n\n";
                return;
            }
        }
        bool slotBooked = doctors[doctorName]->bookSlot(time, patientName);
        // cout << "Slot booked status: " << slotBooked << " for patient " << patientName << "with doctor " << doctorName << " at time " << time << "\n";
        string bookingStatus = (slotBooked) ? "Booked" : "Waitlisted";
        patients[patientName]->bookAppointment(doctorName, time, bookingStatus);
        bookingIdToPatientDoctorMap.insert({bookingIdCounter, {patientName, doctorName, time}});
        cout << "Booked. Booking id: " << bookingIdCounter++ << "\n\n";
    }

    // Patients can also cancel an appointment, in which case that slot becomes available for someone else to book.
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
        string newPatient = doctors[doctorName]->cancelSlot(time);
        patients[patientName]->cancelAppointment(doctorName, time);
        cout << "Booking ID " << bookingId << " is cancelled\n\n";
        if (newPatient != "")
        {
            for (int i = 0; i < patients[newPatient]->patientAppointments.size(); i++)
            {
                if (patients[newPatient]->patientAppointments[i][0] == doctorName)
                {
                    patients[newPatient]->patientAppointments[i][2] = "Booked";
                    break;
                }
            }
        }
    }

    // The slots should be displayed in a ranked fashion
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
        // Custom sorting function can be used here to sort the available slots based on our needs.
        // Use strategy pattern to sort the available slots based on the requirement
        sort(availableSlots.begin(), availableSlots.end(), [](pair<string, string> a, pair<string, string> b)
             { return a.second < b.second; });
        cout << "Available slots for " << speciality << " are as follows:\n";
        for (int i = 0; i < availableSlots.size(); i++)
        {
            cout << "Dr. " << availableSlots[i].first << " : " << availableSlots[i].second << "\n";
        }
        cout << '\n';
    }

    void displayDoctorSlots(string doctorName)
    {
        if (doctors.find(doctorName) != doctors.end())
        {
            cout << "Dr. " << doctorName << " slots' status is as follows:\n";
            for (int i = 0; i < doctors[doctorName]->doctorSlots.size(); i++)
            {
                cout << doctors[doctorName]->doctorSlots[i]->startTime << "-" << doctors[doctorName]->doctorSlots[i]->endTime << " : ";
                if (doctors[doctorName]->doctorSlots[i]->isCurrSlotAvailable)
                {
                    cout << "Available\n";
                }
                else
                {
                    cout << "Not Available\n";
                }
            }
        }
        else
        {
            cout << "Doctor not found\n";
        }
        cout << '\n';
    }

    void displayPatientAppointments(string patientName)
    {
        if (patients.find(patientName) != patients.end())
        {
            cout << "Patient " << patientName << " has the following appointments:\n";
            for (int i = 0; i < patients[patientName]->patientAppointments.size(); i++)
            {
                cout << "Dr. " << patients[patientName]->patientAppointments[i][0] << " : " << patients[patientName]->patientAppointments[i][1] << " " << patients[patientName]->patientAppointments[i][2] << "\n";
            }
            if (patients[patientName]->patientAppointments.size() == 0)
            {
                cout << "No appointments\n";
            }
        }
        else
        {
            cout << "Patient not found\n";
        }
        cout << '\n';
    }
};

FlipCare *FlipCare::instance = nullptr;

int main()
{
    FlipCare *flipCare = FlipCare::getInstance();
    flipCare->registerDoctor("Curious", "Cardiologist");
    flipCare->markDoctorAvailability("Curious", {"09:30-10:30"});
    flipCare->markDoctorAvailability("Curious", {"09:30-10:00", "12:30-13:00", "16:00-16:30"});
    flipCare->registerDoctor("Dreadful", "Dermatologist");
    flipCare->markDoctorAvailability("Dreadful", {"09:30-10:00", "12:30-13:00", "16:00-16:30"});
    flipCare->showAvailableSlotsBySpeciality("Cardiologist");
    flipCare->registerPatient("PatientA");
    flipCare->bookAppointment("Curious", "PatientA", "12:30");
    flipCare->displayDoctorSlots("Curious");
    flipCare->displayPatientAppointments("PatientA");
    flipCare->showAvailableSlotsBySpeciality("Cardiologist");
    flipCare->cancelBookingId(1);
    flipCare->displayDoctorSlots("Curious");
    flipCare->showAvailableSlotsBySpeciality("Cardiologist");
    flipCare->registerPatient("PatientB");
    flipCare->bookAppointment("Curious", "PatientB", "12:30");
    flipCare->displayDoctorSlots("Curious");
    flipCare->displayPatientAppointments("PatientB");
    flipCare->registerDoctor("Daring", "Dermatologist");
    flipCare->markDoctorAvailability("Daring", {"12:30-13:00", "14:00-14:30"});
    flipCare->bookAppointment("Daring", "PatientB", "12:30");
    flipCare->displayPatientAppointments("PatientB");
    flipCare->showAvailableSlotsBySpeciality("Dermatologist");
    cout << "+++++++++++++\n";
    flipCare->bookAppointment("Daring", "PatientB", "14:00");
    flipCare->bookAppointment("Daring", "PatientA", "14:00");
    flipCare->displayPatientAppointments("PatientB");
    flipCare->displayPatientAppointments("PatientA");
    flipCare->cancelBookingId(3);
    flipCare->displayPatientAppointments("PatientB");
    flipCare->displayPatientAppointments("PatientA");
    return 0;
}