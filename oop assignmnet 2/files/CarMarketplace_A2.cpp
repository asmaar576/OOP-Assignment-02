#include <iostream>
#include <string>
#include "IListable.h"
#include "ISearchable.h"
#include "IReportable.h"
using namespace std;

class Vehicle;
class Seller;
class Buyer;
class Admin;
class Marketplace;
double negotiateDeal(Buyer& buyer, Seller& seller, double askingPrice);
void generateAuditLog(Admin& admin, Marketplace& platform);

class PriceRange {
private:
    double minPrice;
    double maxPrice;
    string label;

public:
    PriceRange() : minPrice(0), maxPrice(0), label("Any") {}

    PriceRange(double minP, double maxP, const string& lbl = "")
        : minPrice(minP), maxPrice(maxP), label(lbl) {
        if (minP > maxP) { minPrice = maxP; maxPrice = minP; }
    }

    PriceRange(const PriceRange& other)
        : minPrice(other.minPrice), maxPrice(other.maxPrice), label(other.label) {}

    double getMin() const { return minPrice; }
    double getMax() const { return maxPrice; }
    string getLabel() const { return label; }
    double getMidpoint() const { return (minPrice + maxPrice) / 2.0; }
    void setMin(double v) { minPrice = v; }
    void setMax(double v) { maxPrice = v; }
    bool contains(double price) const { return price >= minPrice && price <= maxPrice; }
    double span() const { return maxPrice - minPrice; }

    // merges two price windows into widest possible span
    // Useful when buyer wants to combine a "budget" and "stretch" range into one search
    PriceRange operator+(const PriceRange& other) const {
        double newMin = (minPrice < other.minPrice) ? minPrice : other.minPrice;
        double newMax = (maxPrice > other.maxPrice) ? maxPrice : other.maxPrice;
        return PriceRange(newMin, newMax, label + "+" + other.label);
    }

    // exact same window
    bool operator==(const PriceRange& other) const {
        return minPrice == other.minPrice && maxPrice == other.maxPrice;
    }

    // compare ranges by midpoint value (useful for ordering budget tiers)
    bool operator<(const PriceRange& other) const { return getMidpoint() < other.getMidpoint(); }
    bool operator>(const PriceRange& other) const { return getMidpoint() > other.getMidpoint(); }

    // Friend: stream output reads private fields directly for clean display
    friend ostream& operator<<(ostream& os, const PriceRange& pr) {
        os << "[" << pr.label << ": PKR " << pr.minPrice << " - " << pr.maxPrice << "]";
        return os;
    }
};

class VehicleSpecs {
private:
    int engineCC;
    string transmission;
    string fuelType;
    string color;
    int seatingCapacity;
    string bodyType;

public:
    VehicleSpecs() : engineCC(1000), transmission("Manual"), fuelType("Petrol"),
                     color("White"), seatingCapacity(5), bodyType("Sedan") {}

    VehicleSpecs(int cc, const string& trans, const string& fuel,
                 const string& col, int seats, const string& body)
        : engineCC(cc), transmission(trans), fuelType(fuel),
          color(col), seatingCapacity(seats), bodyType(body) {}

    VehicleSpecs(const VehicleSpecs& other)
        : engineCC(other.engineCC), transmission(other.transmission), fuelType(other.fuelType),
          color(other.color), seatingCapacity(other.seatingCapacity), bodyType(other.bodyType) {}

    int getEngineCC() const { return engineCC; }
    string getTransmission() const { return transmission; }
    string getFuelType() const { return fuelType; }
    string getColor() const { return color; }
    int getSeatingCapacity() const { return seatingCapacity; }
    string getBodyType() const { return bodyType; }
    void setTransmission(const string& t) { transmission = t; }
    void setColor(const string& c) { color = c; }

    void display() const {
        cout << "Engine: " << engineCC << "cc | " << fuelType << " | "
             << transmission << " | " << color << " | " << bodyType
             << " | " << seatingCapacity << " seats" << endl;
    }
    bool isElectric() const { return fuelType == "Electric"; }
    string getSpecSummary() const { return to_string(engineCC) + "cc " + transmission + " " + fuelType; }
    bool isLargeEngine() const { return engineCC >= 2000; }
};

class Vehicle : public IListable {
protected:
    static int totalListings;
    string listingID;
    string brand;
    string model;
    int year;
    double price;
    int mileage;
    string condition;
    bool isApproved;
    bool isSold;
    string sellerID;
    VehicleSpecs specs;

public:
    Vehicle() : listingID("VH0000"), brand("Unknown"), model("Unknown"), year(2000),
                price(0.0), mileage(0), condition("Used"), isApproved(false),
                isSold(false), sellerID("") { totalListings++; }

    Vehicle(const string& id, const string& b, const string& m, int yr, double p,
            int mil, const string& cond, const string& sid, const VehicleSpecs& sp)
        : listingID(id), brand(b), model(m), year(yr), price(p), mileage(mil),
          condition(cond), isApproved(false), isSold(false), sellerID(sid), specs(sp) {
        totalListings++;
    }

    Vehicle(const Vehicle& other)
        : listingID(other.listingID + "_copy"), brand(other.brand), model(other.model),
          year(other.year), price(other.price), mileage(other.mileage),
          condition(other.condition), isApproved(false), isSold(false),
          sellerID(other.sellerID), specs(other.specs) { totalListings++; }

    virtual ~Vehicle() {}

    string getListingID() const override { return listingID; }
    string getBrand() const { return brand; }
    string getModel() const { return model; }
    int getYear() const { return year; }
    double getPrice() const override { return price; }
    int getMileage() const { return mileage; }
    string getCondition() const { return condition; }
    bool getIsApproved() const { return isApproved; }
    bool getIsSold() const { return isSold; }
    string getSellerID() const { return sellerID; }
    const VehicleSpecs& getSpecs() const { return specs; }

    void setPrice(double p) { if (p >= 0) price = p; }
    void setMileage(int m) { if (m >= 0) mileage = m; }
    void setCondition(const string& c) { condition = c; }
    void setApproved(bool status) { isApproved = status; }
    void setSold(bool status) { isSold = status; }

    static int getTotalListings() { return totalListings; }

    // Pure virtual — each vehicle type displays itself differently (Polymorphism)
    virtual void displayDetails() const override = 0;
    virtual string getListingSummary() const override = 0;

    void displayListing() const {
        cout << "----------------------------------------" << endl;
        displayDetails();
        cout << "Price: PKR " << price << " | Mileage: " << mileage << " km" << endl;
        cout << "Condition: " << condition << " | Status: "
             << (isApproved ? "Approved" : "Pending") << (isSold ? " [SOLD]" : "") << endl;
        specs.display();
        cout << "----------------------------------------" << endl;
    }

    bool matchesFilter(const string& fb, const string& fm, double minP, double maxP,
                       int minY, int maxY, int maxMil) const {
        if (!fb.empty() && brand != fb) return false;
        if (!fm.empty() && model != fm) return false;
        if (price < minP || price > maxP) return false;
        if (year < minY || year > maxY) return false;
        if (mileage > maxMil) return false;
        return true;
    }

    double getDepreciatedValue(int currentYear) const {
        int age = currentYear - year;
        double value = price;
        for (int i = 0; i < age; i++) value *= 0.90;
        return value;
    }

    // listing identity is the ID, not memory address
    bool operator==(const Vehicle& other) const { return listingID == other.listingID; }
    // price-based comparison for sorting search results
    bool operator<(const Vehicle& other) const { return price < other.price; }
    bool operator>(const Vehicle& other) const { return price > other.price; }

