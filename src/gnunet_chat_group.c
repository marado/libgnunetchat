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

#include "gnunet_chat_group.h"

#include "gnunet_chat_group_intern.c"

struct GNUNET_CHAT_Group*
group_create_from_context (struct GNUNET_CHAT_Handle *handle,
			   struct GNUNET_CHAT_Context *context)
{
  struct GNUNET_CHAT_Group* group = GNUNET_new(struct GNUNET_CHAT_Group);

  group->handle = handle;
  group->context = context;

  group->topic = NULL;

  group->announcement = NULL;
  group->search = NULL;

  return group;
}

void
group_destroy (struct GNUNET_CHAT_Group* group)
{
  if (group->search)
    GNUNET_REGEX_search_cancel(group->search);

  if (group->announcement)
    GNUNET_REGEX_announce_cancel(group->announcement);

  if (group->topic)
    GNUNET_free(group->topic);

  GNUNET_free(group);
}

void
group_publish (struct GNUNET_CHAT_Group* group)
{
  group->announcement = GNUNET_REGEX_announce(
      group->handle->cfg, group->topic, // TODO: raw topic?
      GNUNET_TIME_relative_get_minute_(), // TODO: configure delay?
      1 // TODO: no compression?
  );

  group->search = GNUNET_REGEX_search(
      group->handle->cfg, group->topic, // TODO: raw topic?
      search_group_by_topic, group
  );
}
