//
//  main.cpp
//  Module_6
//
//  Created by Navjot Thind on 4/6/25.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iomanip>
#include <algorithm>

using namespace std;

// Define a constant arrival date (as used in the sample output)
const string ARRIVAL_DATE = "2024-03-05";

// Utility function to trim whitespace from a string.
string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

// Utility function to convert a string to lowercase.
string toLower(const string &s) {
    string res = s;
    transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

// genBirthDay: Generates a birthday using the animal's age and birth season.
// Uses a fixed mapping: spring->03-15, summer->06-15, fall->09-15, winter->12-15,
// and if birth season is unknown, defaults to 01-01.
string genBirthDay(int age, const string &birthSeason) {
    string monthDay;
    string seasonLower = toLower(birthSeason);
    if(seasonLower.find("spring") != string::npos) {
        monthDay = "03-15";
    } else if(seasonLower.find("summer") != string::npos) {
        monthDay = "06-15";
    } else if(seasonLower.find("fall") != string::npos) {
        monthDay = "09-15";
    } else if(seasonLower.find("winter") != string::npos) {
        monthDay = "12-15";
    } else {
        monthDay = "01-01";
    }
    // Calculate birth year based on the ARRIVAL_DATE.
    int arrivalYear = stoi(ARRIVAL_DATE.substr(0,4));
    int birthYear = arrivalYear - age;
    return to_string(birthYear) + "-" + monthDay;
}

// genUniqueID: Creates a unique ID from the species name and a counter.
// The first two letters (capitalized appropriately) followed by a two-digit number.
string genUniqueID(const string &species, int counter) {
    string sp = toLower(species);
    string id;
    if(sp == "hyena") id = "Hy";
    else if(sp == "lion") id = "Li";
    else if(sp == "tiger") id = "Ti";
    else if(sp == "bear") id = "Be";
    else id = "XX";
    ostringstream oss;
    oss << setw(2) << setfill('0') << counter;
    return id + oss.str();
}

// Base Animal class to hold common attributes and behavior.
class Animal {
public:
    string id;
    string name;
    string birthDate;
    string color;
    string sex;
    int weight;
    string origin;
    string species;
    string arrivalDate;
    
    Animal(const string& id, const string& name, const string& birthDate,
           const string& color, const string& sex, int weight,
           const string& origin, const string& species)
    : id(id), name(name), birthDate(birthDate), color(color),
      sex(sex), weight(weight), origin(origin), species(species),
      arrivalDate(ARRIVAL_DATE) {}
    
    // Returns a header line for the habitat.
    virtual string getHabitat() const {
        return species + " Habitat:";
    }
    
    // Returns a formatted string containing all animal attributes.
    virtual string toString() const {
        ostringstream oss;
        oss << id << "; " << name << "; birth date " << birthDate
            << "; " << color << "; " << sex << "; " << weight
            << " pounds; from " << origin << "; arrived " << arrivalDate;
        return oss.str();
    }
    
    virtual ~Animal() {}
};

// Derived classes for each species.
class Hyena : public Animal {
public:
    Hyena(const string &id, const string &name, const string &birthDate,
          const string &color, const string &sex, int weight, const string &origin)
    : Animal(id, name, birthDate, color, sex, weight, origin, "Hyena") {}
};

class Lion : public Animal {
public:
    Lion(const string &id, const string &name, const string &birthDate,
         const string &color, const string &sex, int weight, const string &origin)
    : Animal(id, name, birthDate, color, sex, weight, origin, "Lion") {}
};

class Tiger : public Animal {
public:
    Tiger(const string &id, const string &name, const string &birthDate,
          const string &color, const string &sex, int weight, const string &origin)
    : Animal(id, name, birthDate, color, sex, weight, origin, "Tiger") {}
};

class Bear : public Animal {
public:
    Bear(const string &id, const string &name, const string &birthDate,
         const string &color, const string &sex, int weight, const string &origin)
    : Animal(id, name, birthDate, color, sex, weight, origin, "Bear") {}
};

int main() {
    try {
        // Step 1: Read animal names from animalNames.txt and organize them by species.
        ifstream namesFile("animalNames.txt");
        if(!namesFile)
            throw runtime_error("Could not open animalNames.txt");
        
        map<string, vector<string>> namesMap;
        string line;
        string currentSpecies;
        while(getline(namesFile, line)) {
            line = trim(line);
            if(line.empty()) continue;
            // Check if the line is a header (e.g., "Hyena Names:")
            if(line.find("Names:") != string::npos) {
                string header = toLower(line);
                if(header.find("hyena") != string::npos) currentSpecies = "hyena";
                else if(header.find("lion") != string::npos) currentSpecies = "lion";
                else if(header.find("bear") != string::npos) currentSpecies = "bear";
                else if(header.find("tiger") != string::npos) currentSpecies = "tiger";
            } else {
                // Parse the comma-separated names.
                istringstream iss(line);
                string name;
                while(getline(iss, name, ',')) {
                    name = trim(name);
                    if(!name.empty())
                        namesMap[currentSpecies].push_back(name);
                }
            }
        }
        namesFile.close();
        
        // Set up counters for assigning names and unique IDs.
        map<string, int> nameIndex = { {"hyena", 0}, {"lion", 0}, {"tiger", 0}, {"bear", 0} };
        map<string, int> idCounters = { {"hyena", 0}, {"lion", 0}, {"tiger", 0}, {"bear", 0} };
        
        // Vectors to store animals organized by species.
        vector<Animal*> hyenas, lions, tigers, bears;
        
        // Step 2: Read and parse arrivingAnimals.txt.
        ifstream animalsFile("arrivingAnimals.txt");
        if(!animalsFile)
            throw runtime_error("Could not open arrivingAnimals.txt");
        
        while(getline(animalsFile, line)) {
            if(line.empty()) continue;
            
            // Split the line by commas to extract different attributes.
            vector<string> segments;
            istringstream segStream(line);
            string segment;
            while(getline(segStream, segment, ',')) {
                segments.push_back(trim(segment));
            }
            if(segments.size() < 5) continue; // Ensure there are enough segments.
            
            // Parse the first segment: "4 year old female hyena"
            istringstream iss(segments[0]);
            int age;
            string dummy;
            string sex;
            string species;
            iss >> age;         // Age (e.g., 4)
            iss >> dummy;       // "year"
            iss >> dummy;       // "old"
            iss >> sex;         // Sex (e.g., "female")
            iss >> species;     // Species (e.g., "hyena")
            
            // Parse the second segment for birth season (e.g., "born in spring" or "unknown birth season").
            string birthSeason;
            if(segments[1].find("born in") != string::npos) {
                birthSeason = segments[1].substr(segments[1].find("born in") + 8);
            } else {
                birthSeason = segments[1];
            }
            birthSeason = trim(birthSeason);
            
            // Parse the third segment for color (removing the trailing "color").
            string color = segments[2];
            size_t pos = color.find("color");
            if(pos != string::npos) {
                color = trim(color.substr(0, pos));
            }
            
            // Parse the fourth segment for weight (removing the trailing "pounds").
            string weightStr = segments[3];
            pos = weightStr.find("pounds");
            if(pos != string::npos) {
                weightStr = trim(weightStr.substr(0, pos));
            }
            int weight = stoi(weightStr);
            
            // Parse the fifth segment for origin (remove the "from" prefix).
            string origin = segments[4];
            pos = origin.find("from");
            if(pos != string::npos) {
                origin = trim(origin.substr(pos + 5));
            }
            
            // Generate the animal's birthday.
            string birthDate = genBirthDay(age, birthSeason);
            // Generate a unique ID for the animal.
            string speciesLower = toLower(species);
            idCounters[speciesLower]++;
            string uniqueID = genUniqueID(speciesLower, idCounters[speciesLower]);
            // Assign a unique name from the namesMap.
            string assignedName = "Unnamed";
            if(nameIndex.find(speciesLower) != nameIndex.end() && nameIndex[speciesLower] < namesMap[speciesLower].size()) {
                assignedName = namesMap[speciesLower][nameIndex[speciesLower]++];
            }
            
            // Create an object of the appropriate subclass and add it to its habitat.
            Animal* animal = nullptr;
            if(speciesLower == "hyena") {
                animal = new Hyena(uniqueID, assignedName, birthDate, color, sex, weight, origin);
                hyenas.push_back(animal);
            } else if(speciesLower == "lion") {
                animal = new Lion(uniqueID, assignedName, birthDate, color, sex, weight, origin);
                lions.push_back(animal);
            } else if(speciesLower == "tiger") {
                animal = new Tiger(uniqueID, assignedName, birthDate, color, sex, weight, origin);
                tigers.push_back(animal);
            } else if(speciesLower == "bear") {
                animal = new Bear(uniqueID, assignedName, birthDate, color, sex, weight, origin);
                bears.push_back(animal);
            } else {
                // If species is unrecognized, skip this record.
                continue;
            }
        }
        animalsFile.close();
        
        // Step 3: Write the detailed report to zooPopulation.txt.
        ofstream reportFile("zooPopulation.txt");
        if(!reportFile)
            throw runtime_error("Could not open zooPopulation.txt for writing");
        
        auto writeHabitat = [&reportFile](const string &habitatName, const vector<Animal*> &animals) {
            reportFile << habitatName << "\n";
            for(const auto &a : animals) {
                reportFile << a->toString() << "\n";
            }
            reportFile << "\n";
        };
        
        writeHabitat("Hyena Habitat:", hyenas);
        writeHabitat("Lion Habitat:", lions);
        writeHabitat("Tiger Habitat:", tigers);
        writeHabitat("Bear Habitat:", bears);
        
        reportFile.close();
        
        // Clean up dynamically allocated memory.
        for(auto a : hyenas) delete a;
        for(auto a : lions) delete a;
        for(auto a : tigers) delete a;
        for(auto a : bears) delete a;
        
        cout << "Zoo population report generated successfully in zooPopulation.txt\n";
    }
    catch(const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

