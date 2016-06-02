// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-dbus.h"
#include "utils-log.h"
#include "global-macros.h"

#include <unistd.h>
#include <string.h>
#include <dbus/dbus.h>

//------------------------------------------------------------------------------

static const char* scLogindDestination = "org.freedesktop.login1";
static const char* scLoginObject = "/org/freedesktop/login1";
static const char* scSessionObject = NULL;
static const char* scManagerInterface = "org.freedesktop.login1.Manager";
static const char* scSessionInterface = "org.freedesktop.login1.Session";

static DBusConnection* sDBusSystemConn = NULL;
static bool sHaveSessionControl = false;

#define NOIA_DBUS_ASSERT(IS_OK, ...) \
    if (not (IS_OK)) { \
        LOG_ERROR(__VA_ARGS__); \
        result = NOIA_RESULT_ERROR; \
        break; \
    }

#define NOIA_DBUS_ASSERT_SYSTEM_CONNECTION(IS_OK) \
    NOIA_DBUS_ASSERT(IS_OK, "No DBus system connection!");

#define NOIA_DBUS_ASSERT_SESSION_OBJECT(IS_OK) \
    NOIA_DBUS_ASSERT(IS_OK, "Not connected to session!");

#define NOIA_DBUS_ASSERT_MESSAGE(IS_OK) \
    NOIA_DBUS_ASSERT(IS_OK, "Failed to initialize DBus message!");

#define NOIA_DBUS_ASSERT_ARGUMENTS(IS_OK) \
    NOIA_DBUS_ASSERT(IS_OK, "Failed to store DBus message arguments!");

#define NOIA_DBUS_ASSERT_SEND(IS_OK, MESSAGE) \
    NOIA_DBUS_ASSERT(IS_OK, "DBus Send message failed! (%s)", MESSAGE);

#define NOIA_DBUS_ASSERT_REPLY(IS_OK) \
    NOIA_DBUS_ASSERT(IS_OK, "Failed to get args from DBus reply!");

#define NOIA_DBUS_PRINT_INIT(TEXT) LOG_INFO2("%s...", TEXT)

#define NOIA_DBUS_PRINT_RESULT(RESULT, TEXT) \
    if (dbus_error_is_set(&err)) { \
        LOG_WARN1("DBus: %s", err.message); \
        dbus_error_free(&err); \
    } \
    LOG_INFO2("%s: %s", TEXT, \
              (RESULT == NOIA_RESULT_SUCCESS) \
              ? "SUCCESS" : "FAILURE!");

//------------------------------------------------------------------------------

NoiaResult noia_dbus_initalize(void)
{
    static const char* const scInfoText = "Initializing DBus";

    DBusError err;
    dbus_error_init(&err);

    NoiaResult result = NOIA_RESULT_SUCCESS;

    NOIA_DBUS_PRINT_INIT(scInfoText);

    NOIA_BLOCK {
        // Connect to the system bus
        sDBusSystemConn = dbus_bus_get_private(DBUS_BUS_SYSTEM, &err);
        if (dbus_error_is_set(&err) or (sDBusSystemConn == NULL)) {
            result = NOIA_RESULT_ERROR;
            break;
        }

        // Get session ID
        result =
               noia_dbus_session_get_session_by_pid(getpid(), &scSessionObject);
        if (result == NOIA_RESULT_SUCCESS) {
            LOG_INFO2("Session DBus path is: '%s'", scSessionObject);
        } else {
            LOG_WARN2("Could not get session DBus path!");
            scSessionObject = NULL;
            break;
        }

        // Take control over session
        result = noia_dbus_session_take_control();
        if (result == NOIA_RESULT_SUCCESS) {
            sHaveSessionControl = true;
        } else {
            sHaveSessionControl = false;
            break;
        }
    }

    NOIA_DBUS_PRINT_RESULT(result, scInfoText);
    return result;
}

