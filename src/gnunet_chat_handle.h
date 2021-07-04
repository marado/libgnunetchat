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
#include <gnunet/gnunet_messenger_service.h>
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
     * required!
     */
    struct GNUNET_MESSENGER_Handle *messenger;
  } handles;

  struct {
    struct GNUNET_CONTAINER_MultiShortmap *short_map;
    struct GNUNET_CONTAINER_MultiHashMap *hash_map;
  } contacts;

  struct GNUNET_CONTAINER_MultiHashMap *groups;
  struct GNUNET_CONTAINER_MultiHashMap *contexts;
  struct GNUNET_CONTAINER_MultiHashMap *files;

  GNUNET_CHAT_ContextMessageCallback msg_cb;
  void *msg_cls;

  GNUNET_CHAT_WarningCallback warn_cb;
  void *warn_cls;
};

int
handle_update_chat_contact (struct GNUNET_CHAT_Handle *handle,
			    struct GNUNET_CHAT_Contact *chatContact,
			    int removeContact);

int
handle_update_chat_group (struct GNUNET_CHAT_Handle *handle,
			  struct GNUNET_CHAT_Group *chatGroup,
			  int removeGroup);

int
handle_update_chat_context (struct GNUNET_CHAT_Handle *handle,
			    struct GNUNET_CHAT_Context *context,
			    int removeContext);

int
handle_update_chat_file (struct GNUNET_CHAT_Handle *handle,
			 struct GNUNET_CHAT_File *file,
			 int removeFile);

struct GNUNET_CHAT_Contact*
handle_get_chat_contact (struct GNUNET_CHAT_Handle *handle,
			 const struct GNUNET_MESSENGER_Contact *contact);

void
handle_set_chat_contact (struct GNUNET_CHAT_Handle *handle,
			 const struct GNUNET_MESSENGER_Contact *contact,
			 struct GNUNET_CHAT_Contact *chatContact);

struct GNUNET_CHAT_Context*
handle_get_chat_context (struct GNUNET_CHAT_Handle *handle,
			 const struct GNUNET_HashCode *key);

struct GNUNET_CHAT_File*
handle_get_chat_file (struct GNUNET_CHAT_Handle *handle,
		      const struct GNUNET_HashCode *hash);

#endif /* GNUNET_CHAT_HANDLE_H_ */
