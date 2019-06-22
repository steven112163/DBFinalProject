#ifndef WHERE_CONDITIONS_H
#define WHERE_CONDITIONS_H

#include "User.h"

class WhereConditions {
private:
    std::string firstField;
    std::string firstData;
    std::string firstOp;
    std::string secondField;
    std::string secondData;
    std::string secondOp;
    std::string op;

    bool getPartialResult(User_t *user, const std::string &field, double data, std::string op);

    static bool compare(unsigned int target, double data, std::string op);
    bool getPartialResult(User_t *user, std::string field, std::string data, std::string op);

    static bool compare(std::string target, std::string data, std::string op);

public:
    WhereConditions(std::vector<std::string> conditions);
    bool getResult(User_t *user);
};

#endif
