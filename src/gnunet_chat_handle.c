/*
 * @author Tobias Frisch
 * @file gnunet_chat_handle.c
 */

#include "gnunet_chat_lib.h"
#include "gnunet_chat_handle.h"
#include "gnunet_chat_group.h"

static void handle_arm_connection(void* cls, int connected) {
  struct GNUNET_CHAT_Handle *handle = cls;

  if (GNUNET_YES == connected) {
    GNUNET_ARM_request_service_start(handle->handles.arm, "messenger", GNUNET_OS_INHERIT_STD_NONE, NULL, NULL);
    GNUNET_ARM_request_service_start(handle->handles.arm, "fs", GNUNET_OS_INHERIT_STD_NONE, NULL, NULL);
  } else {
    GNUNET_ARM_request_service_start(handle->handles.arm, "arm", GNUNET_OS_INHERIT_STD_NONE, NULL, NULL);
  }
}


struct GNUNET_CHAT_Handle*
GNUNET_CHAT_start (const struct GNUNET_CONFIGURATION_Handle* cfg,
		     const char *name) {
  if (!cfg)
    return NULL;

  struct GNUNET_CHAT_Handle *handle = GNUNET_new(struct GNUNET_CHAT_Handle);
  memset(handle, 0, sizeof(*handle));
  handle->cfg = cfg;

  handle->handles.arm = GNUNET_ARM_connect(cfg, &handle_arm_connection, handle);

  if (handle->handles.arm)
    handle_arm_connection(handle, GNUNET_NO);

  handle->handles.messenger = GNUNET_MESSENGER_connect(
      cfg, name, NULL, NULL, NULL, NULL //TODO
  );

  if (!handle->handles.messenger)
  {
    GNUNET_CHAT_stop(handle);
    return NULL;
  }

  handle->handles.fs = GNUNET_FS_start(
      cfg,
      NULL, //TODO
      NULL,
      GNUNET_FS_FLAGS_NONE,
      GNUNET_FS_OPTIONS_END
  );

  handle->contacts = GNUNET_CONTAINER_multihashmap_create(8, GNUNET_NO);
  handle->groups = GNUNET_CONTAINER_multihashmap_create(8, GNUNET_NO);

  return handle;
}

void
GNUNET_CHAT_stop (struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return;

  if (handle->handles.fs)
   {
     // TODO: stop each action

     GNUNET_FS_stop(handle->handles.fs);
     handle->handles.fs = NULL;
   }

   if (handle->handles.messenger)
   {
     // TODO: stop everything related

     GNUNET_MESSENGER_disconnect(handle->handles.messenger);
     handle->handles.messenger = NULL;
   }

  if (handle->groups)
  {
    // TODO: destroy each

    GNUNET_CONTAINER_multihashmap_destroy(handle->groups);
    handle->groups = NULL;
  }

  if (handle->contacts)
  {
    // TODO: destroy each

    GNUNET_CONTAINER_multihashmap_destroy(handle->contacts);
    handle->contacts = NULL;
  }

  if (handle->handles.arm)
  {
    //TODO: stop started services?

    GNUNET_ARM_disconnect(handle->handles.arm);
    handle->handles.arm = NULL;
  }

  GNUNET_free(handle);
}

int
GNUNET_CHAT_update (struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return GNUNET_SYSERR;

  return GNUNET_MESSENGER_update(handle->handles.messenger);
}

int
GNUNET_CHAT_set_name (struct GNUNET_CHAT_Handle *handle,
		      const char *name)
{
  if (!handle)
    return GNUNET_SYSERR;

  return GNUNET_MESSENGER_set_name(handle, name);
}

const char*
GNUNET_CHAT_get_name (const struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return GNUNET_SYSERR;

  return GNUNET_MESSENGER_get_name(handle->handles.messenger);
}

const struct GNUNET_IDENTITY_PublicKey*
GNUNET_CHAT_get_key (const struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return GNUNET_SYSERR;

  return GNUNET_MESSENGER_get_key(handle->handles.messenger);
}

int
GNUNET_CHAT_iterate_contacts (struct GNUNET_CHAT_Handle *handle,
			      GNUNET_CHAT_ContactCallback callback,
			      void *cls)
{
  return GNUNET_SYSERR;
}

struct GNUNET_CHAT_Group*
GNUNET_CHAT_group_create (struct GNUNET_CHAT_Handle *handle)
{
  if (!handle)
    return NULL;

  return group_create(handle);
}

int
GNUNET_CHAT_iterate_groups (struct GNUNET_CHAT_Handle *handle,
			    GNUNET_CHAT_GroupCallback callback,
			    void *cls)
{
  return GNUNET_SYSERR;
}
