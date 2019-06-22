#include <string.h>
#include <stdlib.h>
#include "SelectState.h"

void field_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table) {
    cmd->cmd_args.sel_args.fields = nullptr;
    cmd->cmd_args.sel_args.fields_len = 0;
    cmd->cmd_args.sel_args.limit = -1;
    cmd->cmd_args.sel_args.offset = -1;
    cmd->cmd_args.sel_args.idxList.clear();
    cmd->cmd_args.sel_args.idxListLen = 0;
    table->aggreTypes.clear();
    table->aggreFields.clear();
    table->aggreResults.clear();
    table->joinTuples.clear();
    while (arg_idx < cmd->args_len) {
        std::string field;
        std::string type = check_aggregation(cmd->args[arg_idx], field);
        if (!type.empty()) {
            table->aggreTypes.push_back(type);
            table->aggreFields.push_back(field);
        } else if (cmd->args[arg_idx] == "*") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "id") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "name") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "email") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "age") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "id1") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "id2") {
            add_select_field(cmd, cmd->args[arg_idx].c_str());
        } else if (cmd->args[arg_idx] == "from") {
            table_state_handler(cmd, arg_idx + 1, table);
            return;
        } else {
            cmd->type = UNRECOG_CMD;
            return;
        }
        arg_idx += 1;
    }
    cmd->type = UNRECOG_CMD;
}

std::string check_aggregation(const std::string &aggregation, std::string &field) {
    size_t aggreLoc = aggregation.find("avg");
    if (aggreLoc != std::string::npos) {
        field = aggregation.substr(aggreLoc + 4, aggregation.length() - 5);
        return "avg";
    }

    aggreLoc = aggregation.find("count");
    if (aggreLoc != std::string::npos) {
        field = aggregation.substr(aggreLoc + 6, aggregation.length() - 7);
        return "count";
    }

    aggreLoc = aggregation.find("sum");
    if (aggreLoc != std::string::npos) {
        field = aggregation.substr(aggreLoc + 4, aggregation.length() - 5);
        return "sum";
    }

    field = "";
    return "";
}

