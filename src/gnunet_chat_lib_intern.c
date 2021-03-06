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
 * @file gnunet_chat_lib_intern.c
 */

#define GNUNET_UNUSED __attribute__ ((unused))

struct GNUNET_CHAT_HandleIterateContacts
{
  struct GNUNET_CHAT_Handle *handle;
  GNUNET_CHAT_ContactCallback cb;
  void *cls;
};

int
it_handle_iterate_contacts (void *cls,
			    GNUNET_UNUSED const struct GNUNET_ShortHashCode *key,
			    void *value)
{
  struct GNUNET_CHAT_HandleIterateContacts *it = cls;

  if (!(it->cb))
    return GNUNET_YES;

  struct GNUNET_CHAT_Contact *contact = value;

  return it->cb(it->cls, it->handle, contact);
}

struct GNUNET_CHAT_HandleIterateGroups
{
  struct GNUNET_CHAT_Handle *handle;
  GNUNET_CHAT_GroupCallback cb;
  void *cls;
};

int
it_handle_iterate_groups (void *cls,
			  GNUNET_UNUSED const struct GNUNET_HashCode *key,
			  void *value)
{
  struct GNUNET_CHAT_HandleIterateGroups *it = cls;

  if (!(it->cb))
    return GNUNET_YES;

  struct GNUNET_CHAT_Group *group = value;

  return it->cb(it->cls, it->handle, group);
}

struct GNUNET_CHAT_ContactFindRoom
{
  struct GNUNET_MESSENGER_Room *room;
};

int
it_contact_find_room (void *cls, struct GNUNET_MESSENGER_Room *room,
		      GNUNET_UNUSED const struct GNUNET_MESSENGER_Contact *member)
{
  struct GNUNET_CHAT_ContactFindRoom *find = cls;
  find->room = room;
  return GNUNET_NO;
}

struct GNUNET_CHAT_GroupIterateContacts
{
  struct GNUNET_CHAT_Group *group;
  GNUNET_CHAT_GroupContactCallback cb;
  void *cls;
};

int
it_group_iterate_contacts (void* cls,
			   GNUNET_UNUSED struct GNUNET_MESSENGER_Room *room,
                           const struct GNUNET_MESSENGER_Contact *member)
{
  struct GNUNET_CHAT_GroupIterateContacts *it = cls;

  if (!(it->cb))
    return GNUNET_YES;

  struct GNUNET_ShortHashCode shorthash;
  util_shorthash_from_member(member, &shorthash);

  struct GNUNET_CHAT_Contact *contact = GNUNET_CONTAINER_multishortmap_get(
      it->group->handle->contacts, &shorthash
  );

  return it->cb(it->cls, it->group, contact);
}

struct GNUNET_CHAT_ContextIterateMessages
{
  struct GNUNET_CHAT_Context *context;
  GNUNET_CHAT_ContextMessageCallback cb;
  void *cls;
};

int
it_context_iterate_messages (void *cls,
			     GNUNET_UNUSED const struct GNUNET_HashCode *key,
			     void *value)
{
  struct GNUNET_CHAT_ContextIterateMessages *it = cls;

  if (!(it->cb))
    return GNUNET_YES;

  struct GNUNET_CHAT_Message *message = value;

  return it->cb(it->cls, it->context, message);
}

struct GNUNET_CHAT_ContextIterateFiles
{
  struct GNUNET_CHAT_Context *context;
  GNUNET_CHAT_ContextFileCallback cb;
  void *cls;
};

int
it_context_iterate_files (void *cls,
			  GNUNET_UNUSED const struct GNUNET_HashCode *key,
			  void *value)
{
  struct GNUNET_CHAT_ContextIterateFiles *it = cls;

  if (!(it->cb))
    return GNUNET_YES;

  struct GNUNET_CHAT_File *file = value;

  return it->cb(it->cls, it->context, file);
}

struct GNUNET_CHAT_MessageIterateReadReceipts
{
  const struct GNUNET_CHAT_Message *message;
  GNUNET_CHAT_MessageReadReceiptCallback cb;
  void *cls;
};

int
it_message_iterate_read_receipts (void *cls,
				  GNUNET_UNUSED struct GNUNET_MESSENGER_Room *room,
				  const struct GNUNET_MESSENGER_Contact *member)
{
  struct GNUNET_CHAT_MessageIterateReadReceipts *it = cls;
  struct GNUNET_CHAT_Handle *handle = it->message->context->handle;

  if (!handle)
    return GNUNET_NO;

  struct GNUNET_ShortHashCode shorthash;
  util_shorthash_from_member(member, &shorthash);

  struct GNUNET_CHAT_Contact *contact = GNUNET_CONTAINER_multishortmap_get(
      handle->contacts, &shorthash
  );

  if (!contact)
    return GNUNET_YES;

  struct GNUNET_TIME_Absolute *timestamp = GNUNET_CONTAINER_multishortmap_get(
      it->message->context->timestamps, &shorthash
  );

  if (!timestamp)
    return GNUNET_YES;

  struct GNUNET_TIME_Relative delta = GNUNET_TIME_absolute_get_difference(
      *timestamp, GNUNET_CHAT_message_get_timestamp(it->message)
  );

  int read_receipt;
  if (GNUNET_TIME_relative_get_zero_().rel_value_us == delta.rel_value_us)
    read_receipt = GNUNET_YES;
  else
    read_receipt = GNUNET_NO;

  if (it->cb)
    it->cb(it->cls, it->message, contact, read_receipt);

  return GNUNET_YES;
}
