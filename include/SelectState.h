#ifndef SELECT_STATE_H
#define SELECT_STATE_H

#include "Command.h"
#include "Table.h"

//void field_state_handler(Command_t *cmd, size_t arg_idx);
void field_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table);
//void table_state_handler(Command_t *cmd, size_t arg_idx);
void table_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table);
void where_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table);
void offset_state_handler(Command_t *cmd, size_t arg_idx);
void limit_state_handler(Command_t *cmd, size_t arg_idx);

class WhereConditions {
private:
    std::string firstField;
    std::string firstData;
    std::string firstOp;
    std::string secondField;
    std::string secondData;
    std::string secondOp;
    std::string op;
    size_t findOp(std::string condition, std::string& op);
    bool getPartialResult(User_t *user, std::string field, double data, std::string op);
    bool compare(unsigned int target, double data, std::string op);
    bool getPartialResult(User_t *user, std::string field, std::string data, std::string op);
    bool compare(std::string target, std::string data, std::string op);

public:
    WhereConditions(std::string conditions);
    bool getResult(User_t *user);
};

#endif