    // Friend stream operator: accesses protected members for compact one-line output
    friend ostream& operator<<(ostream& os, const Vehicle& v) {
        os << "[" << v.listingID << "] " << v.year << " " << v.brand << " " << v.model
           << " | PKR " << v.price << " | " << v.mileage << "km"
           << (v.isApproved ? " [Approved]" : " [Pending]");
        return os;
    }
};

int Vehicle::totalListings = 0;

class Car : public Vehicle {
private:
    bool hasSunroof;
    int airbagCount;
    string infotainmentSystem;
    int safetyRating;
    int numDoors;

public:
    Car() : Vehicle(), hasSunroof(false), airbagCount(2),
            infotainmentSystem("Basic"), safetyRating(3), numDoors(4) {}

    Car(const string& id, const string& brand, const string& model, int year, double price,
        int mileage, const string& condition, const string& sellerID, const VehicleSpecs& specs,
        bool sunroof, int airbags, const string& infotainment, int safety, int doors)
        : Vehicle(id, brand, model, year, price, mileage, condition, sellerID, specs),
          hasSunroof(sunroof), airbagCount(airbags), infotainmentSystem(infotainment),
          safetyRating(safety), numDoors(doors) {}

    Car(const Car& other)
        : Vehicle(other), hasSunroof(other.hasSunroof), airbagCount(other.airbagCount),
          infotainmentSystem(other.infotainmentSystem), safetyRating(other.safetyRating),
          numDoors(other.numDoors) {}

    bool getHasSunroof() const { return hasSunroof; }
    int getAirbagCount() const { return airbagCount; }
    string getInfotainment() const { return infotainmentSystem; }
    int getSafetyRating() const { return safetyRating; }
    int getNumDoors() const { return numDoors; }
    void setHasSunroof(bool val) { hasSunroof = val; }
    void setAirbagCount(int val) { if (val >= 0) airbagCount = val; }
    void setSafetyRating(int val) { if (val >= 1 && val <= 5) safetyRating = val; }

    // Polymorphic override: shows car-specific details Bike and Truck do not have
    void displayDetails() const override {
        cout << "[CAR] " << year << " " << brand << " " << model << endl;
        cout << "Doors: " << numDoors << " | Airbags: " << airbagCount
             << " | Sunroof: " << (hasSunroof ? "Yes" : "No") << endl;
        cout << "Infotainment: " << infotainmentSystem
             << " | Safety Rating: " << safetyRating << "/5 stars" << endl;
    }

    string getListingSummary() const override {
        return "[CAR] " + to_string(year) + " " + brand + " " + model +
               " - PKR " + to_string((int)price);
    }

    void displaySafetyInfo() const {
        cout << "Safety: " << airbagCount << " airbags, " << safetyRating << "/5 NCAP rating" << endl;
    }

    bool isPremium() const { return safetyRating >= 4 && airbagCount >= 6 && hasSunroof; }
};

class Bike : public Vehicle {
private:
    string bikeType;
    bool hasCargoRack;
    int maxRPM;
    bool isElectricKickstart;
    double weightKG;

public:
    Bike() : Vehicle(), bikeType("Standard"), hasCargoRack(false),
             maxRPM(8000), isElectricKickstart(false), weightKG(120.0) {}

    Bike(const string& id, const string& brand, const string& model, int year, double price,
         int mileage, const string& condition, const string& sellerID, const VehicleSpecs& specs,
         const string& bt, bool cargo, int rpm, bool ekick, double wkg)
        : Vehicle(id, brand, model, year, price, mileage, condition, sellerID, specs),
          bikeType(bt), hasCargoRack(cargo), maxRPM(rpm), isElectricKickstart(ekick), weightKG(wkg) {}

    Bike(const Bike& other)
        : Vehicle(other), bikeType(other.bikeType), hasCargoRack(other.hasCargoRack),
          maxRPM(other.maxRPM), isElectricKickstart(other.isElectricKickstart), weightKG(other.weightKG) {}

    string getBikeType() const { return bikeType; }
    bool getHasCargoRack() const { return hasCargoRack; }
    int getMaxRPM() const { return maxRPM; }
    bool getIsElectricKickstart() const { return isElectricKickstart; }
    double getWeightKG() const { return weightKG; }
    void setBikeType(const string& t) { bikeType = t; }
    void setHasCargoRack(bool val) { hasCargoRack = val; }
    void setMaxRPM(int val) { if (val > 0) maxRPM = val; }

    // Polymorphic override: shows bike-specific details
    void displayDetails() const override {
        cout << "[BIKE] " << year << " " << brand << " " << model << endl;
        cout << "Type: " << bikeType << " | Weight: " << weightKG
             << " kg | Max RPM: " << maxRPM << endl;
        cout << "Cargo Rack: " << (hasCargoRack ? "Yes" : "No")
             << " | Electric Kickstart: " << (isElectricKickstart ? "Yes" : "No") << endl;
    }

    string getListingSummary() const override {
        return "[BIKE] " + to_string(year) + " " + brand + " " + model +
               " (" + bikeType + ") - PKR " + to_string((int)price);
    }

    void displayPerformanceInfo() const {
        cout << "Performance: Max RPM = " << maxRPM
             << " | Engine: " << specs.getSpecSummary() << endl;
    }

    bool isSportsCategory() const { return bikeType == "Sports" || maxRPM >= 10000; }
};

class Truck : public Vehicle {
private:
    double payloadCapacityTons;
    int numAxles;
    bool hasRefrigeration;
    string truckCategory;
    double towingCapacityKG;

public:
    Truck() : Vehicle(), payloadCapacityTons(1.0), numAxles(2),
              hasRefrigeration(false), truckCategory("Mini"), towingCapacityKG(2000) {}

    Truck(const string& id, const string& brand, const string& model, int year, double price,
          int mileage, const string& condition, const string& sellerID, const VehicleSpecs& specs,
          double payload, int axles, bool refrig, const string& cat, double towCap)
        : Vehicle(id, brand, model, year, price, mileage, condition, sellerID, specs),
          payloadCapacityTons(payload), numAxles(axles), hasRefrigeration(refrig),
          truckCategory(cat), towingCapacityKG(towCap) {}

    Truck(const Truck& other)
        : Vehicle(other), payloadCapacityTons(other.payloadCapacityTons), numAxles(other.numAxles),
          hasRefrigeration(other.hasRefrigeration), truckCategory(other.truckCategory),
          towingCapacityKG(other.towingCapacityKG) {}

    double getPayloadCapacity() const { return payloadCapacityTons; }
    int getNumAxles() const { return numAxles; }
    bool getHasRefrigeration() const { return hasRefrigeration; }
    string getTruckCategory() const { return truckCategory; }
    double getTowingCapacity() const { return towingCapacityKG; }
    void setPayloadCapacity(double val) { if (val > 0) payloadCapacityTons = val; }
    void setHasRefrigeration(bool val) { hasRefrigeration = val; }
    void setTruckCategory(const string& cat) { truckCategory = cat; }

    // Polymorphic override: shows truck/commercial-specific details
    void displayDetails() const override {
        cout << "[TRUCK] " << year << " " << brand << " " << model << endl;
        cout << "Category: " << truckCategory << " | Axles: " << numAxles
             << " | Payload: " << payloadCapacityTons << " tons" << endl;
        cout << "Refrigeration: " << (hasRefrigeration ? "Yes" : "No")
             << " | Towing Cap: " << towingCapacityKG << " kg" << endl;
    }

    string getListingSummary() const override {
        return "[TRUCK] " + to_string(year) + " " + brand + " " + model +
               " (" + truckCategory + ") - PKR " + to_string((int)price);
    }

