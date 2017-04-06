#include "tools.hpp"

class abyssTestSuite : public testSuite {

public:
    virtual std::string suiteName();
    virtual void runtests(unsigned int const);
};
