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
 * @file gnunet_chat_handle.c
 */

#include "gnunet_chat_lib.h"
#include "gnunet_chat_handle.h"
#include "gnunet_chat_group.h"
#include "gnunet_chat_contact.h"
#include "gnunet_chat_message.h"
#include "gnunet_chat_file.h"

static void*
handle_fs_progress(void* cls, const struct GNUNET_FS_ProgressInfo* info)
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
    } case GNUNET_FS_STATUS_PUBLISH_PROGRESS: {
      /*publication_t* publication = (publication_t*) info->value.publish.cctx;
      publication->progress = 1.0f * info->value.publish.completed / info->value.publish.size;

      GNUNET_SCHEDULER_add_now(&CGTK_publication_progress, publication);

      return publication;*/
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
    } case GNUNET_FS_STATUS_DOWNLOAD_ACTIVE: {
      return info->value.download.cctx;
    } case GNUNET_FS_STATUS_DOWNLOAD_INACTIVE: {
      return info->value.download.cctx;
    } case GNUNET_FS_STATUS_DOWNLOAD_PROGRESS: {
      /*request_t* request = (request_t*) info->value.download.cctx;
      request->progress = 1.0f * info->value.download.completed / info->value.download.size;

      GNUNET_SCHEDULER_add_now(&CGTK_request_progress, request);

      return request;*/
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
    } case GNUNET_FS_STATUS_UNINDEX_PROGRESS: {
      /*publication_t* publication = (publication_t*) info->value.unindex.cctx;
      publication->progress = 1.0f * info->value.unindex.completed / info->value.unindex.size;

      return publication;*/
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

static void
handle_on_message (void *cls,
		   GNUNET_UNUSED struct GNUNET_MESSENGER_Room *room,
		   GNUNET_UNUSED const struct GNUNET_MESSENGER_Contact *sender,
		   const struct GNUNET_MESSENGER_Message *message,
		   const struct GNUNET_HashCode *hash,
		   GNUNET_UNUSED enum GNUNET_MESSENGER_MessageFlags flags)
{
  struct GNUNET_CHAT_Handle *chat = cls;

  //TODO

  struct GNUNET_CHAT_Message msg;
  GNUNET_memcpy(&(msg.hash), hash, sizeof(msg.hash));
  msg.message = message;

  if ((GNUNET_CHAT_KIND_UNKNOWN == GNUNET_CHAT_message_get_kind(&msg)) ||
      (!chat->msg_cb))
    return;

  chat->msg_cb(chat->msg_cls, NULL /* TODO */, &msg);
}

struct GNUNET_CHAT_CheckRoomMembers
{
  const struct GNUNET_IDENTITY_PublicKey *ignore_key;
  const struct GNUNET_MESSENGER_Contact *contact;
};

static int
handle_check_room_members (void* cls,
			   GNUNET_UNUSED struct GNUNET_MESSENGER_Room *room,
                           const struct GNUNET_MESSENGER_Contact *contact)
{
  struct GNUNET_CHAT_CheckRoomMembers *check = cls;
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

static int
handle_initialize_context (void *cls, struct GNUNET_MESSENGER_Room *room,
			   GNUNET_UNUSED const struct GNUNET_MESSENGER_Contact *contact)
{
  struct GNUNET_CHAT_Handle *chat = cls;

  struct GNUNET_CHAT_CheckRoomMembers check;
  check.ignore_key = GNUNET_CHAT_get_key(chat);
  check.contact = NULL;

  const int amount = GNUNET_MESSENGER_iterate_members(
      room, handle_check_room_members, &check
  );

  if (amount <= 1)
    return GNUNET_YES;

  const struct GNUNET_HashCode *key = GNUNET_MESSENGER_room_get_key(room);
  struct GNUNET_CHAT_Context *context = handle_get_chat_context(chat, key);
  enum GNUNET_CHAT_ContextType type = GNUNET_CHAT_CONTEXT_TYPE_UNKNOWN;

  if (check.contact)
    type = GNUNET_CHAT_CONTEXT_TYPE_CONTACT;
  else
    type = GNUNET_CHAT_CONTEXT_TYPE_GROUP;

  if (!context)
    context = context_create(chat, type, key);
  else
    context->type = type;

  if (GNUNET_YES != handle_update_chat_context(chat, context, GNUNET_NO))
    context_destroy(context);

  return GNUNET_YES;
}

static void
handle_on_identity(void *cls, struct GNUNET_MESSENGER_Handle *handle)
{
  struct GNUNET_CHAT_Handle *chat = cls;

  //TODO

  GNUNET_MESSENGER_find_rooms(
      chat->handles.messenger, NULL, handle_initialize_context, handle
  );
}

static void
handle_arm_connection(void *cls, int connected)
{
  struct GNUNET_CHAT_Handle *chat = cls;

  if (GNUNET_YES == connected) {
    GNUNET_ARM_request_service_start(chat->handles.arm, "messenger",
				     GNUNET_OS_INHERIT_STD_NONE, NULL, NULL);
    GNUNET_ARM_request_service_start(chat->handles.arm, "fs",
				     GNUNET_OS_INHERIT_STD_NONE, NULL, NULL);
  } else {
    GNUNET_ARM_request_service_start(chat->handles.arm, "arm",
				     GNUNET_OS_INHERIT_STD_NONE, NULL, NULL);
  }
}

struct GNUNET_CHAT_Handle*
GNUNET_CHAT_start (const struct GNUNET_CONFIGURATION_Handle* cfg,
		   const char *name,
		   GNUNET_CHAT_WarningCallback warn_cb,
		   void *warn_cls,
		   GNUNET_CHAT_ContextMessageCallback msg_cb,
		   void *msg_cls)
{
  if (!cfg)
    return NULL;

  struct GNUNET_CHAT_Handle *chat = GNUNET_new(struct GNUNET_CHAT_Handle);
  memset(chat, 0, sizeof(*chat));
  chat->cfg = cfg;

  chat->warn_cb = warn_cb;
  chat->warn_cls = warn_cls;

  chat->handles.arm = GNUNET_ARM_connect(cfg, &handle_arm_connection, chat);

  if (chat->handles.arm)
    handle_arm_connection(chat, GNUNET_NO);

  chat->handles.messenger = GNUNET_MESSENGER_connect(
      cfg, name,
      handle_on_identity, chat,
      handle_on_message, chat
  );

  if (!chat->handles.messenger)
  {
    GNUNET_CHAT_stop(chat);
    return NULL;
  }

  chat->handles.fs = GNUNET_FS_start(
      cfg,
      name,
      handle_fs_progress,
      chat,
      GNUNET_FS_FLAGS_NONE,
      GNUNET_FS_OPTIONS_END
  );

  chat->contacts.short_map = GNUNET_CONTAINER_multishortmap_create(8, GNUNET_NO);
  chat->contacts.hash_map = GNUNET_CONTAINER_multihashmap_create(8, GNUNET_NO);

  chat->groups = GNUNET_CONTAINER_multihashmap_create(8, GNUNET_NO);
  chat->contexts = GNUNET_CONTAINER_multihashmap_create(8, GNUNET_NO);
  chat->files = GNUNET_CONTAINER_multihashmap_create(8, GNUNET_NO);

  chat->msg_cb = msg_cb;
  chat->msg_cls = msg_cls;

  return chat;
}

static int
handle_iterate_destroy_contacts (GNUNET_UNUSED void *cls,
				 GNUNET_UNUSED const struct GNUNET_HashCode *key,
				 void *value)
{
  struct GNUNET_CHAT_Contact *contact = value;
  contact_destroy(contact);
  return GNUNET_YES;
}

static int
handle_iterate_destroy_groups (GNUNET_UNUSED void *cls,
			       GNUNET_UNUSED const struct GNUNET_HashCode *key,
			       void *value)
{
  struct GNUNET_CHAT_Group *group = value;
  group_destroy(group);
  return GNUNET_YES;
}

void
GNUNET_CHAT_stop (struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return;

  if (handle->handles.fs)
   {
     // TODO: stop each action

     GNUNET_FS_stop(handle->handles.fs);
     handle->handles.fs = NULL;
   }

   if (handle->handles.messenger)
   {
     // TODO: stop everything related

     GNUNET_MESSENGER_disconnect(handle->handles.messenger);
     handle->handles.messenger = NULL;
   }

  if (handle->groups)
  {
    GNUNET_CONTAINER_multihashmap_iterate(
	handle->groups, handle_iterate_destroy_groups, NULL
    );

    GNUNET_CONTAINER_multihashmap_destroy(handle->groups);
    handle->groups = NULL;
  }

  if (handle->contacts.hash_map)
  {
    GNUNET_CONTAINER_multihashmap_iterate(
	handle->contacts.hash_map, handle_iterate_destroy_contacts, NULL
    );

    GNUNET_CONTAINER_multihashmap_destroy(handle->contacts.hash_map);
    handle->contacts.hash_map = NULL;
  }

  if (handle->contacts.short_map)
  {
    GNUNET_CONTAINER_multishortmap_destroy(handle->contacts.short_map);
    handle->contacts.short_map = NULL;
  }

  if (handle->handles.arm)
  {
    //TODO: stop started services?

    GNUNET_ARM_disconnect(handle->handles.arm);
    handle->handles.arm = NULL;
  }

  GNUNET_free(handle);
}

int
GNUNET_CHAT_update (struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return GNUNET_SYSERR;

  return GNUNET_MESSENGER_update(handle->handles.messenger);
}

int
GNUNET_CHAT_set_name (struct GNUNET_CHAT_Handle *handle,
		      const char *name)
{
  if (!handle)
    return GNUNET_SYSERR;

  return GNUNET_MESSENGER_set_name(handle->handles.messenger, name);
}

const char*
GNUNET_CHAT_get_name (const struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return NULL;

  return GNUNET_MESSENGER_get_name(handle->handles.messenger);
}

const struct GNUNET_IDENTITY_PublicKey*
GNUNET_CHAT_get_key (const struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return NULL;

  return GNUNET_MESSENGER_get_key(handle->handles.messenger);
}

struct GNUNET_CHAT_IterateContacts
{
  struct GNUNET_CHAT_Handle *handle;
  GNUNET_CHAT_ContactCallback callback;
  void *cls;
};

static int
handle_iterate_contacts(void *cls,
			GNUNET_UNUSED const struct GNUNET_HashCode *key,
			void *value)
{
  struct GNUNET_CHAT_IterateContacts *iterate = cls;
  struct GNUNET_CHAT_Contact *contact = value;

  if (!iterate->callback)
    return GNUNET_YES;

  return iterate->callback(iterate->cls, iterate->handle, contact);
}

int
GNUNET_CHAT_iterate_contacts (struct GNUNET_CHAT_Handle *handle,
			      GNUNET_CHAT_ContactCallback callback,
			      void *cls)
{
  if (!handle)
    return GNUNET_SYSERR;

  struct GNUNET_CHAT_IterateContacts iterate;
  iterate.handle = handle;
  iterate.callback = callback;
  iterate.cls = cls;

  return GNUNET_CONTAINER_multihashmap_iterate(handle->contacts.hash_map,
					       handle_iterate_contacts,
					       &iterate);
}

struct GNUNET_CHAT_Group*
GNUNET_CHAT_group_create (struct GNUNET_CHAT_Handle *handle,
			  const char *topic)
{
  if (!handle)
    return NULL;

  struct GNUNET_CHAT_Group *group = group_create(handle, topic);

  if (!group)
    return NULL;

  if (GNUNET_YES != handle_update_chat_group(handle, group, GNUNET_NO))
  {
    group_destroy(group);
    return NULL;
  }

  return group;
}

struct GNUNET_CHAT_IterateGroups
{
  struct GNUNET_CHAT_Handle *handle;
  GNUNET_CHAT_GroupCallback callback;
  void *cls;
};

static int
handle_iterate_groups(void *cls,
		      GNUNET_UNUSED const struct GNUNET_HashCode *key,
		      void *value)
{
  struct GNUNET_CHAT_IterateGroups *iterate = cls;
  struct GNUNET_CHAT_Group *group = value;

  if (!iterate->callback)
    return GNUNET_YES;

  return iterate->callback(iterate->cls, iterate->handle, group);
}

int
GNUNET_CHAT_iterate_groups (struct GNUNET_CHAT_Handle *handle,
			    GNUNET_CHAT_GroupCallback callback,
			    void *cls)
{
  if (!handle)
    return GNUNET_SYSERR;

  struct GNUNET_CHAT_IterateGroups iterate;
  iterate.handle = handle;
  iterate.callback = callback;
  iterate.cls = cls;

  return GNUNET_CONTAINER_multihashmap_iterate(handle->groups,
					       handle_iterate_groups,
					       &iterate);
}

int
handle_update_chat_contact (struct GNUNET_CHAT_Handle *handle,
			    struct GNUNET_CHAT_Contact *chatContact,
			    int removeContact)
{
  const struct GNUNET_HashCode *key = context_get_key(chatContact->context);

  if (GNUNET_YES == removeContact)
  {
    const int result = GNUNET_CONTAINER_multihashmap_remove(
	handle->contacts.hash_map, key, chatContact
    );

    if (GNUNET_YES == result)
      return handle_update_chat_context(handle, chatContact->context,
					GNUNET_YES);
    else
      return GNUNET_NO;
  }
  else
  {
    const int result = GNUNET_CONTAINER_multihashmap_put(
	handle->contacts.hash_map, key, chatContact,
	GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_ONLY
    );

    if (GNUNET_OK != result)
      return GNUNET_NO;
    else
      return handle_update_chat_context(handle, chatContact->context,
					GNUNET_NO);
  }
}

int
handle_update_chat_group (struct GNUNET_CHAT_Handle *handle,
			  struct GNUNET_CHAT_Group *chatGroup,
			  int removeGroup)
{
  const struct GNUNET_HashCode *key = context_get_key(chatGroup->context);

  if (GNUNET_YES == removeGroup)
  {
    const int result = GNUNET_CONTAINER_multihashmap_remove(
      handle->groups, key, chatGroup
    );

    if (GNUNET_YES == result)
      return handle_update_chat_context(handle, chatGroup->context,
					GNUNET_YES);
    else
      return GNUNET_NO;
  }
  else
  {
    const int result = GNUNET_CONTAINER_multihashmap_put(
      handle->groups, key, chatGroup,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_ONLY
    );

    if (GNUNET_OK != result)
      return GNUNET_NO;
    else
      return handle_update_chat_context(handle, chatGroup->context,
					GNUNET_NO);
  }
}

int
handle_update_chat_context (struct GNUNET_CHAT_Handle *handle,
			    struct GNUNET_CHAT_Context *context,
			    int removeContext)
{
  const struct GNUNET_HashCode *key = context_get_key(context);

  if (GNUNET_YES == removeContext)
    return GNUNET_CONTAINER_multihashmap_remove(
      handle->contexts, key, context
    );
  else
  {
    const int result = GNUNET_CONTAINER_multihashmap_put(
      handle->contexts, key, context,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_ONLY
    );

    if (GNUNET_OK != result)
      return GNUNET_NO;
    else
      return GNUNET_YES;
  }
}

int
handle_update_chat_file (struct GNUNET_CHAT_Handle *handle,
			 struct GNUNET_CHAT_File *file,
			 int removeFile)
{
  const struct GNUNET_HashCode *hash = GNUNET_CHAT_file_get_hash(file);

  if (GNUNET_YES == removeFile)
    return GNUNET_CONTAINER_multihashmap_remove(
      handle->files, hash, file
    );
  else
  {
    const int result = GNUNET_CONTAINER_multihashmap_put(
      handle->files, hash, file,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_ONLY
    );

    if (GNUNET_OK != result)
      return GNUNET_NO;
    else
      return GNUNET_YES;
  }
}

static void
handle_get_short_of_contact(struct GNUNET_ShortHashCode *shortHash,
			    const struct GNUNET_MESSENGER_Contact *contact)
{
  memset(shortHash, 0, sizeof(*shortHash));
  GNUNET_memcpy(shortHash, &contact, sizeof(contact));
}

struct GNUNET_CHAT_Contact*
handle_get_chat_contact (struct GNUNET_CHAT_Handle *handle,
			 const struct GNUNET_MESSENGER_Contact *contact)
{
  struct GNUNET_ShortHashCode shortHash;
  handle_get_short_of_contact (&shortHash, contact);

  struct GNUNET_CHAT_Contact* chatContact = GNUNET_CONTAINER_multishortmap_get(
      handle->contacts.short_map, &shortHash
  );

  return chatContact;
}

void
handle_set_chat_contact (struct GNUNET_CHAT_Handle *handle,
			 const struct GNUNET_MESSENGER_Contact *contact,
			 struct GNUNET_CHAT_Contact *chatContact)
{
  struct GNUNET_ShortHashCode shortHash;
  handle_get_short_of_contact (&shortHash, contact);

  if (chatContact)
  {
    GNUNET_CONTAINER_multishortmap_remove_all(handle->contacts.short_map,
					      &shortHash);
    return;
  }

  GNUNET_CONTAINER_multishortmap_put(
      handle->contacts.short_map, &shortHash, chatContact,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_REPLACE
  );
}
