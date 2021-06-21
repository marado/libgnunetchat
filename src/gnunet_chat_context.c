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

#include "gnunet_chat_lib.h"
#include "gnunet_chat_context.h"
#include "gnunet_chat_handle.h"

struct GNUNET_CHAT_Context*
context_create (struct GNUNET_CHAT_Handle *handle,
		enum GNUNET_CHAT_ContextType type,
		const struct GNUNET_HashCode *key)
{
  struct GNUNET_MESSENGER_Handle *messenger = handle->handles.messenger;

  struct GNUNET_HashCode _room_key;
  const struct GNUNET_HashCode *room_key;

  if (key)
    room_key = key;
  else
  {
    GNUNET_CRYPTO_random_block(
    	GNUNET_CRYPTO_QUALITY_WEAK,
    	&_room_key,
    	sizeof(_room_key)
    );

    room_key = &_room_key;
  }

  struct GNUNET_MESSENGER_Room *room = GNUNET_MESSENGER_open_room(
      messenger, room_key
  );

  if (!room)
    return NULL;

  struct GNUNET_CHAT_Context* context = GNUNET_new(struct GNUNET_CHAT_Context);

  context->handle = handle;
  context->room = room;

  context->type = type;
  GNUNET_memcpy(&(context->key), room_key, sizeof(_room_key));
  context->nick = NULL;

  return context;
}

void
context_destroy (struct GNUNET_CHAT_Context* context)
{
  GNUNET_free(context);
}

enum GNUNET_CHAT_ContextType
context_get_type (struct GNUNET_CHAT_Context* context)
{
  return context->type;
}

const struct GNUNET_HashCode*
context_get_key (struct GNUNET_CHAT_Context* context)
{
  return &(context->key);
}

const char*
context_get_nick (struct GNUNET_CHAT_Context* context)
{
  return context->nick;
}

void
context_set_nick (struct GNUNET_CHAT_Context* context,
		  const char *nick)
{
  if (context->nick)
    GNUNET_free(context->nick);

  context->nick = nick? GNUNET_strdup(nick) : NULL;
}

void
GNUNET_CHAT_context_send_text (struct GNUNET_CHAT_Context *context,
			       const char *text)
{
  if (!context)
    return;

  struct GNUNET_MESSENGER_Message message;
  message.header.kind = GNUNET_MESSENGER_KIND_TEXT;
  message.body.text.text = text;

  GNUNET_MESSENGER_send_message(context->room, &message, NULL);
}

void
GNUNET_CHAT_context_send_file (struct GNUNET_CHAT_Context *context,
			       const char *path)
{
  //TODO: generate key, hash file, encrypt file, upload file, share uri & info
}

void
GNUNET_CHAT_context_send_uri (struct GNUNET_CHAT_Context *context,
			      const char *uri)
{
  if (!context)
    return;

  struct GNUNET_MESSENGER_Message message;
  message.header.kind = GNUNET_MESSENGER_KIND_FILE;

  memset(&(message.body.file.key), 0, sizeof(message.body.file.key));

  message.body.file.hash; // TODO: download & hash
  GNUNET_memcpy(message.body.file.name, "", 1); // TODO: cut from uri or get from download?
  message.body.file.uri = uri;

  GNUNET_MESSENGER_send_message(context->room, &message, NULL);
}

void
GNUNET_CHAT_context_share_file (struct GNUNET_CHAT_Context *context,
				const struct GNUNET_CHAT_File *file)
{
  //TODO: send copied message basically
}

void
GNUNET_CHAT_context_delete_message (struct GNUNET_CHAT_Context *context,
				    const struct GNUNET_HashCode *hash,
				    struct GNUNET_TIME_Relative delay)
{
  if (!context)
    return;

  struct GNUNET_MESSENGER_Message message;
  message.header.kind = GNUNET_MESSENGER_KIND_TEXT;
  GNUNET_memcpy(&(message.body.delete.hash), hash, sizeof(*hash));
  message.body.delete.delay = GNUNET_TIME_relative_hton(delay);

  GNUNET_MESSENGER_send_message(context->room, &message, NULL);
}

const struct GNUNET_CHAT_Message*
GNUNET_CHAT_context_get_message (struct GNUNET_CHAT_Context *context,
				 const struct GNUNET_HashCode *hash)
{
  if (!context)
    return NULL;

  struct GNUNET_MESSENGER_Message *message = GNUNET_MESSENGER_get_message(
      context->room, hash
  );

  //TODO: convert messenger-message to chat-message

  return NULL;
}

int
GNUNET_CHAT_context_iterate_messages (struct GNUNET_CHAT_Context *context,
				      GNUNET_CHAT_ContextMessageCallback callback,
				      void *cls)
{
  return GNUNET_SYSERR;
}
