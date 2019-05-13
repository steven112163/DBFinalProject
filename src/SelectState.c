#include <string.h>
#include <stdlib.h>
#include "Command.h"
#include "SelectState.h"

//void field_state_handler(Command_t *cmd, size_t arg_idx) {
void field_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table) {
    cmd->cmd_args.sel_args.fields = NULL;
    cmd->cmd_args.sel_args.fields_len = 0;
    cmd->cmd_args.sel_args.limit = -1;
    cmd->cmd_args.sel_args.offset = -1;
    cmd->cmd_args.sel_args.idxList.clear();
    cmd->cmd_args.sel_args.idxListLen = 0;
    while(arg_idx < cmd->args_len) {
        if (cmd->args[arg_idx] == "*") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "id") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "name") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "email") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "age") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "from") {
            table_state_handler(cmd, arg_idx+1, table);
            return;
        } else {
            cmd->type = UNRECOG_CMD;
            return;
        }
        arg_idx += 1;
    }
    cmd->type = UNRECOG_CMD;
    return;
}

//void table_state_handler(Command_t *cmd, size_t arg_idx) {
void table_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table) {
    if (arg_idx < cmd->args_len && (cmd->args[arg_idx] == "table")) {

        arg_idx++;
        if (arg_idx == cmd->args_len) {
            return;
        } else if (cmd->args[arg_idx] == "where") {
            where_state_handler(cmd, arg_idx+1, table);
            return;
        } else if (cmd->args[arg_idx] == "offset") {
            offset_state_handler(cmd, arg_idx+1);
            return;
        } else if (cmd->args[arg_idx] == "limit") {
            limit_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void where_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table) {
    if (arg_idx < cmd->args_len) {
        size_t next_arg;
        for (next_arg = arg_idx; next_arg < cmd->args_len; next_arg++)
            if (cmd->args[next_arg] == "offset" ||
                cmd->args[next_arg] == "limit")
                break;
        
        std::string conditions = cmd->args[arg_idx];
        size_t next_idx;
        for (next_idx = arg_idx + 1; next_idx < next_arg; next_idx++)
            conditions += cmd->args[next_idx];
        WhereConditions whereConditions(conditions);
        
        size_t idx;
        for (idx = 0; idx < table->len; idx++) {
            User_t *user = get_User(table, idx);
            if (whereConditions.getResult(user)) {
                cmd->cmd_args.sel_args.idxList.push_back(idx);
                cmd->cmd_args.sel_args.idxListLen++;
            }
        }
        if (cmd->cmd_args.sel_args.idxListLen == 0)
            cmd->cmd_args.sel_args.idxListLen = 1;
        
        if (next_arg == cmd->args_len) {
            return;
        }
        if (next_arg < cmd->args_len && cmd->args[next_arg] == "offset") {
            offset_state_handler(cmd, next_arg+1);
            return;
        } else if (next_arg < cmd->args_len && cmd->args[next_arg] == "limit") {
            limit_state_handler(cmd, next_arg+1);
            return;
        }
            
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void offset_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.offset = atoi(cmd->args[arg_idx].c_str());

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        } else if (arg_idx < cmd->args_len && cmd->args[arg_idx] == "limit") {

            limit_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void limit_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.limit = atoi(cmd->args[arg_idx].c_str());

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

WhereConditions::WhereConditions(std::string conditions) {
    size_t orLoc = conditions.find("or");
    size_t andLoc = conditions.find("and");
    
    std::string firstCondition, secondCondition;
    if (orLoc != std::string::npos) {
        this->op = "or";
        firstCondition = conditions.substr(0, orLoc);
        secondCondition = conditions.substr(orLoc + 2, std::string::npos);
    } else if (andLoc != std::string::npos) {
        this->op = "and";
        firstCondition = conditions.substr(0, andLoc);
        secondCondition = conditions.substr(andLoc + 3, std::string::npos);
    } else {
        this->op = "";
        size_t opLoc = this->findOp(conditions, this->firstOp);
        this->firstField = conditions.substr(0, opLoc);
        this->firstData = conditions.substr(opLoc + this->firstOp.length(), std::string::npos);
    }
    
    if (orLoc != std::string::npos || andLoc != std::string::npos) {
        size_t firstOpLoc = this->findOp(firstCondition, this->firstOp);
        this->firstField = firstCondition.substr(0, firstOpLoc);
        this->firstData = firstCondition.substr(firstOpLoc + this->firstOp.length(), std::string::npos);
        
        size_t secondOpLoc = this->findOp(secondCondition, this->secondOp);
        this->secondField = secondCondition.substr(0, secondOpLoc);
        this->secondData = secondCondition.substr(secondOpLoc + this->secondOp.length(), std::string::npos);
    }
}

size_t WhereConditions::findOp(std::string condition, std::string& op) {
    size_t location = condition.find("!=");
    if (location == std::string::npos) {
        location = condition.find(">=");
        if (location == std::string::npos) {
            location = condition.find("<=");
            if (location == std::string::npos) {
                location = condition.find("=");
                if (location == std::string::npos) {
                    location = condition.find(">");
                    if (location == std::string::npos) {
                        location = condition.find("<");
                        if (location != std::string::npos)
                            op = "<";
                    } else {
                        op = ">";
                    }
                } else {
                    op = "=";
                }
            } else {
                op = "<=";
            }
        } else {
            op = ">=";
        }
    } else {
        op = "!=";
    }
    
    return location;
}

bool WhereConditions::getResult(User_t *user) {
    bool partialResult;
    if (this->firstField == "id" || this->firstField == "age") {
        double data = stod(this->firstData);
        partialResult = this->getPartialResult(user, this->firstField, data, this->firstOp);
    } else {
        partialResult = this->getPartialResult(user, this->firstField, this->firstData, this->firstOp);
    }
    
    if (this->secondField.size() == 0)
        return partialResult;
    
    bool secondResult;
    if (this->secondField == "id" || this->secondField == "age") {
        double data = stod(this->secondData);
        secondResult = this->getPartialResult(user, this->secondField, data, this->secondOp);
    } else {
        secondResult = this->getPartialResult(user, this->secondField, this->secondData, this->secondOp);
    }
    
    if (this->op == "or")
        return (partialResult || secondResult);
    else
        return (partialResult && secondResult);
}

bool WhereConditions::getPartialResult(User_t *user, std::string field, double data, std::string op) {
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
