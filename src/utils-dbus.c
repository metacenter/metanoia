// file: utils-dbus.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-dbus.h"
#include "utils-log.h"

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <dbus/dbus.h>

//------------------------------------------------------------------------------

//static const char* scDBusName = "org.aura-wm";
static const char* scLogindDestination = "org.freedesktop.login1";
static const char* scLoginObject = "/org/freedesktop/login1";
static const char* scSessionObject = NULL;
static const char* scManagerInterface = "org.freedesktop.login1.Manager";
static const char* scSessionInterface = "org.freedesktop.login1.Session";

static const char* scDBusSystemConnErrorMsg = "No DBus system conection!";
static const char* scSessionObjectErrorMsg = "Not connected to session!";

static DBusConnection* sDBusSystemConn = NULL;
static bool sHaveSessionControl = FALSE;

#define ASSERT(_ok_, _message_) \
    if (!(_ok_)) { \
        LOG_ERROR(_message_); \
        return -ENODEV; \
    }

#define ASSERT_MESSAGE(_ok_) \
    if (!(_ok_)) { \
        LOG_ERROR("Failed to initialize DBus message!"); \
        return -ENOMEM; \
    }

#define ASSERT_ARGUMENTS(_ok_) \
    if (!(_ok_)) { \
        LOG_ERROR("Failed to store DBus message arguments!"); \
        result = -ENOMEM; \
        goto clear_msg; \
    }

#define ASSERT_SEND(_ok_, _message_) \
    if (dbus_error_is_set(&err)) { \
        LOG_WARN1("DBus: %s", err.message); \
        dbus_error_free(&err); \
    } \
    if (!(_ok_)) { \
        LOG_ERROR("DBus Send message failed! (%s)", _message_); \
        result = -EIO; \
        goto clear_msg; \
    }

#define ASSERT_REPLY(_ok_) \
    if (dbus_error_is_set(&err)) { \
        LOG_WARN1("DBus: %s", err.message); \
        dbus_error_free(&err); \
    } \
    if (!(_ok_)) { \
        LOG_ERROR("Failed to get args from DBus reply!"); \
        result = -ENODEV; \
        goto clear_reply; \
    }

//------------------------------------------------------------------------------

bool aura_dbus_initalize(void)
{
    //int ret; // TBR
    DBusError err;

    LOG_INFO1("Initializing DBus...");

    // Initialise the errors
    dbus_error_init(&err);

    // Connect to the system bus
    sDBusSystemConn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) {
        LOG_ERROR(err.message);
        dbus_error_free(&err);
    }
    if (NULL == sDBusSystemConn) {
        LOG_ERROR("DBus initialization failed!");
        return FALSE;
    }

    // Request our name on the bus // TBR
    /*ret = dbus_bus_request_name(sDBusSystemConn, scDBusName,
                                DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
    if (dbus_error_is_set(&err)) {
        LOG_WARN2(err.message);
        dbus_error_free(&err);
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        LOG_WARN2("DBus name request failed!");
    }*/

    // Get session ID
    if (aura_dbus_session_get_session_by_pid(getpid(), &scSessionObject)) {
        LOG_DATA2("Session DBus path is: '%s'", scSessionObject);
    } else {
        LOG_WARN2("Could not get session DBus path!");
        scSessionObject = NULL;
    }

    // Take control over session
    if (aura_dbus_session_take_control() < 0) {
        LOG_WARN2("Could not take controll over session!");
        sHaveSessionControl = FALSE;
    } else {
        LOG_INFO2("Taking control over session: SUCCESS");
        sHaveSessionControl = TRUE;
    }

    LOG_INFO1("Initializing DBus: SUCCESS");
    return TRUE;
}

//------------------------------------------------------------------------------

void aura_dbus_finalize(void)
{
    LOG_INFO1("Finallizing DBus...");
    if (sHaveSessionControl == TRUE) {
        aura_dbus_session_release_control();
        sHaveSessionControl = FALSE;
    }

    if (sDBusSystemConn) {
        //dbus_connection_close(sDBusSystemConn); // TBR
        dbus_connection_unref(sDBusSystemConn);
    }
    LOG_INFO1("Finallizing DBus: SUCCESS");
}

//------------------------------------------------------------------------------

