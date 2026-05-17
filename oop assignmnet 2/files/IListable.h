#ifndef ILISTABLE_H
#define ILISTABLE_H

#include <string>
using namespace std;

// Abstract interface: any entity that can be displayed as a listing
// Car, Bike, Truck all implement this differently (polymorphism via abstraction)
class IListable {
public:
    virtual void displayDetails() const = 0;
    virtual string getListingSummary() const = 0;
    virtual double getPrice() const = 0;
    virtual string getListingID() const = 0;
    virtual ~IListable() {}
};

#endif
