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
 * @file gnunet_chat_handle_intern.c
 */

#include "gnunet_chat_contact.h"
#include "gnunet_chat_context.h"
#include "gnunet_chat_group.h"
#include "gnunet_chat_handle.h"
#include "gnunet_chat_message.h"
#include "gnunet_chat_util.h"

#define GNUNET_UNUSED __attribute__ ((unused))

void
on_handle_arm_connection(void *cls, int connected)
{
  struct GNUNET_CHAT_Handle *chat = cls;

  if (GNUNET_YES == connected) {
    GNUNET_ARM_request_service_start(
	chat->arm, "messenger",
	GNUNET_OS_INHERIT_STD_NONE,
	NULL, NULL
    );

    GNUNET_ARM_request_service_start(
	chat->arm, "fs",
	GNUNET_OS_INHERIT_STD_NONE,
	NULL, NULL
    );
  } else {
    GNUNET_ARM_request_service_start(
	chat->arm, "arm",
	GNUNET_OS_INHERIT_STD_NONE,
	NULL, NULL
    );
  }
}

void*
notify_handle_fs_progress(void* cls, const struct GNUNET_FS_ProgressInfo* info)
{
  struct GNUNET_CHAT_Handle *chat = cls;

  if (!chat)
    return NULL;

  switch (info->status) {
    case GNUNET_FS_STATUS_PUBLISH_START: {
      /*publication_t* publication = (publication_t*) info->value.publish.cctx;
      publication->progress = 0.0f;

      GNUNET_SCHEDULER_add_now(&CGTK_publication_progress, publication);

      return publication;*/
      break;
    } case GNUNET_FS_STATUS_PUBLISH_PROGRESS: {
      /*publication_t* publication = (publication_t*) info->value.publish.cctx;
      publication->progress = 1.0f * info->value.publish.completed / info->value.publish.size;

      GNUNET_SCHEDULER_add_now(&CGTK_publication_progress, publication);

      return publication;*/
      break;
    } case GNUNET_FS_STATUS_PUBLISH_COMPLETED: {
      /*publication_t* publication = (publication_t*) info->value.publish.cctx;
      publication->uri = GNUNET_FS_uri_dup(info->value.publish.specifics.completed.chk_uri);
      publication->progress = 1.0f;

      GNUNET_SCHEDULER_add_now(&CGTK_publication_finish, publication);*/
      break;
    } case GNUNET_FS_STATUS_PUBLISH_ERROR: {
      /*publication_t* publication = (publication_t*) info->value.publish.cctx;

      GNUNET_SCHEDULER_add_now(&CGTK_publication_error, publication);*/
      break;
    } case GNUNET_FS_STATUS_DOWNLOAD_START: {
      /*request_t* request = (request_t*) info->value.download.cctx;
      request->progress = 0.0f;

      return request;*/
      break;
    } case GNUNET_FS_STATUS_DOWNLOAD_ACTIVE: {
      return info->value.download.cctx;
    } case GNUNET_FS_STATUS_DOWNLOAD_INACTIVE: {
      return info->value.download.cctx;
    } case GNUNET_FS_STATUS_DOWNLOAD_PROGRESS: {
      /*request_t* request = (request_t*) info->value.download.cctx;
      request->progress = 1.0f * info->value.download.completed / info->value.download.size;

      GNUNET_SCHEDULER_add_now(&CGTK_request_progress, request);

      return request;*/
      break;
    } case GNUNET_FS_STATUS_DOWNLOAD_COMPLETED: {
      /*request_t* request = (request_t*) info->value.download.cctx;
      request->progress = 1.0f;

      GNUNET_SCHEDULER_add_now(&CGTK_request_finish, request);*/
      break;
    } case GNUNET_FS_STATUS_DOWNLOAD_ERROR: {
      /*request_t *request = (request_t *) info->value.download.cctx;

      GNUNET_SCHEDULER_add_now(&CGTK_request_error, request);*/
      break;
    } case GNUNET_FS_STATUS_UNINDEX_START: {
      /*publication_t* publication = (publication_t*) info->value.unindex.cctx;
      publication->progress = 0.0f;

      return publication;*/
      break;
    } case GNUNET_FS_STATUS_UNINDEX_PROGRESS: {
      /*publication_t* publication = (publication_t*) info->value.unindex.cctx;
      publication->progress = 1.0f * info->value.unindex.completed / info->value.unindex.size;

      return publication;*/
      break;
    } case GNUNET_FS_STATUS_UNINDEX_COMPLETED: {
      /*publication_t* publication = (publication_t*) info->value.unindex.cctx;
      publication->progress = 1.0f;

      GNUNET_SCHEDULER_add_now(&CGTK_publication_unindex_finish, publication);*/
      break;
    } default: {
      break;
    }
  }

  return NULL;
}