    void displayCargoInfo() const {
        cout << "Cargo: " << payloadCapacityTons << " ton payload | "
             << numAxles << " axles | Tow: " << towingCapacityKG << " kg" << endl;
    }

    bool isHeavyDuty() const { return payloadCapacityTons >= 5.0 || numAxles >= 4; }
};

class Address {
private:
    string city;
    string province;
    string country;
    string postalCode;
    string streetAddress;

public:
    Address() : city("Unknown"), province("Unknown"), country("Pakistan"),
                postalCode("00000"), streetAddress("N/A") {}

    Address(const string& c, const string& prov, const string& ctry,
            const string& pc, const string& st)
        : city(c), province(prov), country(ctry), postalCode(pc), streetAddress(st) {}

    Address(const Address& other)
        : city(other.city), province(other.province), country(other.country),
          postalCode(other.postalCode), streetAddress(other.streetAddress) {}

    string getCity() const { return city; }
    string getProvince() const { return province; }
    string getCountry() const { return country; }
    string getPostalCode() const { return postalCode; }
    string getStreetAddress() const { return streetAddress; }
    void setCity(const string& c) { city = c; }
    void setProvince(const string& p) { province = p; }
    void setPostalCode(const string& pc) { postalCode = pc; }
    void setStreetAddress(const string& s) { streetAddress = s; }

    void display() const {
        cout << streetAddress << ", " << city << ", " << province
             << " - " << postalCode << ", " << country << endl;
    }
    bool isValidPostalCode() const { return postalCode.length() == 5; }
    string getFullAddress() const { return streetAddress + ", " + city + ", " + province; }
    bool isSameCity(const Address& other) const { return city == other.city; }
};

class ContactInfo {
private:
    string email;
    string phoneNumber;
    string whatsappNumber;
    bool emailVerified;
    bool phoneVerified;

public:
    ContactInfo() : email(""), phoneNumber(""), whatsappNumber(""),
                    emailVerified(false), phoneVerified(false) {}

    ContactInfo(const string& em, const string& ph, const string& wa = "")
        : email(em), phoneNumber(ph), whatsappNumber(wa.empty() ? ph : wa),
          emailVerified(false), phoneVerified(false) {}

    string getEmail() const { return email; }
    string getPhone() const { return phoneNumber; }
    string getWhatsapp() const { return whatsappNumber; }
    bool isEmailVerified() const { return emailVerified; }
    bool isPhoneVerified() const { return phoneVerified; }
    void setEmail(const string& e) { email = e; emailVerified = false; }
    void setPhone(const string& p) { phoneNumber = p; phoneVerified = false; }

    void verifyEmail() { emailVerified = true; }
    void verifyPhone() { phoneVerified = true; }
    bool isFullyVerified() const { return emailVerified && phoneVerified; }

    void display() const {
        cout << "Email: " << email << (emailVerified ? " [Verified]" : " [Unverified]") << endl;
        cout << "Phone: " << phoneNumber << (phoneVerified ? " [Verified]" : " [Unverified]") << endl;
    }
    bool isValidEmail() const {
        return email.find('@') != string::npos && email.find('.') != string::npos;
    }
};

class Message {
private:
    string senderID;
    string receiverID;
    string content;
    string timestamp;
    bool isRead;

public:
    Message() : senderID(""), receiverID(""), content(""), timestamp(""), isRead(false) {}

    Message(const string& from, const string& to, const string& msg, const string& time)
        : senderID(from), receiverID(to), content(msg), timestamp(time), isRead(false) {}

    string getSenderID() const { return senderID; }
    string getReceiverID() const { return receiverID; }
    string getContent() const { return content; }
    string getTimestamp() const { return timestamp; }
    bool getIsRead() const { return isRead; }
    void setContent(const string& c) { content = c; }

    void markAsRead() { isRead = true; }

    void display() const {
        cout << "[" << timestamp << "] From: " << senderID << " -> " << receiverID << endl;
        cout << "   " << content << (isRead ? "" : " [NEW]") << endl;
    }

    bool isFromSender(const string& id) const { return senderID == id; }

    string getSummary() const {
        string s = content.substr(0, min((int)content.length(), 30));
        if (content.length() > 30) s += "...";
        return s;
    }

    // same sender + receiver + timestamp = same message event (for deduplication)
    bool operator==(const Message& other) const {
        return senderID == other.senderID && receiverID == other.receiverID && timestamp == other.timestamp;
    }

    // Friend stream: reads private fields directly for compact one-line output
    friend ostream& operator<<(ostream& os, const Message& msg) {
        os << "[" << msg.timestamp << "] " << msg.senderID << " -> " << msg.receiverID
           << ": " << msg.getSummary() << (msg.isRead ? "" : " [NEW]");
        return os;
    }
};

class Inbox {
private:
    static int totalMessagesEverSent;
    static const int MAX_MESSAGES = 50;
    Message messages[MAX_MESSAGES];
    int messageCount;
    string ownerID;
    int unreadCount;
    bool notificationsEnabled;

public:
    Inbox() : messageCount(0), ownerID(""), unreadCount(0), notificationsEnabled(true) {}

    explicit Inbox(const string& id)
        : messageCount(0), ownerID(id), unreadCount(0), notificationsEnabled(true) {}

    int getMessageCount() const { return messageCount; }
    int getUnreadCount() const { return unreadCount; }
    string getOwnerID() const { return ownerID; }
    void setOwnerID(const string& id) { ownerID = id; }
    void setNotifications(bool e) { notificationsEnabled = e; }

    static int getTotalMessagesEverSent() { return totalMessagesEverSent; }

    bool addMessage(const Message& msg) {
        if (messageCount >= MAX_MESSAGES) { cout << "Inbox full!" << endl; return false; }
        messages[messageCount++] = msg;
        unreadCount++;
        totalMessagesEverSent++;
        return true;
    }

    void markAllRead() {
        for (int i = 0; i < messageCount; i++) {
            if (!messages[i].getIsRead()) { messages[i].markAsRead(); unreadCount--; }
        }
    }

    void displayAll() const {
        if (messageCount == 0) { cout << "No messages." << endl; return; }
        for (int i = 0; i < messageCount; i++) messages[i].display();
    }

    void displayUnread() const {
        bool found = false;
        for (int i = 0; i < messageCount; i++) {
            if (!messages[i].getIsRead()) { messages[i].display(); found = true; }
        }
        if (!found) cout << "No unread messages." << endl;
    }
};

int Inbox::totalMessagesEverSent = 0;

class User : public IReportable {
protected:
    static int totalUsers;
    const int MAX_USERNAME_LENGTH;
    string userID;
    string username;
    string passwordHash;
    bool isActive;
    int accountAge;
    Address address;
    ContactInfo contactInfo;
    Inbox inbox;

public:
    User() : MAX_USERNAME_LENGTH(50), userID(""), username(""), passwordHash(""),
             isActive(true), accountAge(0), inbox("") { totalUsers++; }

    User(const string& id, const string& name, const string& pwd,
         const Address& addr, const ContactInfo& contact)
        : MAX_USERNAME_LENGTH(50), userID(id), username(name), passwordHash(pwd),
          isActive(true), accountAge(0), address(addr), contactInfo(contact), inbox(id) {
        totalUsers++;
    }

    User(const User& other)
        : MAX_USERNAME_LENGTH(50), userID(other.userID + "_copy"), username(other.username),
          passwordHash(other.passwordHash), isActive(other.isActive), accountAge(other.accountAge),
          address(other.address), contactInfo(other.contactInfo), inbox(other.userID + "_copy") {
        totalUsers++;
    }

