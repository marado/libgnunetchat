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
