#ifndef ISEARCHABLE_H
#define ISEARCHABLE_H

#include <string>
using namespace std;

// Abstract interface: any class that supports search/filter operations
// SearchEngine and Marketplace both implement this contract
class ISearchable {
public:
    virtual int searchByBrand(const string& brand) const = 0;
    virtual int searchByPriceRange(double minP, double maxP) const = 0;
    virtual int searchByYear(int minY, int maxY) const = 0;
    virtual void displayAllApproved() const = 0;
    virtual ~ISearchable() {}
};

#endif
