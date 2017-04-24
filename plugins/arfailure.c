/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2007-2012  Intel Corporation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define CONNMAN_API_SUBJECT_TO_CHANGE
#include <connman/plugin.h>
#include <connman/log.h>
#include <connman/notifier.h>
#include <connman/service.h>

#include <glib.h>

static int timeout = -1;
static GHashTable* queued_services;

static gboolean timer_callback (gpointer user_data) {
	struct connman_service *service = user_data;

	DBG("service %p", service);

	/*
	 * Use steal so unqueue_service is not called,
	 * as unqueueing is already done when this function returns FALSE.
	 */
	g_hash_table_steal(queued_services, service);

	__connman_service_clear_error(service);

	__connman_service_auto_connect(CONNMAN_SERVICE_CONNECT_REASON_AUTO);

	return FALSE;
}

static void queue_service (struct connman_service *service) {
	guint tag;

	tag = g_timeout_add_seconds(timeout, &timer_callback, service);
	g_hash_table_insert (queued_services, service, GUINT_TO_POINTER(tag));

	DBG("service %p tag %u", service, tag);
}

static void unqueue_service(gpointer data) {
	guint tag = GPOINTER_TO_UINT(data);

	DBG("tag %u", tag);

	g_source_remove (tag);
}

static void service_state_changed(struct connman_service *service,
					enum connman_service_state state)
{
	DBG("service %p state %d", service, state);

	if (state == CONNMAN_SERVICE_STATE_FAILURE &&
			__connman_service_get_favorite(service)) {
		queue_service(service);
	}
}

static void service_remove(struct connman_service *service)
{
	DBG("service %p", service);

	g_hash_table_remove(queued_services, service);

}

static struct connman_notifier notifier = {
	.name				= "arfailure",
	.priority			= CONNMAN_NOTIFIER_PRIORITY_LOW,
	.service_state_changed	= service_state_changed,
	.service_remove 	= service_remove,
};

static int arfailure_init(void)
{
	timeout = connman_auto_reset_failure_timeout();
	DBG("timeout %d", timeout);

	if (timeout < 0) {
		return 0;
	}

	queued_services = g_hash_table_new_full(&g_direct_hash, &g_direct_equal,
													NULL, &unqueue_service);
	if (!queued_services)
		return -1;

	if (connman_notifier_register(&notifier)) {
		g_hash_table_destroy(queued_services);
		return -1;
	}

	return 0;
}

static void arfailure_exit(void)
{
	DBG("");

	if (timeout < 0)
		return;

	connman_notifier_unregister(&notifier);

	g_hash_table_remove_all(queued_services);

	g_hash_table_destroy(queued_services);
}

CONNMAN_PLUGIN_DEFINE(arfailure, "Auto failure reset",
			VERSION, CONNMAN_PLUGIN_PRIORITY_LOW,
			arfailure_init, arfailure_exit)