void table_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table) {
    // Join detection
    auto join_idx = 0;
    for (int i = 0; i < cmd->args_len; i++) {
        if (cmd->args[i] == "join") {
            join_idx = i;
            break;
        }
    }

    // Join detected! Make join tuples...
    if (join_idx > 0) {
        table->joinTuples.clear();  // reset tuple vector

        auto on_idx = 0;
        for (int i = 0; i < cmd->args_len; i++) {
            if (cmd->args[i] == "on") {
                join_idx = i;
                break;
            }
        }

        // There will only be: id = id1 or id = id2
        // joined_field is the field flag of Table like.
        auto joined_field = 0;
        if (cmd->args[on_idx + 3] == "id1")
            joined_field = 1;
        else
            joined_field = 2;


    }


    // TODO: Fix aggr, where, offset, limit after join
    if (arg_idx < cmd->args_len && (cmd->args[arg_idx] == "user")) {
        arg_idx++;
        if (arg_idx == cmd->args_len) {
            if (!table->aggreTypes.empty()) {
                std::vector<size_t> targetIdx;
                size_t idx, len = table->users.size();
                for (idx = 0; idx < len; idx++)
                    targetIdx.push_back(idx);
                get_aggregation_result(targetIdx, table);
            }
            return;
        } else if (cmd->args[arg_idx] == "where") {
            where_state_handler(cmd, arg_idx + 1, table);
            return;
        } else if (cmd->args[arg_idx] == "offset") {
            offset_state_handler(cmd, arg_idx + 1, nullptr, table);
            return;
        } else if (cmd->args[arg_idx] == "limit") {
            limit_state_handler(cmd, arg_idx + 1, nullptr, table);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
}

void where_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table) {
    if (arg_idx < cmd->args_len) {
        size_t next_arg;
        for (next_arg = arg_idx; next_arg < cmd->args_len; next_arg++)
            if (cmd->args[next_arg] == "offset" ||
                cmd->args[next_arg] == "limit")
                break;

        std::vector<std::string> conditions;
        for (; arg_idx < next_arg; arg_idx++)
            conditions.push_back(cmd->args[arg_idx]);
        WhereConditions whereConditions(conditions);

        if (!table->aggreTypes.empty()) {
            std::vector<size_t> targetIdx;
            size_t idx, len = table->users.size();
            for (idx = 0; idx < len; idx++) {
                User_t *user = get_User(table, idx);
                if (whereConditions.getResult(user))
                    targetIdx.push_back(idx);
            }

            get_aggregation_result(targetIdx, table);
        } else {
            size_t idx, len = table->users.size();
            for (idx = 0; idx < len; idx++) {
                User_t *user = get_User(table, idx);
                if (whereConditions.getResult(user)) {
                    cmd->cmd_args.sel_args.idxList.push_back(idx);
                    cmd->cmd_args.sel_args.idxListLen++;
                }
            }
            if (cmd->cmd_args.sel_args.idxListLen == 0)
                cmd->cmd_args.sel_args.idxListLen = 1;
        }

        if (next_arg == cmd->args_len) {
            return;
        }
        if (next_arg < cmd->args_len && cmd->args[next_arg] == "offset") {
            offset_state_handler(cmd, next_arg + 1, &whereConditions, table);
            return;
        } else if (next_arg < cmd->args_len && cmd->args[next_arg] == "limit") {
            limit_state_handler(cmd, next_arg + 1, &whereConditions, table);
            return;
        }

    }
    cmd->type = UNRECOG_CMD;
}

void get_aggregation_result(std::vector<size_t> targetIdx, Table_t *table) {
    size_t aggreIdx;
    for (aggreIdx = 0; aggreIdx < table->aggreTypes.size(); aggreIdx++) {
        if (table->aggreTypes[aggreIdx] == "count") {
            table->aggreResults.push_back(std::to_string(targetIdx.size()));
        } else {
            unsigned int sum = 0;
            size_t idx;
            for (idx = 0; idx < targetIdx.size(); idx++) {
                User_t *user = get_User(table, targetIdx[idx]);
                if (table->aggreFields[aggreIdx] == "id")
                    sum += user->id;
                else
                    sum += user->age;
            }
            if (table->aggreTypes[aggreIdx] == "sum")
                table->aggreResults.push_back(std::to_string(sum));
            else
                table->aggreResults.push_back(std::to_string((double) sum / targetIdx.size()));
        }
    }
}

void offset_state_handler(Command_t *cmd, size_t arg_idx, WhereConditions *whereConditions, Table_t *table) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.offset = atoi(cmd->args[arg_idx].c_str());

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            if (whereConditions == nullptr) {
                if (!table->aggreTypes.empty()) {
                    std::vector<size_t> targetIdx;
                    size_t idx, len = table->users.size();
                    for (idx = 0; idx < len; idx++)
                        targetIdx.push_back(idx);
                    get_aggregation_result(targetIdx, table);
                }
            }
            return;
        } else if (arg_idx < cmd->args_len && cmd->args[arg_idx] == "limit") {

            limit_state_handler(cmd, arg_idx + 1, whereConditions, table);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
}

void limit_state_handler(Command_t *cmd, size_t arg_idx, WhereConditions *whereConditions, Table_t *table) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.limit = atoi(cmd->args[arg_idx].c_str());

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            if (whereConditions == nullptr) {
                if (!table->aggreTypes.empty()) {
                    std::vector<size_t> targetIdx;
                    size_t idx, len = table->users.size();
                    for (idx = 0; idx < len; idx++)
                        targetIdx.push_back(idx);
                    get_aggregation_result(targetIdx, table);
                }
            }
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
}