    virtual ~User() {}

    string getUserID() const { return userID; }
    string getUsername() const { return username; }
    bool getIsActive() const { return isActive; }
    int getAccountAge() const { return accountAge; }

    void setUsername(const string& name) {
        if ((int)name.length() <= MAX_USERNAME_LENGTH) username = name;
    }
    void setActive(bool status) { isActive = status; }
    void setPassword(const string& pwd) { passwordHash = pwd; }

    static int getTotalUsers() { return totalUsers; }

    virtual void displayProfile() const {
        cout << "=== User Profile ===" << endl;
        cout << "ID: " << userID << " | Username: " << username << endl;
        cout << "Status: " << (isActive ? "Active" : "Inactive") << endl;
        cout << "Address: "; address.display();
        contactInfo.display();
    }

    virtual string getRole() const = 0;
    virtual void generateReport() const override = 0;
    virtual void showDashboard() const override = 0;

    void sendMessage(User& recipient, const string& content, const string& timestamp) {
        Message msg(userID, recipient.getUserID(), content, timestamp);
        recipient.inbox.addMessage(msg);
        cout << username << " sent a message to " << recipient.getUsername() << endl;
    }

    void viewInbox() const {
        cout << "=== Inbox for " << username << " ===" << endl;
        inbox.displayAll();
    }

    void viewUnreadMessages() { inbox.displayUnread(); inbox.markAllRead(); }
    bool isUsernameValid() const { return !username.empty() && (int)username.length() <= MAX_USERNAME_LENGTH; }
};

int User::totalUsers = 0;

class Seller : public User {
private:
    static const int MAX_LISTINGS = 20;
    Vehicle* listings[MAX_LISTINGS];
    int listingCount;
    double totalRevenue;
    double sellerRating;
    int totalSales;

public:
    Seller() : User(), listingCount(0), totalRevenue(0.0), sellerRating(0.0), totalSales(0) {
        for (int i = 0; i < MAX_LISTINGS; i++) listings[i] = nullptr;
    }

    Seller(const string& id, const string& name, const string& pwd,
           const Address& addr, const ContactInfo& contact)
        : User(id, name, pwd, addr, contact),
          listingCount(0), totalRevenue(0.0), sellerRating(0.0), totalSales(0) {
        for (int i = 0; i < MAX_LISTINGS; i++) listings[i] = nullptr;
    }

    int getListingCount() const { return listingCount; }
    double getTotalRevenue() const { return totalRevenue; }
    double getSellerRating() const { return sellerRating; }
    int getTotalSales() const { return totalSales; }
    void setSellerRating(double r) { if (r >= 0.0 && r <= 5.0) sellerRating = r; }

    string getRole() const override { return "Seller"; }

    void generateReport() const override {
        cout << "=== Seller Report: " << username << " ===" << endl;
        cout << "Listings: " << listingCount << " | Sales: " << totalSales << endl;
        cout << "Revenue: PKR " << totalRevenue << " | Rating: " << sellerRating << "/5.0" << endl;
    }

    void showDashboard() const override {
        cout << "=== SELLER DASHBOARD ===" << endl;
        cout << "Username: " << username << endl;
        cout << "Listings: " << listingCount << " | Sales: " << totalSales << endl;
        cout << "Revenue: PKR " << totalRevenue << " | Rating: " << sellerRating << "/5.0" << endl;
    }

    bool addListing(Vehicle* v) {
        if (listingCount >= MAX_LISTINGS) { cout << "Max listings reached!" << endl; return false; }
        if (v->getSellerID() == userID) {
            listings[listingCount++] = v;
            cout << "Listing added: " << v->getListingSummary() << endl;
            return true;
        }
        cout << "Vehicle does not belong to this seller!" << endl;
        return false;
    }

    bool removeListing(const string& listingID) {
        for (int i = 0; i < listingCount; i++) {
            if (listings[i]->getListingID() == listingID) {
                for (int j = i; j < listingCount - 1; j++) listings[j] = listings[j + 1];
                listings[--listingCount] = nullptr;
                cout << "Listing " << listingID << " removed." << endl;
                return true;
            }
        }
        cout << "Listing not found." << endl;
        return false;
    }

    void updateListingPrice(const string& listingID, double newPrice) {
        for (int i = 0; i < listingCount; i++) {
            if (listings[i]->getListingID() == listingID) {
                listings[i]->setPrice(newPrice);
                cout << "Price updated to PKR " << newPrice << endl;
                return;
            }
        }
        cout << "Listing not found." << endl;
    }

    void displayAllListings() const {
        cout << "=== Listings by " << username << " ===" << endl;
        if (listingCount == 0) { cout << "No listings." << endl; return; }
        for (int i = 0; i < listingCount; i++) listings[i]->displayListing();
    }

    void recordSale(double amount) { totalSales++; totalRevenue += amount; }

    void displayProfile() const override {
        User::displayProfile();
        cout << "Role: Seller | Rating: " << sellerRating << "/5 | Sales: " << totalSales << endl;
    }

    // == : same sellerID means same account
    bool operator==(const Seller& other) const { return userID == other.userID; }
    // + : combine two sellers' revenues (franchise/team reporting)
    double operator+(const Seller& other) const { return totalRevenue + other.totalRevenue; }

    // Friend stream: reads private fields for concise output
    friend ostream& operator<<(ostream& os, const Seller& s) {
        os << "[Seller] " << s.username << " | Rating: " << s.sellerRating
           << "/5 | Listings: " << s.listingCount << " | Revenue: PKR " << s.totalRevenue;
        return os;
    }

    friend double negotiateDeal(Buyer& buyer, Seller& seller, double askingPrice);
};

class Buyer : public User {
private:
    static const int MAX_FAVORITES = 10;
    Vehicle* favorites[MAX_FAVORITES];
    int favoriteCount;
    double budget;
    string preferredBrand;
    int savedSearchCount;

public:
    Buyer() : User(), favoriteCount(0), budget(0.0), preferredBrand("Any"), savedSearchCount(0) {
        for (int i = 0; i < MAX_FAVORITES; i++) favorites[i] = nullptr;
    }

    Buyer(const string& id, const string& name, const string& pwd,
          const Address& addr, const ContactInfo& contact, double bud = 0.0)
        : User(id, name, pwd, addr, contact),
          favoriteCount(0), budget(bud), preferredBrand("Any"), savedSearchCount(0) {
        for (int i = 0; i < MAX_FAVORITES; i++) favorites[i] = nullptr;
    }

    Buyer(const Buyer& other)
        : User(other), favoriteCount(0), budget(other.budget),
          preferredBrand(other.preferredBrand), savedSearchCount(other.savedSearchCount) {
        for (int i = 0; i < MAX_FAVORITES; i++) favorites[i] = nullptr;
    }

    int getFavoriteCount() const { return favoriteCount; }
    double getBudget() const { return budget; }
    string getPreferredBrand() const { return preferredBrand; }
    void setBudget(double b) { if (b >= 0) budget = b; }
    void setPreferredBrand(const string& b) { preferredBrand = b; }

    string getRole() const override { return "Buyer"; }

    void generateReport() const override {
        cout << "=== Buyer Report: " << username << " ===" << endl;
        cout << "Budget: PKR " << budget << " | Favorites: " << favoriteCount << endl;
        cout << "Preferred Brand: " << preferredBrand << endl;
    }

    void showDashboard() const override {
        cout << "=== BUYER DASHBOARD ===" << endl;
        cout << "Username: " << username << endl;
        cout << "Budget: PKR " << budget << " | Favorites: " << favoriteCount << endl;
        cout << "Preferred Brand: " << preferredBrand << endl;
    }

