/*
 * @author Tobias Frisch
 * @file gnunet_chat_handle.h
 */

#ifndef GNUNET_CHAT_HANDLE_H_
#define GNUNET_CHAT_HANDLE_H_

#include <gnunet/platform.h>
#include <gnunet/gnunet_config.h>
#include <gnunet/gnunet_container_lib.h>
#include <gnunet/gnunet_arm_service.h>
#include <gnunet/gnunet_fs_service.h>
#include <gnunet/gnunet_identity_service.h>
#include <gnunet/gnunet_messenger_service.h>
#include <gnunet/gnunet_regex_service.h>
#include <gnunet/gnunet_util_lib.h>

struct GNUNET_CHAT_Handle
{
  const struct GNUNET_CONFIGURATION_Handle* cfg;

  struct {
    struct GNUNET_ARM_Handle* arm;
    struct GNUNET_FS_Handle* fs;
    struct GNUNET_MESSENGER_Handle* messenger;
  } handles;

  struct GNUNET_CONTAINER_MultiHashMap *contacts;
  struct GNUNET_CONTAINER_MultiHashMap *groups;
};

#endif /* GNUNET_CHAT_HANDLE_H_ */