//------------------------------------------------------------------------------

void noia_dbus_finalize(void)
{
    LOG_INFO1("Finalizing DBus...");

    if (sHaveSessionControl == true) {
        noia_dbus_session_release_control();
        sHaveSessionControl = false;
    }

    if (sDBusSystemConn) {
        dbus_connection_close(sDBusSystemConn);
        dbus_connection_unref(sDBusSystemConn);
        sDBusSystemConn = NULL;
    }

    dbus_shutdown();

    LOG_INFO1("Finalizing DBus: DONE");
}

//------------------------------------------------------------------------------

NoiaResult noia_dbus_session_get_session_by_pid(int pid, const char** sid_out)
{
    static const char* const scMessageName = "GetSessionByPID";
    static const char* const scInfoText = "Getting session by PID";

    DBusMessage* msg = NULL;
    DBusMessage* reply = NULL;
    DBusError err;
    dbus_error_init(&err);

    bool ok = true;
    const char* sid = NULL;
    NoiaResult result = NOIA_RESULT_SUCCESS;

    NOIA_DBUS_PRINT_INIT(scInfoText);

    NOIA_BLOCK {
        NOIA_DBUS_ASSERT_SYSTEM_CONNECTION(sDBusSystemConn);

        // Create new method call
        msg = dbus_message_new_method_call(scLogindDestination,
                                           scLoginObject,
                                           scManagerInterface,
                                           scMessageName);
        NOIA_DBUS_ASSERT_MESSAGE(msg);

        // Append arguments
        ok = dbus_message_append_args(msg,
                                      DBUS_TYPE_UINT32, &pid,
                                      DBUS_TYPE_INVALID);
        NOIA_DBUS_ASSERT_ARGUMENTS(ok);

        // Send message and block
        reply = dbus_connection_send_with_reply_and_block(sDBusSystemConn,
                                                          msg, -1, &err);
        NOIA_DBUS_ASSERT_SEND(reply, scMessageName);

        // Read result
        ok = dbus_message_get_args(reply, NULL,
                                   DBUS_TYPE_OBJECT_PATH, &sid,
                                   DBUS_TYPE_INVALID);
        NOIA_DBUS_ASSERT_REPLY(ok);

        // Copy sid
        *sid_out = strdup(sid);
    }

    NOIA_DBUS_PRINT_RESULT(result, scInfoText);

    // Clear and return
    if (reply) {
        dbus_message_unref(reply);
    }
    if (msg) {
        dbus_message_unref(msg);
    }
    return result;
}

//------------------------------------------------------------------------------

NoiaResult noia_dbus_session_take_control(void)
{
    static const char* const scMessageName = "TakeControl";
    static const char* const scInfoText = "Taking control over session";

    DBusMessage* msg = NULL;
    DBusMessage* reply = NULL;
    DBusError err;
    dbus_error_init(&err);

    bool ok = false;
    uint32_t force = 0;
    int result = NOIA_RESULT_SUCCESS;

    NOIA_DBUS_PRINT_INIT(scInfoText);

    NOIA_BLOCK {
        NOIA_DBUS_ASSERT_SYSTEM_CONNECTION(sDBusSystemConn);
        NOIA_DBUS_ASSERT_SESSION_OBJECT(scSessionObject);

        // Create new method call
        msg = dbus_message_new_method_call(scLogindDestination,
                                           scSessionObject,
                                           scSessionInterface,
                                           scMessageName);
        NOIA_DBUS_ASSERT_MESSAGE(msg);

        // Append arguments
        ok = dbus_message_append_args(msg,
                                      DBUS_TYPE_BOOLEAN, &force,
                                      DBUS_TYPE_INVALID);
        NOIA_DBUS_ASSERT_ARGUMENTS(ok);

        // Send message and block
        reply = dbus_connection_send_with_reply_and_block(sDBusSystemConn,
                                                          msg, -1, &err);
        NOIA_DBUS_ASSERT_SEND(reply, scMessageName);
    }

    NOIA_DBUS_PRINT_RESULT(result, scInfoText);

    // Clear and return
    if (reply) {
        dbus_message_unref(reply);
    }
    if (msg) {
        dbus_message_unref(msg);
    }
    return result;
}

