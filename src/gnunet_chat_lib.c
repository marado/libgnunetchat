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
 * @file gnunet_chat_lib.c
 */

#include "gnunet_chat_lib.h"

#include <limits.h>

#include "gnunet_chat_config.h"
#include "gnunet_chat_contact.h"
#include "gnunet_chat_context.h"
#include "gnunet_chat_file.h"
#include "gnunet_chat_group.h"
#include "gnunet_chat_handle.h"
#include "gnunet_chat_invitation.h"
#include "gnunet_chat_message.h"
#include "gnunet_chat_util.h"

#include "gnunet_chat_lib_intern.c"

struct GNUNET_CHAT_Handle*
GNUNET_CHAT_start (const struct GNUNET_CONFIGURATION_Handle *cfg,
		   const char *name,
		   GNUNET_CHAT_WarningCallback warn_cb, void *warn_cls,
		   GNUNET_CHAT_ContextMessageCallback msg_cb, void *msg_cls)
{
  if (!cfg)
    return NULL;

  if (!name)
    return NULL;

  return handle_create_from_config(
      cfg, name, msg_cb, msg_cls, warn_cb, warn_cls
  );
}

void
GNUNET_CHAT_stop (struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return;

  handle_destroy(handle);
}

int
GNUNET_CHAT_update (struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return GNUNET_SYSERR;

  return GNUNET_MESSENGER_update(handle->messenger);
}

int
GNUNET_CHAT_set_name (struct GNUNET_CHAT_Handle *handle,
		      const char *name)
{
  if (!handle)
    return GNUNET_SYSERR;

  if (!name)
    return GNUNET_NO;

  return GNUNET_MESSENGER_set_name(handle->messenger, name);
}

const char*
GNUNET_CHAT_get_name (const struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return NULL;

  return GNUNET_MESSENGER_get_name(handle->messenger);
}

const struct GNUNET_IDENTITY_PublicKey*
GNUNET_CHAT_get_key (const struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return NULL;

  return GNUNET_MESSENGER_get_key(handle->messenger);
}

int
GNUNET_CHAT_iterate_contacts (struct GNUNET_CHAT_Handle *handle,
			      GNUNET_CHAT_ContactCallback callback,
			      void *cls)
{
  if (!handle)
    return GNUNET_SYSERR;

  struct GNUNET_CHAT_HandleIterateContacts it;
  it.handle = handle;
  it.cb = callback;
  it.cls = cls;

  return GNUNET_CONTAINER_multishortmap_iterate(
      handle->contacts, it_handle_iterate_contacts, &it
  );
}

struct GNUNET_CHAT_Group *
GNUNET_CHAT_group_create (struct GNUNET_CHAT_Handle *handle,
			  const char* topic)
{
  if (!handle)
    return NULL;

  struct GNUNET_HashCode key;

  if (topic)
    GNUNET_CRYPTO_hash(topic, strlen(topic), &key);
  else
    GNUNET_CRYPTO_random_block(GNUNET_CRYPTO_QUALITY_WEAK, &key, sizeof(key));

  if (GNUNET_YES == GNUNET_CONTAINER_multihashmap_contains(
      handle->contexts, &key))
    return NULL;

  struct GNUNET_MESSENGER_Room *room = GNUNET_MESSENGER_open_room(
      handle->messenger, &key
  );

  struct GNUNET_CHAT_Context *context = context_create_from_room(handle, room);

  context->type = GNUNET_CHAT_CONTEXT_TYPE_GROUP;

  if (GNUNET_OK != GNUNET_CONTAINER_multihashmap_put(
      handle->contexts, &key, context,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_FAST))
    goto destroy_context;

  struct GNUNET_CHAT_Group *group = group_create_from_context(handle, context);

  util_set_name_field(topic, &(group->topic));

  if (group->topic)
    group_publish(group);

  if (GNUNET_OK == GNUNET_CONTAINER_multihashmap_put(
      handle->groups, &key, group,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_FAST))
    return group;

  group_destroy(group);

  GNUNET_CONTAINER_multihashmap_remove(handle->contexts, &key, context);

destroy_context:
  context_destroy(context);
  return NULL;
}

int
GNUNET_CHAT_iterate_groups (struct GNUNET_CHAT_Handle *handle,
			    GNUNET_CHAT_GroupCallback callback,
			    void *cls)
{
  if (!handle)
    return GNUNET_SYSERR;

  struct GNUNET_CHAT_HandleIterateGroups it;
  it.handle = handle;
  it.cb = callback;
  it.cls = cls;

  return GNUNET_CONTAINER_multihashmap_iterate(
      handle->groups, it_handle_iterate_groups, &it
  );
}

