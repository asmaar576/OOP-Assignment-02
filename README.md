# Assignment 02 - OOP 
(BAI-2C) - 25K-0057
**FAST NUCES Karachi | Spring 2026**
**Student ID: 25K-0057**

---

## Files submitted

```
BAI2C-25k-0057/
├── IListable.h
├── ISearchable.h
├── IReportable.h
└── CarMarketplace_A2.cpp
```

The 3 header files are for the abstract classes only (as required). Everything else is in the single cpp file.

---

## 1. Inheritance

6 relationships implemented:

- **IListable <- Vehicle <- Car**
  Car is a vehicle that can be listed. I added car-specific stuff like number of doors, airbag count, sunroof, infotainment system and safety rating since these dont apply to bikes or trucks.

- **IListable <- Vehicle <- Bike**
  Bike inherits vehicle but has its own attributes like bikeType (Sports/Standard etc), maxRPM, weight in kg and cargo rack. None of these make sense on a car.

- **IListable <- Vehicle <- Truck**
  Truck needed payload capacity in tons, number of axles, refrigeration support and towing capacity. Very different from a car listing.

- **IReportable <- User <- Seller**
  Seller is a type of user. IReportable is inherited by User which forces Seller to implement generateReport() and showDashboard(). A seller needs to show their listings, revenue and rating.

- **IReportable <- User <- Buyer**
  Same as above. Buyer is also a user but shows budget and favorites instead.

- **IReportable <- User <- Admin**
  Admin is also a user but with extra permissions like banning users and approving/removing listings.

---

## 2. Polymorphism

The main example is `displayDetails()` which is pure virtual in Vehicle and overridden differently in each subclass.

Car prints: doors, airbags, sunroof, safety rating
Bike prints: type, weight, max RPM, kickstart type
Truck prints: category, payload, axles, refrigeration

This is called in main through a base class pointer array:

```cpp
Vehicle* polyDemo[] = {civic, cbr500, hino};
for (int i = 0; i < 3; i++)
    polyDemo[i]->displayDetails();
```

Same concept used for IReportable:

```cpp
IReportable* reporters[] = {&ahmed, &sara, &superAdmin};
reporters[i]->generateReport();
reporters[i]->showDashboard();
```

And ISearchable:

```cpp
ISearchable* searcher = &platform;
searcher->searchByBrand("Toyota");
```

---

## 3. Abstraction

3 abstract classes, each in its own header file as required.

**IListable.h** - for anything that can appear as a listing on the platform
```cpp
virtual void displayDetails() const = 0;
virtual string getListingSummary() const = 0;
virtual double getPrice() const = 0;
virtual string getListingID() const = 0;
```

**ISearchable.h** - for anything that supports search operations
```cpp
virtual int searchByBrand(const string& brand) const = 0;
virtual int searchByPriceRange(double minP, double maxP) const = 0;
virtual int searchByYear(int minY, int maxY) const = 0;
virtual void displayAllApproved() const = 0;
```

**IReportable.h** - for any user type that needs to show reports
```cpp
virtual void generateReport() const = 0;
virtual void showDashboard() const = 0;
```

The headers only contain the interface (pure virtual functions). The actual implementations are in the .cpp file inside each concrete class. This is the point of abstraction - you hide the details and only expose what is necessary.

---

## 4. Operator Overloading

I overloaded operators on 4 classes. Each one has a proper reason behind it.

**Vehicle: ==, <, >, <<**
- `==` compares by listingID not memory address, because two vehicle objects can represent the same listing
- `<` and `>` compare by price, useful when sorting search results
- `<<` prints a one line summary for debugging

**PriceRange: +, ==, <, >, <<**
- `+` merges two price ranges into the widest possible span. For example a buyer might have a comfortable range and a stretch range and want to search both at once
```cpp
PriceRange budget(500000, 1500000, "Budget");
PriceRange mid(1500000, 3500000, "Mid");
PriceRange merged = budget + mid;
platform.searchByPriceRangeObj(merged);
```
- `==` checks if two ranges cover the exact same window
- `<` and `>` compare ranges by their midpoint value

**Seller: ==, +, <<**
- `==` checks if two seller objects are the same account (by userID)
- `+` adds up two sellers revenues, useful if you want a combined total for a team or franchise
- `<<` prints seller info in one line

**Message: ==, <<**
- `==` checks sender + receiver + timestamp. If all three match it is the same message event (useful for deduplication)

**Buyer: ==, <<**
- `==` compares by userID

---

## 5. Friend Functions

I used 2 friend functions. Both have a genuine reason for being friend.

**negotiateDeal(Buyer& buyer, Seller& seller, double askingPrice)**

This is friend of both Buyer and Seller. It directly accesses:
- `buyer.budget` (private)
- `seller.sellerRating` (private)
- `seller.totalRevenue` (private)

Why friend: price negotiation needs private info from both sides at the same time. It doesnt make sense to put this logic inside Buyer (it doesnt know seller details) or inside Seller (it doesnt know buyer budget). Adding public getters just for this one function would expose financial data unnecessarily.

Sample output:
```
Asking Price:  PKR 850000
Buyer Budget:  PKR 1200000
Seller Rating: 2.5/5
Discount:      4%
Fair Offer:    PKR 816000
```

**generateAuditLog(Admin& admin, Marketplace& platform)**

This is friend of both Admin and Marketplace. It reads:
- `admin.approvalsCount`, `admin.removalsCount`, `admin.adminLevel`, `admin.department`
- `platform.vehicleCount`, `platform.sellerCount`, `platform.buyerCount`, `platform.isOnline`

Why friend: an audit log is basically a report that needs data from both classes together. Neither class should make these internal counts public just to support one report function.

Sample output:
```
========== AUDIT LOG ==========
Generated by: Admin_User [Super]
Active Listings: 6
Approvals: 6  |  Removals: 1
Total Messages Sent: 3
================================
```

---

## 6. Integration with Assignment 01

Everything from A1 is still there, just extended:

- Vehicle from A1 is now abstract. It got split into Car, Bike and Truck each with their own displayDetails().
- User from A1 now also inherits IReportable so all user types are forced to implement reporting.
- SearchEngine from A1 now implements ISearchable. Also added searchByVehicleType() which uses dynamic_cast to filter Cars, Bikes or Trucks from a mixed Vehicle* array.
- Marketplace from A1 now implements ISearchable too. Also changed to use heap allocated Vehicle* so polymorphism works properly. Destructor deletes all owned vehicles.
- Message from A1 got == and << operators added.
- All static members (totalUsers, totalListings, totalMessagesEverSent) are still there.
- All composition and aggregation relationships from A1 are preserved.