struct GNUNET_CHAT_CheckHandleRoomMembers
{
  const struct GNUNET_IDENTITY_PublicKey *ignore_key;
  const struct GNUNET_MESSENGER_Contact *contact;
};

int
check_handle_room_members (void* cls,
			   GNUNET_UNUSED struct GNUNET_MESSENGER_Room *room,
                           const struct GNUNET_MESSENGER_Contact *contact)
{
  struct GNUNET_CHAT_CheckHandleRoomMembers *check = cls;

  const struct GNUNET_IDENTITY_PublicKey *contact_key = (
      GNUNET_MESSENGER_contact_get_key(contact)
  );

  if (0 == GNUNET_memcmp(contact_key, check->ignore_key))
    return GNUNET_YES;

  if (check->contact)
    return GNUNET_NO;

  check->contact = contact;
  return GNUNET_YES;
}

struct GNUNET_CHAT_Context*
request_handle_context_by_room (struct GNUNET_CHAT_Handle *handle,
				struct GNUNET_MESSENGER_Room *room)
{
  const struct GNUNET_HashCode *key = GNUNET_MESSENGER_room_get_key(room);

  struct GNUNET_CHAT_Context *context = GNUNET_CONTAINER_multihashmap_get(
      handle->contexts, key
  );

  if (context)
    return context;

  context = context_create_from_room(handle, room);

  if (GNUNET_OK != GNUNET_CONTAINER_multihashmap_put(
      handle->contexts, key, context,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_FAST))
  {
    context_destroy(context);
    return NULL;
  }

  struct GNUNET_CHAT_CheckHandleRoomMembers check;
  check.ignore_key = GNUNET_MESSENGER_get_key(handle->messenger);
  check.contact = NULL;

  GNUNET_MESSENGER_iterate_members(room, check_handle_room_members, &check);

  if (check.contact)
  {
    context->type = GNUNET_CHAT_CONTEXT_TYPE_CONTACT;

    struct GNUNET_CHAT_Contact *contact = contact_create_from_member(
      handle, check.contact
    );

    struct GNUNET_ShortHashCode shorthash;
    util_shorthash_from_member(check.contact, &shorthash);

    if (GNUNET_OK == GNUNET_CONTAINER_multishortmap_put(
      handle->contacts, &shorthash, contact,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_FAST))
      return context;

    contact_destroy(contact);
  }
  else
  {
    context->type = GNUNET_CHAT_CONTEXT_TYPE_GROUP;

    struct GNUNET_CHAT_Group *group = group_create_from_context(
      handle, context
    );

    if (GNUNET_OK == GNUNET_CONTAINER_multihashmap_put(
      handle->groups, key, group,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_FAST))
      return context;

    group_destroy(group);
  }

  GNUNET_CONTAINER_multihashmap_remove(handle->contexts, key, context);
  context_destroy(context);
  return NULL;
}

int
find_handle_rooms (void *cls, struct GNUNET_MESSENGER_Room *room,
		   GNUNET_UNUSED const struct GNUNET_MESSENGER_Contact *member)
{
  struct GNUNET_CHAT_Handle *handle = cls;

  request_handle_context_by_room(
      handle, room
  );

  return GNUNET_YES;
}

void
on_handle_identity(void *cls,
		   GNUNET_UNUSED struct GNUNET_MESSENGER_Handle *messenger)
{
  struct GNUNET_CHAT_Handle *handle = cls;

  GNUNET_MESSENGER_find_rooms(
      handle->messenger, NULL, find_handle_rooms, handle
  );
}

void
on_handle_message (void *cls,
		   struct GNUNET_MESSENGER_Room *room,
		   GNUNET_UNUSED const struct GNUNET_MESSENGER_Contact *sender,
		   const struct GNUNET_MESSENGER_Message *msg,
		   const struct GNUNET_HashCode *hash,
		   GNUNET_UNUSED enum GNUNET_MESSENGER_MessageFlags flags)
{
  struct GNUNET_CHAT_Handle *handle = cls;

  struct GNUNET_CHAT_Context *context = request_handle_context_by_room(
      handle, room
  );

  if (!context)
    return;

  struct GNUNET_CHAT_Message *message = GNUNET_CONTAINER_multihashmap_get(
      context->messages, hash
  );

  if (message)
    goto process_callback;

  message = message_create_from_msg(context, hash, msg);

  if (GNUNET_OK != GNUNET_CONTAINER_multihashmap_put(
      context->messages, hash, message,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_FAST))
  {
    message_destroy(message);
    return;
  }

process_callback:
  if (handle->msg_cb)
    handle->msg_cb(handle->msg_cls, context, message);
}
