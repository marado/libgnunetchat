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
 * @file gnunet_chat_contact.c
 */

#include "gnunet_chat_lib.h"
#include "gnunet_chat_contact.h"
#include "gnunet_chat_handle.h"

struct GNUNET_CHAT_Contact*
contact_create (struct GNUNET_CHAT_Handle *handle,
		const struct GNUNET_MESSENGER_Contact *msg_contact)
{
  struct GNUNET_CHAT_Contact* contact = GNUNET_new(struct GNUNET_CHAT_Contact);

  contact->handle = handle;
  contact->context = context_create(
      handle,
      GNUNET_CHAT_CONTEXT_TYPE_CONTACT,
      NULL
  ); // TODO: check for existing context?

  contact->contact = msg_contact;

  if (!contact->context)
  {
    contact_destroy (contact);
    return NULL;
  }

  const struct GNUNET_HashCode *key = context_get_key(contact->context);

  const int result = GNUNET_CONTAINER_multihashmap_put(
      handle->contacts,
      key,
      contact,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_FAST
  );

  if (GNUNET_OK != result) {
    contact_destroy (contact);
    return NULL;
  }

  return contact;
}

void
contact_destroy (struct GNUNET_CHAT_Contact *contact)
{
  if (!contact->context)
    goto skip_context;

  struct GNUNET_CHAT_Handle *handle = contact->handle;
  const struct GNUNET_HashCode *key = context_get_key(contact->context);

  GNUNET_CONTAINER_multihashmap_remove(
      handle->contacts,
      key,
      contact
  );

  context_destroy(contact->context);

skip_context:
  GNUNET_free(contact);
}

struct GNUNET_CHAT_SearchContact
{
  struct GNUNET_MESSENGER_Contact *contact;
  const struct GNUNET_MESSENGER_Contact *msg_contact;
};

static int
contact_search (void *cls, const struct GNUNET_HashCode *key, void *value)
{
  struct GNUNET_CHAT_Contact *contact = value;
  struct GNUNET_CHAT_SearchContact *search = cls;

  if (contact->contact == search->msg_contact)
  {
    search->contact = contact;
    return GNUNET_NO;
  }

  return GNUNET_YES;
}

int
contact_call (struct GNUNET_CHAT_Handle *handle,
	      const struct GNUNET_MESSENGER_Contact *msg_contact,
	      GNUNET_CHAT_ContactCallback callback,
	      void *cls)
{
  struct GNUNET_CHAT_SearchContact search;
  search.contact = NULL;
  search.msg_contact = msg_contact;

  GNUNET_CONTAINER_multihashmap_iterate(
      handle->contacts,
      contact_search,
      &search
  );

  if (search.contact)
    return callback(cls, handle, search.contact);

  search.contact = contact_create(handle, msg_contact);
  int result = callback(cls, handle, search.contact);

  // TODO: check if contact has private chat

  contact_destroy(search.contact);
  return result;
}

int
GNUNET_CHAT_contact_delete (struct GNUNET_CHAT_Contact *contact)
{
  if (!contact)
    return GNUNET_SYSERR;

  if (GNUNET_YES != handle_update_chat_contact(contact->handle,
					       contact, GNUNET_YES))
    return GNUNET_SYSERR;

  if (contact->context)
    GNUNET_MESSENGER_close_room(contact->context->room);

  contact_destroy(contact);
  return GNUNET_OK;
}

void
GNUNET_CHAT_contact_set_name (struct GNUNET_CHAT_Contact *contact,
			      const char *name)
{
  if (!contact)
    return;

  if (contact->context)
    context_set_nick(contact->context, name);
}

const char*
GNUNET_CHAT_contact_get_name (const struct GNUNET_CHAT_Contact *contact)
{
  if (!contact)
    return NULL;

  if (!contact->context)
    goto skip_context;

  const char *nick = context_get_nick(contact->context);

  if (nick)
    return nick;

skip_context:
  return GNUNET_MESSENGER_contact_get_name(contact->contact);
}

struct GNUNET_CHAT_Context*
GNUNET_CHAT_contact_get_context (struct GNUNET_CHAT_Contact *contact)
{
  if (!contact)
    return NULL;

  return contact->context;
}
