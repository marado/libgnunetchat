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
 * @file gnunet_chat_context.h
 */

#ifndef GNUNET_CHAT_CONTEXT_H_
#define GNUNET_CHAT_CONTEXT_H_

#include <gnunet/platform.h>
#include <gnunet/gnunet_messenger_service.h>
#include <gnunet/gnunet_util_lib.h>

enum GNUNET_CHAT_ContextType
{
  GNUNET_CHAT_CONTEXT_TYPE_CONTACT = 1,
  GNUNET_CHAT_CONTEXT_TYPE_GROUP   = 2,
  GNUNET_CHAT_CONTEXT_TYPE_UNKNOWN = 0
};

struct GNUNET_CHAT_Handle;

struct GNUNET_CHAT_Context
{
  struct GNUNET_CHAT_Handle *handle;
  struct GNUNET_MESSENGER_Room *room;

  enum GNUNET_CHAT_ContextType type;
  struct GNUNET_HashCode key;
  char *nick;
};

struct GNUNET_CHAT_Context*
context_create (struct GNUNET_CHAT_Handle *handle,
		enum GNUNET_CHAT_ContextType type,
		const struct GNUNET_HashCode *key);

void
context_destroy (struct GNUNET_CHAT_Context* context);

enum GNUNET_CHAT_ContextType
context_get_type (struct GNUNET_CHAT_Context* context);

const struct GNUNET_HashCode*
context_get_key (struct GNUNET_CHAT_Context* context);

const char*
context_get_nick (struct GNUNET_CHAT_Context* context);

void
context_set_nick (struct GNUNET_CHAT_Context* context,
		  const char *nick);

#endif /* GNUNET_CHAT_CONTEXT_H_ */
