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
contact_create (struct GNUNET_CHAT_Handle *handle)
{
  struct GNUNET_CHAT_Contact* contact = GNUNET_new(struct GNUNET_CHAT_Contact);

  contact->handle = handle;
  contact->context = context_create(handle, NULL); // TODO: check for existing context?

  contact->contact = NULL;
  contact->nick = NULL;

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
  if (contact->nick)
    GNUNET_free(contact->nick);
}

int
GNUNET_CHAT_contact_delete (struct GNUNET_CHAT_Contact *contact)
{
  if (!contact)
    return GNUNET_SYSERR;

  if (contact->context)
    GNUNET_MESSENGER_close_room(contact->context->room);

  contact_destroy(contact);
  return GNUNET_OK;
}

void
GNUNET_CHAT_contact_set_blocking (struct GNUNET_CHAT_Contact *contact,
				  int blocking)
{
  //TODO
}

int
GNUNET_CHAT_contact_is_blocking (const struct GNUNET_CHAT_Contact *contact)
{
  return GNUNET_NO;
}

void
GNUNET_CHAT_contact_set_name (struct GNUNET_CHAT_Contact *contact,
			      const char *name)
{
  if (!contact)
    return;

  if (contact->nick)
    GNUNET_free(contact->nick);

  contact->nick = name? GNUNET_strdup(name) : NULL;
}

const char*
GNUNET_CHAT_contact_get_name (const struct GNUNET_CHAT_Contact *contact)
{
  if (!contact)
    return NULL;

  if (contact->nick)
    return contact->nick;

  return GNUNET_MESSENGER_contact_get_name(contact->contact);
}

struct GNUNET_CHAT_Context*
GNUNET_CHAT_contact_get_context (struct GNUNET_CHAT_Contact *contact)
{
  if (!contact)
    return NULL;

  return contact->context;
}
