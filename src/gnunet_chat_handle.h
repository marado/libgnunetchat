/*
   This file is part of GNUnet.
   Copyright (C) 2021 GNUnet e.V.

   GNUnet is free software: you can redistribute it and/or modify it
   under the terms of the GNU Affero General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   GNUnet is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   SPDX-License-Identifier: AGPL3.0-or-later
 */
/*
 * @author Tobias Frisch
 * @file gnunet_chat_handle.h
 */

#ifndef GNUNET_CHAT_HANDLE_H_
#define GNUNET_CHAT_HANDLE_H_

#include <gnunet/platform.h>
#include <gnunet/gnunet_config.h>
#include <gnunet/gnunet_container_lib.h>
#include <gnunet/gnunet_arm_service.h>
#include <gnunet/gnunet_fs_service.h>
#include <gnunet/gnunet_gns_service.h>
#include <gnunet/gnunet_identity_service.h>
#include <gnunet/gnunet_messenger_service.h>
#include <gnunet/gnunet_reclaim_service.h>
#include <gnunet/gnunet_regex_service.h>
#include <gnunet/gnunet_util_lib.h>

struct GNUNET_CHAT_Handle
{
  const struct GNUNET_CONFIGURATION_Handle* cfg;

  struct {
    /*
     * feature: (automatically start required services)
     */
    struct GNUNET_ARM_Handle *arm;

    /*
     * required: (files can be uploaded/downloaded)
     */
    struct GNUNET_FS_Handle *fs;

    /*
     * required: (names can be resolved as well as zones and members)
     */
    struct GNUNET_GNS_Handle *gns;

    /*
     * optional: (if not anonymous to receive private key)
     * (has to be reset as soon as the private key changes)
     */
    struct GNUNET_IDENTITY_Handle *identity;

    /*
     * required!
     */
    struct GNUNET_MESSENGER_Handle *messenger;

    /*
     * feature/optional: (maybe add new reclaim message kind?)
     * (the message would automatically issue the ticket)
     * (send the ticket and consume it)
     */
    struct GNUNET_RECLAIM_Handle *reclaim;
  } handles;

  struct GNUNET_CONTAINER_MultiHashMap *contacts;
  struct GNUNET_CONTAINER_MultiHashMap *groups;
};

#endif /* GNUNET_CHAT_HANDLE_H_ */
