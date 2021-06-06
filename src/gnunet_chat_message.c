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
 * @file gnunet_chat_message.c
 */

#include "gnunet_chat_lib.h"
#include "gnunet_chat_message.h"

enum GNUNET_CHAT_MessageKind
GNUNET_CHAT_message_get_kind (const struct GNUNET_CHAT_Message *message)
{
  if (!message)
    return GNUNET_CHAT_KIND_UNKNOWN;

  switch (message->message->header.kind) {
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

  return GNUNET_TIME_absolute_ntoh(message->message->header.timestamp);
}

const struct GNUNET_CHAT_Contact*
GNUNET_CHAT_message_get_sender (const struct GNUNET_CHAT_Message *message)
{
  return NULL;
}

int
GNUNET_CHAT_message_get_read_receipt (const struct GNUNET_CHAT_Message *message,
				      GNUNET_CHAT_MessageReadReceiptCallback callback,
				      void *cls)
{
  return GNUNET_SYSERR;
}

const char*
GNUNET_CHAT_message_get_text (const struct GNUNET_CHAT_Message *message)
{
  if (!message)
    return NULL;

  if (GNUNET_MESSENGER_KIND_TEXT != message->message->header.kind)
    return NULL;

  return message->message->body.text.text;
}

struct GNUNET_CHAT_File*
GNUNET_CHAT_message_get_file (const struct GNUNET_CHAT_Message *message)
{
  return NULL;
}
