/*
 * @author Tobias Frisch
 * @file gnunet_chat_file.h
 */

#ifndef GNUNET_CHAT_FILE_H_
#define GNUNET_CHAT_FILE_H_

#include <gnunet/platform.h>
#include <gnunet/gnunet_fs_service.h>
#include <gnunet/gnunet_util_lib.h>

struct GNUNET_CHAT_File
{
  struct GNUNET_FS_Uri* uri;
  struct GNUNET_FS_DownloadContext* context;
};

#endif /* GNUNET_CHAT_FILE_H_ */
