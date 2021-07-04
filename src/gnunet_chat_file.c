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

#include "gnunet_chat_lib.h"
#include "gnunet_chat_file.h"

const struct GNUNET_HashCode*
GNUNET_CHAT_file_get_hash (const struct GNUNET_CHAT_File *file)
{
  return &(file->hash);
}

uint64_t
GNUNET_CHAT_file_get_size (const struct GNUNET_CHAT_File *file)
{
  return 0;
}

int
GNUNET_CHAT_file_is_local (const struct GNUNET_CHAT_File *file)
{
  return GNUNET_NO;
}

int
GNUNET_CHAT_file_start_download (struct GNUNET_CHAT_File *file,
				 GNUNET_CHAT_MessageFileDownloadCallback callback,
				 void *cls)
{
  if (!file)
    return GNUNET_SYSERR;

  struct GNUNET_FS_Handle *handle;
  const char *path = ""; // TODO: path = download_directory + filename

  GNUNET_FS_download_start(
      handle,
      file->uri,
      NULL,
      path,
      NULL,
      0,
      0,
      0,
      GNUNET_FS_DOWNLOAD_OPTION_NONE,
      NULL,
      NULL
  );

  return GNUNET_OK;
}

int
GNUNET_CHAT_file_pause_download (struct GNUNET_CHAT_File *file)
{
  if (!file)
    return GNUNET_SYSERR;

  GNUNET_FS_download_suspend(file->context);
  return GNUNET_OK;
}

int
GNUNET_CHAT_file_resume_download (struct GNUNET_CHAT_File *file)
{
  if (!file)
    return GNUNET_SYSERR;

  GNUNET_FS_download_resume(file->context);
  return GNUNET_OK;
}

int
GNUNET_CHAT_file_stop_download (struct GNUNET_CHAT_File *file)
{
  if (!file)
    return GNUNET_SYSERR;

  GNUNET_FS_download_stop(file->context, GNUNET_YES);
  return GNUNET_OK;
}