int aura_dbus_session_get_session_by_pid(int pid, const char** sid_out)
{
    static const char const* cMessageName = "GetSessionByPID";

    bool ok;
    int result;
    const char* sid;
    DBusMessage *msg;
    DBusMessage *reply;
    DBusError err;

    ASSERT(sDBusSystemConn, scDBusSystemConnErrorMsg);
    dbus_error_init(&err);

    // Create new method call
    msg = dbus_message_new_method_call(scLogindDestination,
                                       scLoginObject,
                                       scManagerInterface,
                                       cMessageName);
    ASSERT_MESSAGE(msg);

    // Append arguments
    ok = dbus_message_append_args(msg,
                                  DBUS_TYPE_UINT32, &pid,
                                  DBUS_TYPE_INVALID);
    ASSERT_ARGUMENTS(ok);

    // Send message and block
    reply = dbus_connection_send_with_reply_and_block(sDBusSystemConn,
                                                      msg, -1, &err);
    ASSERT_SEND(reply, cMessageName);

    // Read result
    ok = dbus_message_get_args(reply, NULL,
                               DBUS_TYPE_OBJECT_PATH, &sid,
                               DBUS_TYPE_INVALID);
    ASSERT_REPLY(ok);

    // Copy sid
    *sid_out = strdup(sid);

    // Clear and return
    result = TRUE;
    clear_reply: dbus_message_unref(reply);
    clear_msg: dbus_message_unref(msg);
    return result;
}

//------------------------------------------------------------------------------

int aura_dbus_session_take_control()
{
    static const char const* cMessageName = "TakeControl";

    bool ok;
    uint32_t force = 0;
    int result;
    DBusMessage *msg;
    DBusMessage *reply;
    DBusError err;

    LOG_INFO2("Taking control over session...");
    ASSERT(sDBusSystemConn, scDBusSystemConnErrorMsg);
    ASSERT(scSessionObject, scSessionObjectErrorMsg);
    dbus_error_init(&err);

    // Create new method call
    msg = dbus_message_new_method_call(scLogindDestination,
                                       scSessionObject,
                                       scSessionInterface,
                                       cMessageName);
    ASSERT_MESSAGE(msg);

    // Append arguments
    ok = dbus_message_append_args(msg,
                                  DBUS_TYPE_BOOLEAN, &force,
                                  DBUS_TYPE_INVALID);
    ASSERT_ARGUMENTS(ok);

    // Send message and block
    reply = dbus_connection_send_with_reply_and_block(sDBusSystemConn,
                                                      msg, -1, &err);
    ASSERT_SEND(reply, cMessageName);

    // Clear and return
    result = TRUE;
    clear_msg: dbus_message_unref(msg);
    return result;
}

//------------------------------------------------------------------------------

int aura_dbus_session_release_control()
{
    static const char const* cMessageName = "ReleaseControl";

    int result;
    DBusMessage *msg;
    DBusMessage *reply;
    DBusError err;

    LOG_INFO2("Releasing control over session...");
    ASSERT(sDBusSystemConn, scDBusSystemConnErrorMsg);
    ASSERT(scSessionObject, scSessionObjectErrorMsg);
    dbus_error_init(&err);

    // Create new method call
    msg = dbus_message_new_method_call(scLogindDestination,
                                       scSessionObject,
                                       scSessionInterface,
                                       cMessageName);
    ASSERT_MESSAGE(msg);

    // Send message and block
    reply = dbus_connection_send_with_reply_and_block(sDBusSystemConn,
                                                      msg, -1, &err);
    ASSERT_SEND(reply, cMessageName);

    // Clear and return
    LOG_INFO2("Releasing control over session: SUCCESS");
    result = TRUE;
    clear_msg: dbus_message_unref(msg);
    return result;
}

//------------------------------------------------------------------------------

int aura_dbus_session_take_device(uint32_t major, uint32_t minor)
{
    static const char const* cMessageName = "TakeDevice";

    bool ok;
    int result, fd, paused;
    DBusMessage *msg;
    DBusMessage *reply;
    DBusError err;

    ASSERT(sDBusSystemConn, scDBusSystemConnErrorMsg);
    ASSERT(scSessionObject, scSessionObjectErrorMsg);
    dbus_error_init(&err);

    // Create new method call
    msg = dbus_message_new_method_call(scLogindDestination,
                                       scSessionObject,
                                       scSessionInterface,
                                       cMessageName);
    ASSERT_MESSAGE(msg);

    // Append arguments
    ok = dbus_message_append_args(msg,
                                  DBUS_TYPE_UINT32, &major,
                                  DBUS_TYPE_UINT32, &minor,
                                  DBUS_TYPE_INVALID);
    ASSERT_ARGUMENTS(ok);

    // Send message and block
    reply = dbus_connection_send_with_reply_and_block(sDBusSystemConn,
                                                      msg, -1, &err);
    ASSERT_SEND(reply, cMessageName);

    // Read result
    ok = dbus_message_get_args(reply, &err,
                               DBUS_TYPE_UNIX_FD, &fd,
                               DBUS_TYPE_BOOLEAN, &paused,
                               DBUS_TYPE_INVALID);
    ASSERT_REPLY(ok);

    // Clear and return
    result = fd;
    clear_reply: dbus_message_unref(reply);
    clear_msg: dbus_message_unref(msg);
    return result;
}

//------------------------------------------------------------------------------