    bool saveFavorite(Vehicle* v) {
        if (favoriteCount >= MAX_FAVORITES) { cout << "Favorites full!" << endl; return false; }
        for (int i = 0; i < favoriteCount; i++) {
            if (favorites[i]->getListingID() == v->getListingID()) {
                cout << "Already in favorites." << endl;
                return false;
            }
        }
        favorites[favoriteCount++] = v;
        cout << "Saved: " << v->getListingSummary() << endl;
        return true;
    }

    bool removeFavorite(const string& listingID) {
        for (int i = 0; i < favoriteCount; i++) {
            if (favorites[i]->getListingID() == listingID) {
                for (int j = i; j < favoriteCount - 1; j++) favorites[j] = favorites[j + 1];
                favorites[--favoriteCount] = nullptr;
                cout << "Removed from favorites." << endl;
                return true;
            }
        }
        cout << "Not found in favorites." << endl;
        return false;
    }

    void viewFavorites() const {
        cout << "=== " << username << "'s Favorites ===" << endl;
        if (favoriteCount == 0) { cout << "No favorites." << endl; return; }
        for (int i = 0; i < favoriteCount; i++) favorites[i]->displayListing();
    }

    void contactSeller(User& seller, const string& vehicleID, const string& timestamp) {
        string msg = "Hi, I am interested in vehicle " + vehicleID + ". Is it still available?";
        sendMessage(seller, msg, timestamp);
    }

    bool isWithinBudget(const Vehicle& v) const { return v.getPrice() <= budget; }

    void displayProfile() const override {
        User::displayProfile();
        cout << "Role: Buyer | Budget: PKR " << budget << " | Favorites: " << favoriteCount << endl;
    }

    bool operator==(const Buyer& other) const { return userID == other.userID; }

    friend ostream& operator<<(ostream& os, const Buyer& b) {
        os << "[Buyer] " << b.username << " | Budget: PKR " << b.budget
           << " | Favorites: " << b.favoriteCount;
        return os;
    }

    friend double negotiateDeal(Buyer& buyer, Seller& seller, double askingPrice);
};

class Admin : public User {
private:
    string adminLevel;
    int approvalsCount;
    int removalsCount;
    bool canBanUsers;
    string department;

public:
    Admin() : User(), adminLevel("Junior"), approvalsCount(0),
              removalsCount(0), canBanUsers(false), department("Operations") {}

    Admin(const string& id, const string& name, const string& pwd,
          const Address& addr, const ContactInfo& contact,
          const string& level = "Junior", const string& dept = "Operations")
        : User(id, name, pwd, addr, contact), adminLevel(level), approvalsCount(0),
          removalsCount(0), canBanUsers(level != "Junior"), department(dept) {}

    string getAdminLevel() const { return adminLevel; }
    int getApprovalsCount() const { return approvalsCount; }
    int getRemovalsCount() const { return removalsCount; }
    bool getCanBanUsers() const { return canBanUsers; }
    void setAdminLevel(const string& lvl) { adminLevel = lvl; canBanUsers = (lvl != "Junior"); }

    string getRole() const override { return "Admin [" + adminLevel + "]"; }

    void generateReport() const override {
        cout << "=== Admin Report: " << username << " ===" << endl;
        cout << "Level: " << adminLevel << " | Dept: " << department << endl;
        cout << "Approvals: " << approvalsCount << " | Removals: " << removalsCount << endl;
        cout << "Total Users (static): " << User::getTotalUsers() << endl;
        cout << "Total Listings (static): " << Vehicle::getTotalListings() << endl;
        cout << "Total Messages (static): " << Inbox::getTotalMessagesEverSent() << endl;
    }

    void showDashboard() const override {
        cout << "=== ADMIN DASHBOARD ===" << endl;
        cout << "Admin: " << username << " | Level: " << adminLevel << endl;
        cout << "Approvals: " << approvalsCount << " | Removals: " << removalsCount << endl;
        cout << "Can Ban Users: " << (canBanUsers ? "Yes" : "No") << endl;
    }

    void approveListing(Vehicle& v) {
        if (!v.getIsApproved()) {
            v.setApproved(true); approvalsCount++;
            cout << "[ADMIN] " << v.getListingID() << " APPROVED by " << username << endl;
        } else { cout << "Already approved." << endl; }
    }

    void removeListing(Vehicle& v) {
        v.setApproved(false); removalsCount++;
        cout << "[ADMIN] " << v.getListingID() << " REMOVED by " << username << endl;
    }

    void banUser(User& user) {
        if (!canBanUsers) { cout << "Insufficient permissions." << endl; return; }
        user.setActive(false);
        cout << "[ADMIN] " << user.getUsername() << " BANNED by " << username << endl;
    }

    void reinstateUser(User& user) {
        user.setActive(true);
        cout << "[ADMIN] " << user.getUsername() << " reinstated by " << username << endl;
    }

    void displayProfile() const override {
        User::displayProfile();
        cout << "Role: Admin | Level: " << adminLevel << " | Dept: " << department << endl;
    }

    bool operator==(const Admin& other) const { return userID == other.userID; }

    friend ostream& operator<<(ostream& os, const Admin& a) {
        os << "[Admin] " << a.username << " | Level: " << a.adminLevel
           << " | Approvals: " << a.approvalsCount << " | Removals: " << a.removalsCount;
        return os;
    }

    friend void generateAuditLog(Admin& admin, Marketplace& platform);
};

class SearchEngine : public ISearchable {
private:
    Vehicle** vehiclePool;
    int poolSize;
    int lastResultCount;
    string lastSearchQuery;
    bool sortByPriceAsc;

public:
    SearchEngine() : vehiclePool(nullptr), poolSize(0), lastResultCount(0),
                     lastSearchQuery(""), sortByPriceAsc(true) {}

    SearchEngine(Vehicle** pool, int size)
        : vehiclePool(pool), poolSize(size), lastResultCount(0),
          lastSearchQuery(""), sortByPriceAsc(true) {}

    int getLastResultCount() const { return lastResultCount; }
    string getLastQuery() const { return lastSearchQuery; }
    void setPool(Vehicle** pool, int size) { vehiclePool = pool; poolSize = size; }
    void setSortOrder(bool asc) { sortByPriceAsc = asc; }

    int searchByBrand(const string& brand) const override {
        int count = 0;
        cout << "=== Results for Brand: " << brand << " ===" << endl;
        for (int i = 0; i < poolSize; i++) {
            if (vehiclePool[i] && vehiclePool[i]->getBrand() == brand && vehiclePool[i]->getIsApproved()) {
                vehiclePool[i]->displayListing(); count++;
            }
        }
        if (count == 0) cout << "No results found." << endl;
        return count;
    }

    int searchByPriceRange(double minP, double maxP) const override {
        int count = 0;
        cout << "=== Listings: PKR " << minP << " - " << maxP << " ===" << endl;
        for (int i = 0; i < poolSize; i++) {
            if (vehiclePool[i] && vehiclePool[i]->getPrice() >= minP
                && vehiclePool[i]->getPrice() <= maxP && vehiclePool[i]->getIsApproved()) {
                vehiclePool[i]->displayListing(); count++;
            }
        }
        if (count == 0) cout << "No results found." << endl;
        return count;
    }

    int searchByYear(int minY, int maxY) const override {
        int count = 0;
        cout << "=== Year Range: " << minY << " - " << maxY << " ===" << endl;
        for (int i = 0; i < poolSize; i++) {
            if (vehiclePool[i] && vehiclePool[i]->getYear() >= minY
                && vehiclePool[i]->getYear() <= maxY && vehiclePool[i]->getIsApproved()) {
                vehiclePool[i]->displayListing(); count++;
            }
        }
        return count;
    }

