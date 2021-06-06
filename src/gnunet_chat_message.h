/*
 * @author Tobias Frisch
 * @file gnunet_chat_message.h
 */

#ifndef GNUNET_CHAT_MESSAGE_H_
#define GNUNET_CHAT_MESSAGE_H_

#include <gnunet/platform.h>
#include <gnunet/gnunet_messenger_service.h>
#include <gnunet/gnunet_util_lib.h>

struct GNUNET_CHAT_Message
{
  struct GNUNET_MESSENGER_Message *message;
};

#endif /* GNUNET_CHAT_MESSAGE_H_ */
