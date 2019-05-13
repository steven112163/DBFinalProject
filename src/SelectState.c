#include <string.h>
#include <stdlib.h>
#include "SelectState.h"
#include "WhereConditions.h"

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
