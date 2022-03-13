#include "underlying/event_internal.h"

const char *get_type_str(Log_event_type type) {
    switch (type) {
        case Log_event_type::STOP_EVENT:
            return "Stop";
        case Log_event_type::QUERY_EVENT:
            return "Query";
        case Log_event_type::ROTATE_EVENT:
            return "Rotate";
        case Log_event_type::INTVAR_EVENT:
            return "Intvar";
        case Log_event_type::APPEND_BLOCK_EVENT:
            return "Append_block";
        case Log_event_type::DELETE_FILE_EVENT:
            return "Delete_file";
        case Log_event_type::RAND_EVENT:
            return "RAND";
        case Log_event_type::XID_EVENT:
            return "Xid";
        case Log_event_type::USER_VAR_EVENT:
            return "User var";
        case Log_event_type::FORMAT_DESCRIPTION_EVENT:
            return "Format_desc";
        case Log_event_type::TABLE_MAP_EVENT:
            return "Table_map";
        case Log_event_type::WRITE_ROWS_EVENT_V1:
            return "Write_rows_v1";
        case Log_event_type::UPDATE_ROWS_EVENT_V1:
            return "Update_rows_v1";
        case Log_event_type::DELETE_ROWS_EVENT_V1:
            return "Delete_rows_v1";
        case Log_event_type::BEGIN_LOAD_QUERY_EVENT:
            return "Begin_load_query";
        case Log_event_type::EXECUTE_LOAD_QUERY_EVENT:
            return "Execute_load_query";
        case Log_event_type::INCIDENT_EVENT:
            return "Incident";
        case Log_event_type::IGNORABLE_LOG_EVENT:
            return "Ignorable";
        case Log_event_type::ROWS_QUERY_LOG_EVENT:
            return "Rows_query";
        case Log_event_type::WRITE_ROWS_EVENT:
            return "Write_rows";
        case Log_event_type::UPDATE_ROWS_EVENT:
            return "Update_rows";
        case Log_event_type::DELETE_ROWS_EVENT:
            return "Delete_rows";
        case Log_event_type::GTID_LOG_EVENT:
            return "Gtid";
        case Log_event_type::ANONYMOUS_GTID_LOG_EVENT:
            return "Anonymous_Gtid";
        case Log_event_type::PREVIOUS_GTIDS_LOG_EVENT:
            return "Previous_gtids";
        case Log_event_type::HEARTBEAT_LOG_EVENT:
        case Log_event_type::HEARTBEAT_LOG_EVENT_V2:
            return "Heartbeat";
        case Log_event_type::TRANSACTION_CONTEXT_EVENT:
            return "Transaction_context";
        case Log_event_type::VIEW_CHANGE_EVENT:
            return "View_change";
        case Log_event_type::XA_PREPARE_LOG_EVENT:
            return "XA_prepare";
        case Log_event_type::PARTIAL_UPDATE_ROWS_EVENT:
            return "Update_rows_partial";
        case Log_event_type::TRANSACTION_PAYLOAD_EVENT:
            return "Transaction_payload";
        default:
            return "Unknown"; /* impossible */
    }
}