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

#endif