//------------------------------------------------------------------------------

NoiaResult noia_dbus_session_release_control(void)
{
    static const char* const scMessageName = "ReleaseControl";
    static const char* const scInfoText = "Releasing control over session...";

    DBusMessage* msg = NULL;
    DBusMessage* reply = NULL;
    DBusError err;
    dbus_error_init(&err);

    NoiaResult result = NOIA_RESULT_SUCCESS;

    NOIA_DBUS_PRINT_INIT(scInfoText);

    NOIA_BLOCK {
        NOIA_DBUS_ASSERT_SYSTEM_CONNECTION(sDBusSystemConn);
        NOIA_DBUS_ASSERT_SESSION_OBJECT(scSessionObject);

        // Create new method call
        msg = dbus_message_new_method_call(scLogindDestination,
                                           scSessionObject,
                                           scSessionInterface,
                                           scMessageName);
        NOIA_DBUS_ASSERT_MESSAGE(msg);

        // Send message and block
        reply = dbus_connection_send_with_reply_and_block(sDBusSystemConn,
                                                          msg, -1, &err);
        NOIA_DBUS_ASSERT_SEND(reply, scMessageName);
    }

    NOIA_DBUS_PRINT_RESULT(result, scInfoText);

    // Clear and return
    if (reply) {
        dbus_message_unref(reply);
    }
    if (msg) {
        dbus_message_unref(msg);
    }
    return result;
}

//------------------------------------------------------------------------------

int noia_dbus_session_take_device(uint32_t major, uint32_t minor)
{
    static const char* const scMessageName = "TakeDevice";
    static const char* const scInfoText = "Taking control over device";

    DBusMessage* msg = NULL;
    DBusMessage* reply = NULL;
    DBusError err;
    dbus_error_init(&err);

    int paused, fd = -1;
    bool ok = false;
    NoiaResult result = NOIA_RESULT_SUCCESS;

    NOIA_DBUS_PRINT_INIT(scInfoText);

    NOIA_BLOCK {
        NOIA_DBUS_ASSERT_SYSTEM_CONNECTION(sDBusSystemConn);
        NOIA_DBUS_ASSERT_SESSION_OBJECT(scSessionObject);

        // Create new method call
        msg = dbus_message_new_method_call(scLogindDestination,
                                           scSessionObject,
                                           scSessionInterface,
                                           scMessageName);
        NOIA_DBUS_ASSERT_MESSAGE(msg);

        // Append arguments
        ok = dbus_message_append_args(msg,
                                      DBUS_TYPE_UINT32, &major,
                                      DBUS_TYPE_UINT32, &minor,
                                      DBUS_TYPE_INVALID);
        NOIA_DBUS_ASSERT_ARGUMENTS(ok);

        // Send message and block
        reply = dbus_connection_send_with_reply_and_block(sDBusSystemConn,
                                                          msg, -1, &err);
        NOIA_DBUS_ASSERT_SEND(reply, scMessageName);

        // Read result
        ok = dbus_message_get_args(reply, &err,
                                   DBUS_TYPE_UNIX_FD, &fd,
                                   DBUS_TYPE_BOOLEAN, &paused,
                                   DBUS_TYPE_INVALID);
        NOIA_DBUS_ASSERT_REPLY(ok);
    }

    NOIA_DBUS_PRINT_RESULT(result, scInfoText);

    // Clear and return
    if (reply) {
        dbus_message_unref(reply);
    }
    if (msg) {
        dbus_message_unref(msg);
    }
    return fd;
}

//------------------------------------------------------------------------------