int
GNUNET_CHAT_contact_delete (struct GNUNET_CHAT_Contact *contact)
{
  if (!contact)
    return GNUNET_SYSERR;

  struct GNUNET_ShortHashCode shorthash;
  util_shorthash_from_member(contact->member, &shorthash);

  GNUNET_CONTAINER_multishortmap_remove(
      contact->handle->contacts, &shorthash, contact
  );

  const struct GNUNET_HashCode *key = GNUNET_MESSENGER_room_get_key(
      contact->context->room
  );

  GNUNET_CONTAINER_multihashmap_remove(
      contact->handle->contexts, key, contact->context
  );

  GNUNET_MESSENGER_close_room(contact->context->room);

  context_destroy(contact->context);
  contact_destroy(contact);
  return GNUNET_OK;
}

void
GNUNET_CHAT_contact_set_name (struct GNUNET_CHAT_Contact *contact,
			      const char *name)
{
  if (!contact)
    return;

  util_set_name_field(name, &(contact->context->nick));
}


const char*
GNUNET_CHAT_contact_get_name (const struct GNUNET_CHAT_Contact *contact)
{
  if (!contact)
    return NULL;

  if (contact->context->nick)
    return contact->context->nick;

  return GNUNET_MESSENGER_contact_get_name(contact->member);
}


const struct GNUNET_IDENTITY_PublicKey*
GNUNET_CHAT_contact_get_key (const struct GNUNET_CHAT_Contact *contact)
{
  if (!contact)
    return NULL;

  return GNUNET_MESSENGER_contact_get_key(contact->member);
}


struct GNUNET_CHAT_Context*
GNUNET_CHAT_contact_get_context (struct GNUNET_CHAT_Contact *contact)
{
  if (!contact)
    return NULL;

  return contact->context;
}


int
GNUNET_CHAT_group_leave (struct GNUNET_CHAT_Group *group)
{
  if (!group)
    return GNUNET_SYSERR;

  const struct GNUNET_HashCode *key = GNUNET_MESSENGER_room_get_key(
      group->context->room
  );

  GNUNET_CONTAINER_multihashmap_remove(
      group->handle->groups, key, group
  );

  GNUNET_CONTAINER_multihashmap_remove(
      group->handle->contexts, key, group->context
  );

  GNUNET_MESSENGER_close_room(group->context->room);

  context_destroy(group->context);
  group_destroy(group);
  return GNUNET_OK;
}


void
GNUNET_CHAT_group_set_name (struct GNUNET_CHAT_Group *group,
			    const char *name)
{
  if (!group)
    return;

  util_set_name_field(name, &(group->context->nick));
}


const char*
GNUNET_CHAT_group_get_name (const struct GNUNET_CHAT_Group *group)
{
  if (!group)
    return NULL;

  return group->context->nick;
}


void
GNUNET_CHAT_group_invite_contact (struct GNUNET_CHAT_Group *group,
				  struct GNUNET_CHAT_Contact *contact)
{
  if ((!group) || (!contact))
    return;

  const struct GNUNET_HashCode *key = GNUNET_MESSENGER_room_get_key(
      group->context->room
  );

  GNUNET_MESSENGER_open_room(group->handle->messenger, key);

  struct GNUNET_MESSENGER_Message msg;
  msg.header.kind = GNUNET_MESSENGER_KIND_INVITE;
  GNUNET_CRYPTO_get_peer_identity(group->handle->cfg, &(msg.body.invite.door));
  GNUNET_memcpy(&(msg.body.invite.key), key, sizeof(msg.body.invite.key));

  GNUNET_MESSENGER_send_message(contact->context->room, &msg, contact->member);
}

int
GNUNET_CHAT_group_iterate_contacts (struct GNUNET_CHAT_Group *group,
				    GNUNET_CHAT_GroupContactCallback callback,
				    void *cls)
{
  if (!group)
    return GNUNET_SYSERR;

  struct GNUNET_CHAT_GroupIterateContacts it;
  it.group = group;
  it.cb = callback;
  it.cls = cls;

  return GNUNET_MESSENGER_iterate_members(
      group->context->room, it_group_iterate_contacts, &it
  );
}


struct GNUNET_CHAT_Context*
GNUNET_CHAT_group_get_context (struct GNUNET_CHAT_Group *group)
{
  if (!group)
    return NULL;

  return group->context;
}

