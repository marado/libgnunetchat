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
 * @file gnunet_chat_message_intern.c
 */

#include <gnunet/gnunet_container_lib.h>

#include "gnunet_chat_context.h"

void
link_message_parent (struct GNUNET_CHAT_Message *message)
{
  struct GNUNET_CHAT_Context *context = message->context;

  struct GNUNET_CHAT_Message *prev = GNUNET_CONTAINER_multihashmap_get(
      context->messages, &(message->msg->header.previous)
  );

  if (prev)
  {
    struct GNUNET_CHAT_MessageList *list = GNUNET_new(
	struct GNUNET_CHAT_MessageList
    );

    list->message = message;

    GNUNET_CONTAINER_DLL_insert(prev->head, prev->tail, list);
  }

  if (GNUNET_MESSENGER_KIND_MERGE != message->msg->header.kind)
    return;

  prev = GNUNET_CONTAINER_multihashmap_get(
      context->messages, &(message->msg->body.merge.previous)
  );

  if (prev)
  {
    struct GNUNET_CHAT_MessageList *list = GNUNET_new(
	struct GNUNET_CHAT_MessageList
    );

    list->message = message;

    GNUNET_CONTAINER_DLL_insert(prev->head, prev->tail, list);
  }
}

void
unlink_message_parent (struct GNUNET_CHAT_Message *message)
{
  struct GNUNET_CHAT_Context *context = message->context;

  struct GNUNET_CHAT_Message *prev = GNUNET_CONTAINER_multihashmap_get(
      context->messages, &(message->msg->header.previous)
  );

  if (prev)
  {
    struct GNUNET_CHAT_MessageList *list = prev->head;

    while (list)
    {
      if (list->message == message)
	break;

      list = list->next;
    }

    if (list)
    {
      GNUNET_CONTAINER_DLL_remove(prev->head, prev->tail, list);
      GNUNET_free(list);
    }
  }

  if (GNUNET_MESSENGER_KIND_MERGE != message->msg->header.kind)
    return;

  prev = GNUNET_CONTAINER_multihashmap_get(
      context->messages, &(message->msg->body.merge.previous)
  );

  if (prev)
  {
    struct GNUNET_CHAT_MessageList *list = prev->head;

    while (list)
    {
      if (list->message == message)
	break;

      list = list->next;
    }

    if (list)
    {
      GNUNET_CONTAINER_DLL_remove(prev->head, prev->tail, list);
      GNUNET_free(list);
    }
  }
}

void
clear_message_children (struct GNUNET_CHAT_Message *message)
{
  struct GNUNET_CHAT_MessageList *list = message->tail;

  while (list)
  {
    GNUNET_CONTAINER_DLL_remove(message->head, message->tail, list);
    GNUNET_free(list);

    list = message->tail;
  }
}
