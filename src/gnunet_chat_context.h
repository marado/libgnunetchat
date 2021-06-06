/*
 * @author Tobias Frisch
 * @file gnunet_chat_context.h
 */

#ifndef GNUNET_CHAT_CONTEXT_H_
#define GNUNET_CHAT_CONTEXT_H_

#include <gnunet/platform.h>
#include <gnunet/gnunet_messenger_service.h>
#include <gnunet/gnunet_util_lib.h>

struct GNUNET_CHAT_Handle;

struct GNUNET_CHAT_Context
{
  struct GNUNET_CHAT_Handle *handle;
  struct GNUNET_MESSENGER_Room *room;

  struct GNUNET_HashCode key;
};

struct GNUNET_CHAT_Context*
context_create (struct GNUNET_CHAT_Handle *handle,
		const struct GNUNET_HashCode *key);

void
context_destroy (struct GNUNET_CHAT_Context* context);

const struct GNUNET_HashCode*
context_get_key (struct GNUNET_CHAT_Context* context);

#endif /* GNUNET_CHAT_CONTEXT_H_ */
