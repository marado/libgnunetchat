/*
 * @author Tobias Frisch
 * @file gnunet_chat_contact.h
 */

#ifndef GNUNET_CHAT_CONTACT_H_
#define GNUNET_CHAT_CONTACT_H_

#include "gnunet_chat_context.h"

struct GNUNET_CHAT_Contact
{
  struct GNUNET_CHAT_Handle *handle;
  struct GNUNET_CHAT_Context *context;

  struct GNUNET_MESSENGER_Contact *contact;

  char *nick;
};

struct GNUNET_CHAT_Contact*
contact_create (struct GNUNET_CHAT_Handle *handle);

void
contact_destroy (struct GNUNET_CHAT_Contact *contact);

#endif /* GNUNET_CHAT_CONTACT_H_ */
