#include "WhereConditions.h"

WhereConditions::WhereConditions(std::vector<std::string> conditions) {
    if (conditions.size() > 3) {
        this->firstField = conditions[0];
        this->firstOp = conditions[1];
        this->firstData = conditions[2];
        this->op = conditions[3];
        this->secondField = conditions[4];
        this->secondOp = conditions[5];
        this->secondData = conditions[6];
    } else {
        this->firstField = conditions[0];
        this->firstOp = conditions[1];
        this->firstData = conditions[2];
        this->op = "";
        this->secondField = "";
        this->secondOp = "";
        this->secondData = "";
    }
}

bool WhereConditions::getResult(User_t *user) {
    bool firstResult;
    if (this->firstField == "id" || this->firstField == "age") {
        double data = stod(this->firstData);
        firstResult = this->getPartialResult(user, this->firstField, data, this->firstOp);
    } else {
        firstResult = this->getPartialResult(user, this->firstField, this->firstData, this->firstOp);
    }

    if (this->secondField.empty())
        return firstResult;
    
    bool secondResult;
    if (this->secondField == "id" || this->secondField == "age") {
        double data = stod(this->secondData);
        secondResult = this->getPartialResult(user, this->secondField, data, this->secondOp);
    } else {
        secondResult = this->getPartialResult(user, this->secondField, this->secondData, this->secondOp);
    }
    
    if (this->op == "or")
        return (firstResult || secondResult);
    else
        return (firstResult && secondResult);
}

bool WhereConditions::getPartialResult(User_t *user, const std::string &field, double data, std::string op) {
    if (field == "id")
        return this->compare(user->id, data, op);
    else
        return this->compare(user->age, data, op);
}

bool WhereConditions::compare(unsigned int target, double data, std::string op) {
    if (op == "=")
        return (double)target == data;
    else if (op == "!=")
        return (double)target != data;
    else if (op == ">")
        return (double)target > data;
    else if (op == "<")
        return (double)target < data;
    else if (op == ">=")
        return (double)target >= data;
    else
        return (double)target <= data;
}

bool WhereConditions::getPartialResult(User_t *user, std::string field, std::string data, std::string op) {
    if (field == "name") {
        std::string name = user->name;
        return this->compare(name, data, op);
    } else {
        std::string email = user->email;
        return this->compare(email, data, op);
    }
}

bool WhereConditions::compare(std::string target, std::string data, std::string op) {
    if (op == "=")
        return target == data;
    else
        return target != data;
}
