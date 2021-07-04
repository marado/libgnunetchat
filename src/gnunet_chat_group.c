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
 * @file gnunet_chat_group.c
 */

#include "gnunet_chat_lib.h"
#include "gnunet_chat_group.h"
#include "gnunet_chat_handle.h"
#include "gnunet_chat_contact.h"

struct GNUNET_CHAT_Group*
group_create(struct GNUNET_CHAT_Handle *handle,
	     const char *topic)
{
  struct GNUNET_HashCode topic_key;

  if (topic) {
    GNUNET_CRYPTO_hash(topic, strlen(topic), &topic_key);
  }

  struct GNUNET_CHAT_Group *group = GNUNET_new(struct GNUNET_CHAT_Group);

  group->handle = handle;
  group->context = context_create(
      handle,
      GNUNET_CHAT_CONTEXT_TYPE_GROUP,
      topic? &topic_key : NULL
  );

  group->is_public = topic? GNUNET_YES : GNUNET_NO;
  group->announcement = NULL;
  group->search = NULL;

  if (!group->context)
  {
    group_destroy (group);
    return NULL;
  }

  const struct GNUNET_HashCode *key = context_get_key(group->context);

  const int result = GNUNET_CONTAINER_multihashmap_put(
      handle->groups,
      key,
      group,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_FAST
  );

  if (GNUNET_OK != result) {
      group_destroy(group);
      return NULL;
  }

  return group;
}

void
group_destroy(struct GNUNET_CHAT_Group* group)
{
  if (!group->context)
    goto skip_context;

  struct GNUNET_CHAT_Handle *handle = group->handle;
  const struct GNUNET_HashCode *key = context_get_key(group->context);

  GNUNET_CONTAINER_multihashmap_remove(
      handle->groups,
      key,
      group
  );

  context_destroy(group->context);

skip_context:
  GNUNET_free(group);
}

int
GNUNET_CHAT_group_leave (struct GNUNET_CHAT_Group *group)
{
  if (!group)
    return GNUNET_SYSERR;

  if (GNUNET_YES != handle_update_chat_group(group->handle, group, GNUNET_YES))
    return GNUNET_SYSERR;

  if (group->context)
    GNUNET_MESSENGER_close_room(group->context->room);

  group_destroy(group);
  return GNUNET_OK;
}

void
GNUNET_CHAT_group_set_name (struct GNUNET_CHAT_Group *group,
			    const char *name)
{
  if (!group)
    return;

  if (group->context)
    context_set_nick(group->context, name);
}

const char*
GNUNET_CHAT_group_get_name (const struct GNUNET_CHAT_Group *group)
{
  if (!group)
    return NULL;

  if (!group->context)
    return NULL;

  return context_get_nick(group->context);
}

void
GNUNET_CHAT_group_invite_contact (struct GNUNET_CHAT_Group *group,
				  struct GNUNET_CHAT_Contact *contact)
{
  if ((!group) || (!contact))
    return;

  struct GNUNET_CHAT_Context *context = GNUNET_CHAT_contact_get_context(contact);

  if (!context)
    return;

  struct GNUNET_MESSENGER_Handle *messenger = group->handle->handles.messenger;
  GNUNET_MESSENGER_open_room(messenger, context_get_key(group->context));

  struct GNUNET_MESSENGER_Message message;
  message.header.kind = GNUNET_MESSENGER_KIND_INVITE;

  int result = GNUNET_CRYPTO_get_peer_identity(
      group->handle->cfg,
      &(message.body.invite.door)
  );

  if (GNUNET_OK != result)
    return;

  GNUNET_memcpy(
      &(message.body.invite.key),
      context_get_key(group->context),
      sizeof(message.body.invite.key)
  );

  GNUNET_MESSENGER_send_message(context->room, &message, NULL);
}

struct GNUNET_CHAT_GroupIterateContacts
{
  struct GNUNET_CHAT_Group *group;
  GNUNET_CHAT_GroupContactCallback callback;
  void *cls;
};

static int
group_call_member (void* cls, struct GNUNET_CHAT_Handle *handle,
		   struct GNUNET_CHAT_Contact *contact)
{
  struct GNUNET_CHAT_GroupIterateContacts *iterate = cls;

  return iterate->callback(iterate->cls, iterate->group, contact);
}

static int
group_iterate_members (void* cls, struct GNUNET_MESSENGER_Room *room,
                       const struct GNUNET_MESSENGER_Contact *contact)
{
  struct GNUNET_CHAT_GroupIterateContacts *iterate = cls;

  return contact_call(
      iterate->group->handle,
      contact,
      group_call_member,
      iterate
  );
}

int
GNUNET_CHAT_group_iterate_contacts (struct GNUNET_CHAT_Group *group,
				    GNUNET_CHAT_GroupContactCallback callback,
				    void *cls)
{
  if (!group)
    return GNUNET_SYSERR;

  if (!callback)
    return GNUNET_MESSENGER_iterate_members(group->context->room, NULL, cls);

  struct GNUNET_CHAT_GroupIterateContacts iterate;
  iterate.group = group;
  iterate.callback = callback;
  iterate.cls = cls;

  return GNUNET_MESSENGER_iterate_members(
      group->context->room, group_iterate_members, &iterate
  );
}

struct GNUNET_CHAT_Context*
GNUNET_CHAT_group_get_context (struct GNUNET_CHAT_Group *group)
{
  if (!group)
    return NULL;

  return group->context;
}