void
GNUNET_CHAT_context_send_text (struct GNUNET_CHAT_Context *context,
			       const char *text)
{
  if ((!context) || (!text))
    return;

  struct GNUNET_MESSENGER_Message msg;
  msg.header.kind = GNUNET_MESSENGER_KIND_TEXT;
  msg.body.text.text = GNUNET_strdup(text);

  GNUNET_MESSENGER_send_message(context->room, &msg, NULL);

  GNUNET_free(msg.body.text.text);
}


void
GNUNET_CHAT_context_send_file (struct GNUNET_CHAT_Context *context,
			       const char *path)
{
  if ((!context) || (!path))
    return;

  // TODO: encrypt file, publish file, share file
}


void
GNUNET_CHAT_context_send_uri (struct GNUNET_CHAT_Context *context,
			      const char *uri)
{
  if ((!context) || (!uri))
    return;

  struct GNUNET_FS_Uri *furi = GNUNET_FS_uri_parse(uri, NULL);

  if (!furi)
    return;

  // TODO: download file, hash file, share file
}


void
GNUNET_CHAT_context_share_file (struct GNUNET_CHAT_Context *context,
				const struct GNUNET_CHAT_File *file)
{
  if ((!context) || (!file) || (strlen(file->name) > NAME_MAX))
    return;

  struct GNUNET_MESSENGER_Message msg;
  msg.header.kind = GNUNET_MESSENGER_KIND_FILE;
  GNUNET_memcpy(&(msg.body.file.key), &(file->key), sizeof(file->key));
  GNUNET_memcpy(&(msg.body.file.hash), &(file->hash), sizeof(file->hash));
  GNUNET_strlcpy(msg.body.file.name, file->name, NAME_MAX);
  msg.body.file.uri = GNUNET_FS_uri_to_string(file->uri);

  GNUNET_MESSENGER_send_message(context->room, &msg, NULL);

  GNUNET_free(msg.body.file.uri);
}


void
GNUNET_CHAT_context_delete_message (const struct GNUNET_CHAT_Message *message,
				    struct GNUNET_TIME_Relative delay)
{
  if (!message)
    return;

  struct GNUNET_MESSENGER_Message msg;
  msg.header.kind = GNUNET_MESSENGER_KIND_DELETE;
  GNUNET_memcpy(&(msg.body.delete.hash), &(message->hash), sizeof(message->hash));
  msg.body.delete.delay = GNUNET_TIME_relative_hton(delay);

  GNUNET_MESSENGER_send_message(message->context->room, &msg, NULL);
}



int
GNUNET_CHAT_context_iterate_messages (struct GNUNET_CHAT_Context *context,
				      GNUNET_CHAT_ContextMessageCallback callback,
				      void *cls)
{
  if (!context)
    return GNUNET_SYSERR;

  struct GNUNET_CHAT_ContextIterateMessages it;
  it.context = context;
  it.cb = callback;
  it.cls = cls;

  return GNUNET_CONTAINER_multihashmap_iterate(
      context->messages, it_context_iterate_messages, &it
  );
}


enum GNUNET_CHAT_MessageKind
GNUNET_CHAT_message_get_kind (const struct GNUNET_CHAT_Message *message)
{
  if (!message)
    return GNUNET_CHAT_KIND_UNKNOWN;

  switch (message->msg->header.kind)
  {
    case GNUNET_MESSENGER_KIND_INVITE:
      return GNUNET_CHAT_KIND_INVITATION;
    case GNUNET_MESSENGER_KIND_TEXT:
      return GNUNET_CHAT_KIND_TEXT;
    case GNUNET_MESSENGER_KIND_FILE:
      return GNUNET_CHAT_KIND_FILE;
    default:
      return GNUNET_CHAT_KIND_UNKNOWN;
  }
}


struct GNUNET_TIME_Absolute
GNUNET_CHAT_message_get_timestamp (const struct GNUNET_CHAT_Message *message)
{
  if (!message)
    return GNUNET_TIME_absolute_get_zero_();

  return GNUNET_TIME_absolute_ntoh(message->msg->header.timestamp);
}


