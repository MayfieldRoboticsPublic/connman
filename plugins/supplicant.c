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

#include "supplicant.h"

int __supplicant_start(struct connman_iface *iface)
{
	printf("[SUPPLICANT] start for index %d\n", iface->index);

	return 0;
}

int __supplicant_stop(struct connman_iface *iface)
{
	printf("[SUPPLICANT] stop for index %d\n", iface->index);

	return 0;
}

int __supplicant_connect(struct connman_iface *iface)
{
	printf("[SUPPLICANT] connect for index %d\n", iface->index);

	return 0;
}