    void displayAllApproved() const override {
        cout << "=== All Approved Listings ===" << endl;
        int count = 0;
        for (int i = 0; i < poolSize; i++) {
            if (vehiclePool[i] && vehiclePool[i]->getIsApproved()) {
                vehiclePool[i]->displayListing(); count++;
            }
        }
        if (count == 0) cout << "No approved listings." << endl;
    }

    int searchByPriceRangeObj(const PriceRange& range) const {
        int count = 0;
        cout << "=== PriceRange Search: " << range << " ===" << endl;
        for (int i = 0; i < poolSize; i++) {
            if (vehiclePool[i] && range.contains(vehiclePool[i]->getPrice()) && vehiclePool[i]->getIsApproved()) {
                vehiclePool[i]->displayListing(); count++;
            }
        }
        if (count == 0) cout << "No results in that range." << endl;
        return count;
    }

    int searchByMaxMileage(int maxMileage) const {
        int count = 0;
        cout << "=== Low Mileage (< " << maxMileage << " km) ===" << endl;
        for (int i = 0; i < poolSize; i++) {
            if (vehiclePool[i] && vehiclePool[i]->getMileage() <= maxMileage && vehiclePool[i]->getIsApproved()) {
                vehiclePool[i]->displayListing(); count++;
            }
        }
        return count;
    }

    // dynamic_cast to filter by concrete vehicle subtype - genuinely OOP type-safe filtering
    int searchByVehicleType(const string& type) const {
        int count = 0;
        cout << "=== Vehicle Type: " << type << " ===" << endl;
        for (int i = 0; i < poolSize; i++) {
            if (!vehiclePool[i] || !vehiclePool[i]->getIsApproved()) continue;
            bool match = false;
            if (type == "Car" && dynamic_cast<Car*>(vehiclePool[i])) match = true;
            if (type == "Bike" && dynamic_cast<Bike*>(vehiclePool[i])) match = true;
            if (type == "Truck" && dynamic_cast<Truck*>(vehiclePool[i])) match = true;
            if (match) { vehiclePool[i]->displayListing(); count++; }
        }
        if (count == 0) cout << "No " << type << " listings found." << endl;
        return count;
    }
};

class Marketplace : public ISearchable {
private:
    static const string PLATFORM_NAME;
    static const int MAX_VEHICLES = 100;
    static const int MAX_SELLERS = 50;
    static const int MAX_BUYERS = 200;
    Vehicle* vehicleListings[MAX_VEHICLES];
    int vehicleCount;
    Seller* sellers[MAX_SELLERS];
    int sellerCount;
    Buyer* buyers[MAX_BUYERS];
    int buyerCount;
    SearchEngine searchEngine;
    string platformVersion;
    bool isOnline;

public:
    Marketplace() : vehicleCount(0), sellerCount(0), buyerCount(0),
                    platformVersion("1.0"), isOnline(true) {
        for (int i = 0; i < MAX_VEHICLES; i++) vehicleListings[i] = nullptr;
        for (int i = 0; i < MAX_SELLERS; i++) sellers[i] = nullptr;
        for (int i = 0; i < MAX_BUYERS; i++) buyers[i] = nullptr;
    }

    explicit Marketplace(const string& version)
        : vehicleCount(0), sellerCount(0), buyerCount(0),
          platformVersion(version), isOnline(true) {
        for (int i = 0; i < MAX_VEHICLES; i++) vehicleListings[i] = nullptr;
        for (int i = 0; i < MAX_SELLERS; i++) sellers[i] = nullptr;
        for (int i = 0; i < MAX_BUYERS; i++) buyers[i] = nullptr;
    }

    ~Marketplace() {
        for (int i = 0; i < vehicleCount; i++) { delete vehicleListings[i]; vehicleListings[i] = nullptr; }
    }

    int getVehicleCount() const { return vehicleCount; }
    int getSellerCount() const { return sellerCount; }
    int getBuyerCount() const { return buyerCount; }
    bool getIsOnline() const { return isOnline; }
    void setOnline(bool status) { isOnline = status; }

    static string getPlatformName() { return PLATFORM_NAME; }

    bool addVehicleListing(Vehicle* v) {
        if (vehicleCount >= MAX_VEHICLES) { cout << "Platform at capacity!" << endl; return false; }
        vehicleListings[vehicleCount++] = v;
        searchEngine.setPool(vehicleListings, vehicleCount);
        cout << "Submitted: " << v->getListingSummary() << endl;
        return true;
    }

    bool deleteVehicleListing(const string& listingID) {
        for (int i = 0; i < vehicleCount; i++) {
            if (vehicleListings[i] && vehicleListings[i]->getListingID() == listingID) {
                delete vehicleListings[i];
                for (int j = i; j < vehicleCount - 1; j++) vehicleListings[j] = vehicleListings[j + 1];
                vehicleListings[--vehicleCount] = nullptr;
                searchEngine.setPool(vehicleListings, vehicleCount);
                cout << "Listing " << listingID << " deleted." << endl;
                return true;
            }
        }
        cout << "Listing not found." << endl;
        return false;
    }

    Vehicle* findVehicle(const string& listingID) {
        for (int i = 0; i < vehicleCount; i++) {
            if (vehicleListings[i] && vehicleListings[i]->getListingID() == listingID)
                return vehicleListings[i];
        }
        return nullptr;
    }

    bool updateVehiclePrice(const string& listingID, double newPrice) {
        Vehicle* v = findVehicle(listingID);
        if (v) { v->setPrice(newPrice); cout << "Price updated." << endl; return true; }
        cout << "Vehicle not found." << endl;
        return false;
    }

    void registerSeller(Seller* s) {
        if (sellerCount < MAX_SELLERS) { sellers[sellerCount++] = s; cout << "Seller registered: " << s->getUsername() << endl; }
    }

    void registerBuyer(Buyer* b) {
        if (buyerCount < MAX_BUYERS) { buyers[buyerCount++] = b; cout << "Buyer registered: " << b->getUsername() << endl; }
    }

    int searchByBrand(const string& brand) const override { return searchEngine.searchByBrand(brand); }
    int searchByPriceRange(double minP, double maxP) const override { return searchEngine.searchByPriceRange(minP, maxP); }
    int searchByYear(int minY, int maxY) const override { return searchEngine.searchByYear(minY, maxY); }
    void displayAllApproved() const override { searchEngine.displayAllApproved(); }
    void searchByPriceRangeObj(const PriceRange& range) const { searchEngine.searchByPriceRangeObj(range); }
    void searchByMaxMileage(int maxMileage) const { searchEngine.searchByMaxMileage(maxMileage); }
    void searchByVehicleType(const string& type) const { searchEngine.searchByVehicleType(type); }

    void displayPlatformStats() const {
        cout << "\n====== " << PLATFORM_NAME << " v" << platformVersion << " ======" << endl;
        cout << "Status: " << (isOnline ? "Online" : "Offline") << endl;
        cout << "Sellers: " << sellerCount << " | Buyers: " << buyerCount << " | Listings: " << vehicleCount << endl;
        cout << "All-time Users: " << User::getTotalUsers() << endl;
        cout << "All-time Listings Created: " << Vehicle::getTotalListings() << endl;
        cout << "Total Messages Sent: " << Inbox::getTotalMessagesEverSent() << endl;
        cout << "==============================\n" << endl;
    }

