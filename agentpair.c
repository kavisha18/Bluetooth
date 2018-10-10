#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <glib.h>
#include <dbus/dbus.h>

#include "lib/bluetooth.h"
#include "lib/sdp.h"

#include "gdbus/gdbus.h"

#include "log.h"
#include "error.h"
#include "hcid.h"
#include "dbus-common.h"
#include "adapter.h"
#include "device.h"
#include "agent.h"
#include "shared/queue.h"

#define REQUEST_TIMEOUT (60 * 1000)		/* 60 seconds */
#define AGENT_INTERFACE "org.bluez.Agent1"

static GHashTable *agent_list;

struct queue *default_agents = NULL;
typedef enum {
	AGENT_REQUEST_PASSKEY,
	AGENT_REQUEST_CONFIRMATION,
	AGENT_REQUEST_AUTHORIZATION,
	AGENT_REQUEST_PINCODE,
	AGENT_REQUEST_AUTHORIZE_SERVICE,
	AGENT_REQUEST_DISPLAY_PINCODE,
} agent_request_type_t;

struct agent {
	int ref;
	char *owner;
	char *path;
	uint8_t capability;
	struct agent_request *request;
	guint watch;
};

struct agent_request {
	agent_request_type_t type;
	struct agent *agent;
	DBusMessage *msg;
	DBusPendingCall *call;
	void *cb;
	void *user_data;
	GDestroyNotify destroy;
};
int main(int argc, char *argv[])
{
	GDBusClient *client;
	int status;

	bt_shell_init(argc, argv, &opt);
	bt_shell_set_menu(&main_menu);
	bt_shell_add_submenu(&advertise_menu);
	bt_shell_add_submenu(&scan_menu);
	bt_shell_add_submenu(&gatt_menu);
	bt_shell_set_prompt(PROMPT_OFF);

	if (agent_option)
		auto_register_agent = g_strdup(agent_option);
	else
		auto_register_agent = g_strdup("");

	dbus_conn = g_dbus_setup_bus(DBUS_BUS_SYSTEM, NULL, NULL);
	g_dbus_attach_object_manager(dbus_conn);

	client = g_dbus_client_new(dbus_conn, "org.bluez", "/org/bluez");

	g_dbus_client_set_connect_watch(client, connect_handler, NULL);
	g_dbus_client_set_disconnect_watch(client, disconnect_handler, NULL);
	g_dbus_client_set_signal_watch(client, message_handler, NULL);

	g_dbus_client_set_proxy_handlers(client, proxy_added, proxy_removed,
							property_changed, NULL);

	g_dbus_client_set_ready_watch(client, client_ready, NULL);

	status = bt_shell_run();

	g_dbus_client_unref(client);

	dbus_connection_unref(dbus_conn);

	g_list_free_full(ctrl_list, proxy_leak);

	g_free(auto_register_agent);

	return status;
}
