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
 * @file gnunet_chat_message.h
 */

#ifndef GNUNET_CHAT_MESSAGE_H_
#define GNUNET_CHAT_MESSAGE_H_

#include <gnunet/platform.h>
#include <gnunet/gnunet_common.h>
#include <gnunet/gnunet_messenger_service.h>
#include <gnunet/gnunet_util_lib.h>

struct GNUNET_CHAT_Context;

struct GNUNET_CHAT_Message
{
  struct GNUNET_CHAT_Context *context;

  struct GNUNET_HashCode hash;

  const struct GNUNET_MESSENGER_Message *msg;
};

struct GNUNET_CHAT_Message*
message_create_from_msg (struct GNUNET_CHAT_Context *context,
			 const struct GNUNET_HashCode *hash,
			 const struct GNUNET_MESSENGER_Message *msg);

void
message_destroy (struct GNUNET_CHAT_Message* message);

#endif /* GNUNET_CHAT_MESSAGE_H_ */