    void showWelcomeBanner() const {
        cout << "\n*** Welcome to " << PLATFORM_NAME << " v" << platformVersion << " ***" << endl;
        cout << "Pakistan's Leading Car Marketplace" << endl;
        cout << "Status: " << (isOnline ? "LIVE" : "OFFLINE") << "\n" << endl;
    }

    friend void generateAuditLog(Admin& admin, Marketplace& platform);
};

const string Marketplace::PLATFORM_NAME = "PakWheels-OOP";

// Friend Function 1: negotiateDeal
// Friend of both Buyer and Seller
// Accesses Buyer::budget (private) and Seller::sellerRating,
// Seller::totalRevenue (private)
// Justification: price negotiation is a bilateral operation that needs private data from both sides. Adding public getters just
// for this would expose internals with no other purpose.

double negotiateDeal(Buyer& buyer, Seller& seller, double askingPrice) {
    double budgetRatio = buyer.budget / askingPrice;
    double sellerFirmness = seller.sellerRating / 5.0;
    double budgetStress = (budgetRatio >= 1.0) ? 0.0 : (1.0 - budgetRatio);
    double discount = (1.0 - sellerFirmness) * 0.08 + budgetStress * 0.05;
    if (discount > 0.15) discount = 0.15;
    double offer = askingPrice * (1.0 - discount);
    cout << "\n--- Price Negotiation ---" << endl;
    cout << "Asking Price:  PKR " << askingPrice << endl;
    cout << "Buyer Budget:  PKR " << buyer.budget << endl;
    cout << "Seller Rating: " << seller.sellerRating << "/5" << endl;
    cout << "Discount:      " << (discount * 100) << "%" << endl;
    cout << "Fair Offer:    PKR " << offer << endl;
    return offer;
}

// Friend Function 2: generateAuditLog
// Friend of both Admin and Marketplace
// Accesses Admin's private action counts and Marketplace's
// private operational data without exposing them publicly
// Justification: an audit log is a compliance report that crosses both class boundaries. Neither class should expose
// raw internal counts just to support this one operation.

void generateAuditLog(Admin& admin, Marketplace& platform) {
    cout << "\n========== AUDIT LOG ==========" << endl;
    cout << "Generated by: " << admin.username << " [" << admin.adminLevel << "] | Dept: " << admin.department << endl;
    cout << "Platform: " << Marketplace::PLATFORM_NAME << " v" << platform.platformVersion << endl;
    cout << "Online: " << (platform.isOnline ? "Yes" : "No") << endl;
    cout << "Active Listings: " << platform.vehicleCount << endl;
    cout << "Registered Sellers: " << platform.sellerCount << endl;
    cout << "Registered Buyers: " << platform.buyerCount << endl;
    cout << "Admin Approvals This Session: " << admin.approvalsCount << endl;
    cout << "Admin Removals This Session: " << admin.removalsCount << endl;
    cout << "Total System Messages: " << Inbox::getTotalMessagesEverSent() << endl;
    cout << "================================\n" << endl;
}

