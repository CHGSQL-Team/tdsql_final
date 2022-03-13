#pragma once

enum Log_event_type {
    /**
      Every time you add a type, you have to
      - Assign it a number explicitly. Otherwise it will cause trouble
        if a event type before is deprecated and removed directly from
        the enum.
      - Fix Format_description_event::Format_description_event().
    */
    UNKNOWN_EVENT = 0,
    /*
      Deprecated since mysql 8.0.2. It is just a placeholder,
      should not be used anywhere else.
    */
    START_EVENT_V3 = 1,
    QUERY_EVENT = 2,
    STOP_EVENT = 3,
    ROTATE_EVENT = 4,
    INTVAR_EVENT = 5,

    SLAVE_EVENT = 7,

    APPEND_BLOCK_EVENT = 9,
    DELETE_FILE_EVENT = 11,

    RAND_EVENT = 13,
    USER_VAR_EVENT = 14,
    FORMAT_DESCRIPTION_EVENT = 15,
    XID_EVENT = 16,
    BEGIN_LOAD_QUERY_EVENT = 17,
    EXECUTE_LOAD_QUERY_EVENT = 18,

    TABLE_MAP_EVENT = 19,

    /**
      The V1 event numbers are used from 5.1.16 until mysql-5.6.
    */
    WRITE_ROWS_EVENT_V1 = 23,
    UPDATE_ROWS_EVENT_V1 = 24,
    DELETE_ROWS_EVENT_V1 = 25,

    /**
      Something out of the ordinary happened on the master
     */
    INCIDENT_EVENT = 26,

    /**
      Heartbeat event to be send by master at its idle time
      to ensure master's online status to slave
    */
    HEARTBEAT_LOG_EVENT = 27,

    /**
      In some situations, it is necessary to send over ignorable
      data to the slave: data that a slave can handle in case there
      is code for handling it, but which can be ignored if it is not
      recognized.
    */
    IGNORABLE_LOG_EVENT = 28,
    ROWS_QUERY_LOG_EVENT = 29,

    /** Version 2 of the Row events */
    WRITE_ROWS_EVENT = 30,
    UPDATE_ROWS_EVENT = 31,
    DELETE_ROWS_EVENT = 32,

    GTID_LOG_EVENT = 33,
    ANONYMOUS_GTID_LOG_EVENT = 34,

    PREVIOUS_GTIDS_LOG_EVENT = 35,

    TRANSACTION_CONTEXT_EVENT = 36,

    VIEW_CHANGE_EVENT = 37,

    /* Prepared XA transaction terminal event similar to Xid */
    XA_PREPARE_LOG_EVENT = 38,

    /**
      Extension of UPDATE_ROWS_EVENT, allowing partial values according
      to binlog_row_value_options.
    */
    PARTIAL_UPDATE_ROWS_EVENT = 39,

    TRANSACTION_PAYLOAD_EVENT = 40,

    HEARTBEAT_LOG_EVENT_V2 = 41,
    /**
      Add new events here - right above this comment!
      Existing events (except ENUM_END_EVENT) should never change their numbers
    */
    ENUM_END_EVENT /* end marker */
};

#define EVENT_TYPE_OFFSET 4
#define SERVER_ID_OFFSET 5
#define EVENT_LEN_OFFSET 9
#define LOG_POS_OFFSET 13
#define FLAGS_OFFSET 17

const char *get_type_str(Log_event_type type);