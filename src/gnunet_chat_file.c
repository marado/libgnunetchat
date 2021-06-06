/*
 * @author Tobias Frisch
 * @file gnunet_chat_file.c
 */

#include "gnunet_chat_lib.h"
#include "gnunet_chat_file.h"

void
GNUNET_CHAT_file_start_download (struct GNUNET_CHAT_File *file)
{
  if (!file)
    return;

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
}

void
GNUNET_CHAT_file_pause_download (struct GNUNET_CHAT_File *file)
{
  if (!file)
    return;

  GNUNET_FS_download_suspend(file->context);
}

void
GNUNET_CHAT_file_resume_download (struct GNUNET_CHAT_File *file)
{
  if (!file)
    return;

  GNUNET_FS_download_resume(file->context);
}

void
GNUNET_CHAT_file_stop_download (struct GNUNET_CHAT_File *file)
{
  if (!file)
    return;

  GNUNET_FS_download_stop(file, GNUNET_YES);
}