const struct GNUNET_CHAT_Contact*
GNUNET_CHAT_message_get_sender (const struct GNUNET_CHAT_Message *message)
{
  if (!message)
    return NULL;

  const struct GNUNET_MESSENGER_Contact *sender = GNUNET_MESSENGER_get_sender(
      message->context->room, &(message->hash)
  );

  if (!sender)
    return NULL;

  struct GNUNET_ShortHashCode shorthash;
  util_shorthash_from_member(sender, &shorthash);

  return GNUNET_CONTAINER_multishortmap_get(
      message->context->handle->contacts, &shorthash
  );
}


int
GNUNET_CHAT_message_get_read_receipt (const struct GNUNET_CHAT_Message *message,
				      GNUNET_CHAT_MessageReadReceiptCallback callback,
				      void *cls)
{
  if (!message)
    return GNUNET_SYSERR;

  // TODO: request read receipt? / check if newer message was received of sender?

  return GNUNET_SYSERR;
}


const char*
GNUNET_CHAT_message_get_text (const struct GNUNET_CHAT_Message *message)
{
  if (!message)
    return NULL;

  if (GNUNET_MESSENGER_KIND_TEXT != message->msg->header.kind)
    return NULL;

  return message->msg->body.text.text;
}


struct GNUNET_CHAT_File*
GNUNET_CHAT_message_get_file (const struct GNUNET_CHAT_Message *message)
{
  if (!message)
    return NULL;

  if (GNUNET_MESSENGER_KIND_FILE != message->msg->header.kind)
    return NULL;

  return GNUNET_CONTAINER_multihashmap_get(
      message->context->handle->files,
      &(message->msg->body.file.hash)
  );
}


struct GNUNET_CHAT_Invitation*
GNUNET_CHAT_message_get_invitation (const struct GNUNET_CHAT_Message *message)
{
  if (!message)
    return NULL;

  if (GNUNET_MESSENGER_KIND_INVITE != message->msg->header.kind)
    return NULL;

  return GNUNET_CONTAINER_multihashmap_get(
      message->context->invites,
      &(message->hash)
  );
}


const struct GNUNET_HashCode*
GNUNET_CHAT_file_get_hash (const struct GNUNET_CHAT_File *file)
{
  if (!file)
    return NULL;

  return &(file->hash);
}


uint64_t
GNUNET_CHAT_file_get_size (const struct GNUNET_CHAT_File *file)
{
  if (!file)
    return 0;

  if (file->uri)
    return GNUNET_FS_uri_chk_get_file_size(file->uri);

  const char *path = ""; // TODO: path = download_directory + file->name

  // TODO: check size through info or check locally?

  return 0;
}


int
GNUNET_CHAT_file_is_local (const struct GNUNET_CHAT_File *file)
{
  if (!file)
    return GNUNET_SYSERR;

  const char *path = ""; // TODO: path = download_directory + file->name

  // TODO: check locally?

  return GNUNET_SYSERR;
}


int
GNUNET_CHAT_file_start_download (struct GNUNET_CHAT_File *file,
				 GNUNET_CHAT_MessageFileDownloadCallback callback,
				 void *cls)
{
  if (!file)
    return GNUNET_SYSERR;

  // TODO: check if downloading?

  const char *path = ""; // TODO: path = download_directory + file->name

  file->download = GNUNET_FS_download_start(
      file->handle->fs,
      file->uri,
      NULL,
      path,
      NULL,
      0,
      0,
      0,
      GNUNET_FS_DOWNLOAD_OPTION_NONE,
      NULL,
      NULL
  );

  return GNUNET_OK;
}


int
GNUNET_CHAT_file_pause_download (struct GNUNET_CHAT_File *file)
{
  if (!file)
    return GNUNET_SYSERR;

  GNUNET_FS_download_suspend(file->download);
  return GNUNET_OK;
}


int
GNUNET_CHAT_file_resume_download (struct GNUNET_CHAT_File *file)
{
  if (!file)
    return GNUNET_SYSERR;

  GNUNET_FS_download_resume(file->download);
  return GNUNET_OK;
}


int
GNUNET_CHAT_file_stop_download (struct GNUNET_CHAT_File *file)
{
  if (!file)
    return GNUNET_SYSERR;

  GNUNET_FS_download_stop(file->download, GNUNET_YES);
  file->download = NULL;
  return GNUNET_OK;
}


void
GNUNET_CHAT_invitation_accept (struct GNUNET_CHAT_Invitation *invitation)
{
  if (!invitation)
    return;

  struct GNUNET_PeerIdentity door;
  GNUNET_PEER_resolve(invitation->door, &door);

  GNUNET_MESSENGER_enter_room(
      invitation->context->handle->messenger,
      &door, &(invitation->key)
  );
}

