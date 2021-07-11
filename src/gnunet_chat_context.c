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
 * @file gnunet_chat_context.c
 */

#include "gnunet_chat_context.h"

#include "gnunet_chat_context_intern.c"

struct GNUNET_CHAT_Context*
context_create_from_room (struct GNUNET_CHAT_Handle *handle,
			  struct GNUNET_MESSENGER_Room *room)
{
  struct GNUNET_CHAT_Context* context = GNUNET_new(struct GNUNET_CHAT_Context);

  context->handle = handle;

  context->type = GNUNET_CHAT_CONTEXT_TYPE_UNKNOWN;
  context->nick = NULL;

  context->messages = GNUNET_CONTAINER_multihashmap_create(8, GNUNET_NO);
  context->invites = GNUNET_CONTAINER_multihashmap_create(8, GNUNET_NO);
  context->files = GNUNET_CONTAINER_multihashmap_create(8, GNUNET_NO);

  context->room = room;

  return context;
}

void
context_destroy (struct GNUNET_CHAT_Context* context)
{
  GNUNET_CONTAINER_multihashmap_iterate(
      context->messages, it_destroy_context_messages, NULL
  );

  GNUNET_CONTAINER_multihashmap_iterate(
      context->messages, it_destroy_context_invites, NULL
  );

  GNUNET_CONTAINER_multihashmap_destroy(context->messages);
  GNUNET_CONTAINER_multihashmap_destroy(context->invites);
  GNUNET_CONTAINER_multihashmap_destroy(context->files);

  if (context->nick)
    GNUNET_free(context->nick);

  GNUNET_free(context);
}
