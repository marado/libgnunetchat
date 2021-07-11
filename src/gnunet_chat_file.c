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
 * @file gnunet_chat_file.c
 */

#include "gnunet_chat_file.h"

#include <limits.h>

struct GNUNET_CHAT_File*
file_create_from_message (struct GNUNET_CHAT_Handle *handle,
			  const struct GNUNET_MESSENGER_MessageFile* message)
{
  struct GNUNET_CHAT_File* file = GNUNET_new(struct GNUNET_CHAT_File);

  file->handle = handle;

  file->name = GNUNET_strndup(message->name, NAME_MAX);

  GNUNET_memcpy(&(file->key), &(message->key), sizeof(file->key));
  GNUNET_memcpy(&(file->hash), &(message->hash), sizeof(file->hash));

  file->published = 0;
  file->downloaded = 0;
  file->unindexed = 0;

  file->meta = GNUNET_CONTAINER_meta_data_create();

  file->uri = GNUNET_FS_uri_parse(message->uri, NULL);
  file->download = NULL;
  file->publish = NULL;
  file->unindex = NULL;

  return file;
}

struct GNUNET_CHAT_File*
file_create_from_disk (struct GNUNET_CHAT_Handle *handle,
		       const char *name, const struct GNUNET_HashCode *hash,
		       const struct GNUNET_CRYPTO_SymmetricSessionKey *key)
{
  struct GNUNET_CHAT_File* file = GNUNET_new(struct GNUNET_CHAT_File);

  file->handle = handle;

  file->name = GNUNET_strndup(name, NAME_MAX);

  GNUNET_memcpy(&(file->key), key, sizeof(file->key));
  GNUNET_memcpy(&(file->hash), hash, sizeof(file->hash));

  file->published = 0;
  file->downloaded = 0;
  file->unindexed = 0;

  file->meta = GNUNET_CONTAINER_meta_data_create();

  file->uri = NULL;
  file->download = NULL;
  file->publish = NULL;
  file->unindex = NULL;

  return file;
}

void
file_destroy (struct GNUNET_CHAT_File* file)
{
  if (file->uri)
    GNUNET_FS_uri_destroy(file->uri);

  if (file->meta)
    GNUNET_CONTAINER_meta_data_destroy(file->meta);

  if (file->name)
    GNUNET_free(file->name);

  GNUNET_free(file);
}
