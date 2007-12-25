/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2007  Intel Corporation. All rights reserved.
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

#include <stdio.h>
#include <arpa/inet.h>

#include <connman/plugin.h>
#include <connman/iface.h>

#include "net.h"
#include "supplicant.h"

static int iface_probe(struct connman_iface *iface)
{
	printf("[802.11] probe interface index %d\n", iface->index);

	iface->type = CONNMAN_IFACE_TYPE_80211;

	iface->flags = CONNMAN_IFACE_FLAG_RTNL |
				CONNMAN_IFACE_FLAG_IPV4;

	return 0;
}

static void iface_remove(struct connman_iface *iface)
{
	printf("[802.11] remove interface index %d\n", iface->index);

	__net_clear(iface->index);

	__supplicant_stop(iface);
}

static int iface_activate(struct connman_iface *iface)
{
	printf("[802.11] activate interface index %d\n", iface->index);

	__supplicant_start(iface);

	connman_iface_update(iface, CONNMAN_IFACE_STATE_ACTIVE);

	return 0;
}

static int iface_get_ipv4(struct connman_iface *iface,
					struct connman_ipv4 *ipv4)
{
	if (__net_ifaddr(iface->index, &ipv4->address) < 0)
		return -1;

	printf("[802.11] get address %s\n", inet_ntoa(ipv4->address));

	return 0;
}

static int iface_set_ipv4(struct connman_iface *iface,
					struct connman_ipv4 *ipv4)
{
	printf("[802.11] set address %s\n", inet_ntoa(ipv4->address));
	printf("[802.11] set netmask %s\n", inet_ntoa(ipv4->netmask));
	printf("[802.11] set gateway %s\n", inet_ntoa(ipv4->gateway));

	__net_set(iface->index, &ipv4->address, &ipv4->netmask,
				&ipv4->gateway, &ipv4->broadcast,
						&ipv4->nameserver);

	return 0;
}

static int iface_scan(struct connman_iface *iface)
{
	printf("[802.11] scanning interface index %d\n", iface->index);

	return 0;
}

static int iface_connect(struct connman_iface *iface,
					struct connman_network *network)
{
	printf("[802.11] connect interface index %d\n", iface->index);

	__supplicant_connect(iface);

	return 0;
}

static struct connman_iface_driver iface_driver = {
	.name		= "80211",
	.capability	= "net.80211",
	.probe		= iface_probe,
	.remove		= iface_remove,
	.activate	= iface_activate,
	.get_ipv4	= iface_get_ipv4,
	.set_ipv4	= iface_set_ipv4,
	.scan		= iface_scan,
	.connect	= iface_connect,
};

static int plugin_init(void)
{
	connman_iface_register(&iface_driver);

	return 0;
}

static void plugin_exit(void)
{
	connman_iface_unregister(&iface_driver);
}

CONNMAN_PLUGIN_DEFINE("80211", "IEEE 802.11 interface plugin", VERSION,
						plugin_init, plugin_exit)
