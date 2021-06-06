/*
 * @author Tobias Frisch
 * @file gnunet_chat_group.h
 */

#ifndef GNUNET_CHAT_GROUP_H_
#define GNUNET_CHAT_GROUP_H_

#include "gnunet_chat_context.h"

struct GNUNET_CHAT_Group
{
  struct GNUNET_CHAT_Handle *handle;
  struct GNUNET_CHAT_Context *context;

  char *name;
};

struct GNUNET_CHAT_Group*
group_create(struct GNUNET_CHAT_Handle *handle);

void
group_destroy(struct GNUNET_CHAT_Group* group);

#endif /* GNUNET_CHAT_GROUP_H_ */
