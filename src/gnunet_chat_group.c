/*
 * @author Tobias Frisch
 * @file gnunet_chat_group.c
 */

#include "gnunet_chat_lib.h"
#include "gnunet_chat_group.h"
#include "gnunet_chat_handle.h"

struct GNUNET_CHAT_Group*
group_create(struct GNUNET_CHAT_Handle *handle)
{
  struct GNUNET_CHAT_Group *group = GNUNET_new(struct GNUNET_CHAT_Group);

  group->handle = handle;
  group->context = context_create(handle, NULL);
  group->name = NULL;

  if (!group->context)
  {
    group_destroy (group);
    return NULL;
  }

  const struct GNUNET_HashCode *key = context_get_key(group->context);

  const int result = GNUNET_CONTAINER_multihashmap_put(
      handle->groups,
      key,
      group,
      GNUNET_CONTAINER_MULTIHASHMAPOPTION_UNIQUE_FAST
  );

  if (GNUNET_OK != result) {
      group_destroy(group);
      return NULL;
  }

  return group;
}

void
group_destroy(struct GNUNET_CHAT_Group* group)
{
  if (!group->context)
    goto skip_context;

  struct GNUNET_CHAT_Handle *handle = group->handle;
  const struct GNUNET_HashCode *key = context_get_key(group->context);

  GNUNET_CONTAINER_multihashmap_remove(
      handle->groups,
      key,
      group
  );

  context_destroy(group->context);

skip_context:
  if (group->name)
    GNUNET_free(group->name);

  GNUNET_free(group);
}

void
GNUNET_CHAT_group_leave (struct GNUNET_CHAT_Group *group)
{
  if (!group)
    return;

  if (group->context)
    GNUNET_MESSENGER_close_room(group->context->room);

  group_destroy(group);
}

void
GNUNET_CHAT_group_set_name (struct GNUNET_CHAT_Group *group,
			    const char *name)
{
  if (!group)
    return;

  if (group->name)
    GNUNET_free(group->name);

  group->name = name? GNUNET_strdup(name) : NULL;
}

const char*
GNUNET_CHAT_group_get_name (const struct GNUNET_CHAT_Group *group)
{
  if (!group)
    return NULL;

  return group->name;
}

void
GNUNET_CHAT_group_invite_contact (struct GNUNET_CHAT_Group *group,
				  struct GNUNET_CHAT_Contact *contact)
{
  if ((!group) || (!contact))
    return;

  struct GNUNET_CHAT_Context *context = GNUNET_CHAT_contact_get_context(contact);

  if (!context)
    return;

  struct GNUNET_MESSENGER_Handle *messenger = group->handle->handles.messenger;
  GNUNET_MESSENGER_open_room(messenger, context_get_key(group->context));

  struct GNUNET_MESSENGER_Message message;
  message.header.kind = GNUNET_MESSENGER_KIND_INVITE;

  int result = GNUNET_CRYPTO_get_peer_identity(
      group->handle->cfg,
      &(message.body.invite.door)
  );

  if (GNUNET_OK != result)
    return;

  GNUNET_memcpy(
      &(message.body.invite.key),
      context_get_key(group->context),
      sizeof(message.body.invite.key)
  );

  GNUNET_MESSENGER_send_message(context->room, &message, NULL);
}

struct GNUNET_CHAT_GroupIterateContacts
{
  struct GNUNET_CHAT_Group *group;
  GNUNET_CHAT_GroupContactCallback callback;
  void *cls;
};

static int
group_iterate_members (void* cls, struct GNUNET_MESSENGER_Room *room,
                       const struct GNUNET_MESSENGER_Contact *contact)
{
  struct GNUNET_CHAT_GroupIterateContacts *iterate = cls;

  return iterate->callback(iterate->cls, iterate->group, NULL); // TODO
}

int
GNUNET_CHAT_group_iterate_contacts (struct GNUNET_CHAT_Group *group,
				    GNUNET_CHAT_GroupContactCallback callback,
				    void *cls)
{
  if (!group)
    return GNUNET_SYSERR;

  if (!callback)
    return GNUNET_MESSENGER_iterate_members(group->context->room, NULL, cls);

  struct GNUNET_CHAT_GroupIterateContacts iterate;
  iterate.group = group;
  iterate.callback = callback;
  iterate.cls = cls;

  return GNUNET_MESSENGER_iterate_members(
      group->context->room, group_iterate_members, &iterate
  );
}

struct GNUNET_CHAT_Context*
GNUNET_CHAT_group_get_context (struct GNUNET_CHAT_Group *group)
{
  if (!group)
    return NULL;

  return group->context;
}
