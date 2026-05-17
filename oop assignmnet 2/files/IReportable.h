#ifndef IREPORTABLE_H
#define IREPORTABLE_H

// Abstract interface: any entity that can produce a summary/report
// Admin and Seller both implement this to show their stats
class IReportable {
public:
    virtual void generateReport() const = 0;
    virtual void showDashboard() const = 0;
    virtual ~IReportable() {}
};

#endif