// this is the main funciton :)
int main() {
    Marketplace platform("2.0");
    platform.showWelcomeBanner();

    Address karachi("Karachi", "Sindh", "Pakistan", "75600", "Block 4 PECHS");
    Address lahore("Lahore", "Punjab", "Pakistan", "54000", "DHA Phase 5");
    Address islamabad("Islamabad", "ICT", "Pakistan", "44000", "F-7 Markaz");
    Address peshawar("Peshawar", "KPK", "Pakistan", "25000", "Hayatabad Phase 3");

    ContactInfo sellerContact1("ahmed@email.com", "03001234567");
    ContactInfo sellerContact2("ali@email.com", "03111234567");
    ContactInfo buyerContact("sara@email.com", "03219876543");
    ContactInfo adminContact("admin@pakwheels.com", "02134567890");

    sellerContact1.verifyEmail();
    sellerContact1.verifyPhone();
    sellerContact2.verifyEmail();

    Seller ahmed("SL001", "Ahmed_Khan", "pwd_hash_1", karachi, sellerContact1);
    Seller ali("SL002", "Ali_Raza", "pwd_hash_2", lahore, sellerContact2);
    Buyer sara("BY001", "Sara_Malik", "pwd_hash_3", islamabad, buyerContact, 3500000.0);
    Buyer kamran("BY002", "Kamran_Baig", "pwd_hash_4", peshawar, buyerContact, 1200000.0);

    Buyer guestSara(sara);
    cout << "[Copy Constructor] Guest profile created from: " << sara.getUsername() << endl;

    Admin superAdmin("AD001", "Admin_User", "admin_hash", karachi, adminContact, "Super", "Platform");

    platform.registerSeller(&ahmed);
    platform.registerSeller(&ali);
    platform.registerBuyer(&sara);
    platform.registerBuyer(&kamran);

    VehicleSpecs civicSpecs(1800, "Automatic", "Petrol", "Platinum White", 5, "Sedan");
    VehicleSpecs fortunerSpecs(2700, "Automatic", "Diesel", "Pearl Black", 7, "SUV");
    VehicleSpecs cbr500Specs(500, "Manual", "Petrol", "Repsol Red", 2, "Sports");
    VehicleSpecs cd70Specs(70, "Manual", "Petrol", "Black", 2, "Standard");
    VehicleSpecs hinoSpecs(7400, "Manual", "Diesel", "White", 3, "Truck");
    VehicleSpecs corollaSpecs(1800, "Automatic", "Petrol", "Silver", 5, "Sedan");

    Car* civic = new Car("VH001", "Honda", "Civic", 2021, 2200000, 35000,
                         "Used", "SL001", civicSpecs, true, 6, "Android Auto + Apple CarPlay", 5, 4);
    Car* fortuner = new Car("VH002", "Toyota", "Fortuner", 2020, 9500000, 55000,
                            "Used", "SL001", fortunerSpecs, false, 7, "Toyota Touch2", 4, 4);
    Car* corolla = new Car("VH004", "Toyota", "Corolla", 2022, 3400000, 10000,
                           "Used", "SL002", corollaSpecs, false, 4, "Multimedia Screen", 4, 4);
    Bike* cbr500 = new Bike("VH003", "Honda", "CBR500R", 2022, 850000, 5000,
                            "Used", "SL002", cbr500Specs, "Sports", false, 11000, true, 189.0);
    Bike* cd70 = new Bike("VH005", "Honda", "CD70", 2023, 165000, 2000,
                          "New", "SL001", cd70Specs, "Standard", true, 6500, false, 95.0);
    Truck* hino = new Truck("VH006", "Hino", "500Series", 2019, 7500000, 120000,
                            "Used", "SL002", hinoSpecs, 8.5, 6, true, "Heavy", 12000.0);

    cout << "\n=== POLYMORPHISM: displayDetails() via base pointer ===" << endl;
    Vehicle* polyDemo[] = {civic, cbr500, hino};
    for (int i = 0; i < 3; i++) {
        polyDemo[i]->displayDetails();
        cout << "Summary: " << polyDemo[i]->getListingSummary() << "\n" << endl;
    }

    cout << "\n=== OPERATOR OVERLOADING: Vehicle ==, <, >, << ===" << endl;
    cout << "civic == civic: " << (*civic == *civic ? "true" : "false") << endl;
    cout << "civic == corolla: " << (*civic == *corolla ? "true" : "false") << endl;
    cout << "civic < fortuner (price): " << (*civic < *fortuner ? "true" : "false") << endl;
    cout << "fortuner > corolla (price): " << (*fortuner > *corolla ? "true" : "false") << endl;
    cout << "Stream (<<): " << *civic << endl;

    cout << "\n=== OPERATOR OVERLOADING: PriceRange +, ==, <, >, << ===" << endl;
    PriceRange budget(500000, 1500000, "Budget");
    PriceRange mid(1500000, 3500000, "Mid");
    PriceRange stretch(3500000, 6000000, "Stretch");
    PriceRange merged = budget + mid;
    cout << "Budget: " << budget << endl;
    cout << "Mid:    " << mid << endl;
    cout << "Merged: " << merged << endl;
    cout << "budget == mid: " << (budget == mid ? "true" : "false") << endl;
    cout << "budget < mid: " << (budget < mid ? "true" : "false") << endl;
    cout << "stretch > mid: " << (stretch > mid ? "true" : "false") << endl;

    cout << "\n=== OPERATOR OVERLOADING: Seller ==, +, << ===" << endl;
    ahmed.recordSale(2200000);
    ali.recordSale(850000);
    cout << "ahmed == ahmed: " << (ahmed == ahmed ? "true" : "false") << endl;
    cout << "ahmed == ali: " << (ahmed == ali ? "true" : "false") << endl;
    cout << "Combined revenue: PKR " << (ahmed + ali) << endl;
    cout << "Stream: " << ahmed << endl;

    cout << "\n=== OPERATOR OVERLOADING: Message ==, << ===" << endl;
    Message m1("BY001", "SL001", "Is the Civic available?", "2024-01-15 10:30");
    Message m2("BY001", "SL001", "Is the Civic available?", "2024-01-15 10:30");
    Message m3("BY001", "SL001", "What is the final price?", "2024-01-15 10:45");
    cout << "m1 == m2 (same event): " << (m1 == m2 ? "true" : "false") << endl;
    cout << "m1 == m3 (diff event): " << (m1 == m3 ? "true" : "false") << endl;
    cout << "Stream: " << m1 << endl;

    cout << "\n=== OPERATOR OVERLOADING: Buyer ==, << ===" << endl;
    cout << "sara == kamran: " << (sara == kamran ? "true" : "false") << endl;
    cout << "Stream: " << sara << endl;

    cout << "\n=== REGISTERING VEHICLES ===" << endl;
    ahmed.addListing(civic); ahmed.addListing(fortuner); ahmed.addListing(cd70);
    ali.addListing(cbr500); ali.addListing(corolla); ali.addListing(hino);
    platform.addVehicleListing(civic); platform.addVehicleListing(fortuner);
    platform.addVehicleListing(cbr500); platform.addVehicleListing(corolla);
    platform.addVehicleListing(cd70); platform.addVehicleListing(hino);

    cout << "\n=== ADMIN APPROVALS ===" << endl;
    Vehicle* v1 = platform.findVehicle("VH001");
    Vehicle* v2 = platform.findVehicle("VH002");
    Vehicle* v3 = platform.findVehicle("VH003");
    Vehicle* v4 = platform.findVehicle("VH004");
    Vehicle* v5 = platform.findVehicle("VH005");
    Vehicle* v6 = platform.findVehicle("VH006");
    if (v1) superAdmin.approveListing(*v1);
    if (v2) superAdmin.approveListing(*v2);
    if (v3) superAdmin.approveListing(*v3);
    if (v4) superAdmin.approveListing(*v4);
    if (v5) superAdmin.approveListing(*v5);
    if (v6) superAdmin.approveListing(*v6);
    if (v6) superAdmin.removeListing(*v6);

    cout << "\n=== SEARCH: By Brand ===" << endl;
    platform.searchByBrand("Honda");

    cout << "\n=== SEARCH: By Price Range ===" << endl;
    platform.searchByPriceRange(100000, 1000000);

    cout << "\n=== SEARCH: By Year ===" << endl;
    platform.searchByYear(2021, 2023);

    cout << "\n=== SEARCH: By Vehicle Type (Bike) ===" << endl;
    platform.searchByVehicleType("Bike");

    cout << "\n=== SEARCH: By PriceRange Object ===" << endl;
    platform.searchByPriceRangeObj(merged);

    cout << "\n=== SEARCH: Low Mileage ===" << endl;
    platform.searchByMaxMileage(20000);

    cout << "\n=== BUYER FAVORITES ===" << endl;
    if (v1) sara.saveFavorite(v1);
    if (v4) sara.saveFavorite(v4);
    if (v3) kamran.saveFavorite(v3);
    if (v5) kamran.saveFavorite(v5);
    sara.viewFavorites();
    kamran.viewFavorites();

    cout << "\n=== MESSAGING ===" << endl;
    sara.contactSeller(ahmed, "VH001", "2024-01-15 10:30");
    ahmed.sendMessage(sara, "Yes it is available. Come for a test drive!", "2024-01-15 10:45");
    kamran.contactSeller(ali, "VH003", "2024-01-15 11:00");
    sara.viewInbox();

    cout << "\n=== FRIEND FUNCTION 1: negotiateDeal ===" << endl;
    ahmed.setSellerRating(4.2);
    double fairPrice = negotiateDeal(sara, ahmed, 2200000.0);
    cout << "Negotiated offer for Sara on Civic: PKR " << fairPrice << endl;
    ali.setSellerRating(2.5);
    double fairPrice2 = negotiateDeal(kamran, ali, 850000.0);
    cout << "Negotiated offer for Kamran on CBR500: PKR " << fairPrice2 << endl;

    cout << "\n=== FRIEND FUNCTION 2: generateAuditLog ===" << endl;
    generateAuditLog(superAdmin, platform);

    cout << "\n=== ABSTRACTION: IReportable polymorphic dispatch ===" << endl;
    IReportable* reporters[] = {&ahmed, &sara, &superAdmin};
    string names[] = {"Seller Ahmed", "Buyer Sara", "Admin"};
    for (int i = 0; i < 3; i++) {
        cout << "\n--- " << names[i] << " ---" << endl;
        reporters[i]->generateReport();
        reporters[i]->showDashboard();
    }

    cout << "\n=== ABSTRACTION: ISearchable interface ===" << endl;
    ISearchable* searcher = &platform;
    cout << "Via ISearchable pointer, search Toyota:" << endl;
    searcher->searchByBrand("Toyota");

    cout << "\n=== UPDATE: Price Change ===" << endl;
    platform.updateVehiclePrice("VH001", 2050000);
    ahmed.updateListingPrice("VH001", 2050000);

    cout << "\n=== DELETE: Remove Fortuner ===" << endl;
    ahmed.removeListing("VH002");
    platform.deleteVehicleListing("VH002");

    cout << "\n=== STATIC MEMBERS ===" << endl;
    cout << "Platform Name:          " << Marketplace::getPlatformName() << endl;
    cout << "Total Users Created:    " << User::getTotalUsers() << endl;
    cout << "Total Listings Created: " << Vehicle::getTotalListings() << endl;
    cout << "Total Messages Sent:    " << Inbox::getTotalMessagesEverSent() << endl;

    cout << "\n=== Car-specific polymorphism ===" << endl;
    Car* c = dynamic_cast<Car*>(platform.findVehicle("VH001"));
    if (c) { c->displaySafetyInfo(); cout << "Is Premium: " << (c->isPremium() ? "Yes" : "No") << endl; }

    cout << "\n=== Bike-specific polymorphism ===" << endl;
    Bike* b = dynamic_cast<Bike*>(platform.findVehicle("VH003"));
    if (b) { b->displayPerformanceInfo(); cout << "Is Sports: " << (b->isSportsCategory() ? "Yes" : "No") << endl; }

    platform.displayPlatformStats();
    cout << "=== Program Complete ===" << endl;
    return 0;
}
