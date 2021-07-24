/*
   This file is part of GNUnet.
   Copyright (C) 2021 GNUnet e.V.

   GNUnet is free software: you can redistribute it and/or modify it
   under the terms of the GNU Affero General Public License as published
   by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   GNUnet is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   SPDX-License-Identifier: AGPL3.0-or-later
 */
/*
   The development of this code was supported by the NLnet foundation as part
   of the NGI Assure initative to have a more free and secure internet.
 */
/*
 * @author Tobias Frisch
 * @file gnunet_chat_lib.h
 */

#ifndef GNUNET_CHAT_LIB_H_
#define GNUNET_CHAT_LIB_H_

#include <gnunet/platform.h>
#include <gnunet/gnunet_common.h>
#include <gnunet/gnunet_util_lib.h>

/**
 * TODO
 */
enum GNUNET_CHAT_MessageKind
{
  /**
   * TODO
   */
  GNUNET_CHAT_KIND_JOIN = 1,       /**< GNUNET_CHAT_KIND_JOIN */

  /**
   * TODO
   */
  GNUNET_CHAT_KIND_LEAVE = 2,      /**< GNUNET_CHAT_KIND_LEAVE */

  /**
   * TODO
   */
  GNUNET_CHAT_KIND_CONTACT = 3,    /**< GNUNET_CHAT_KIND_CONTACT */

  /**
   * TODO
   */
  GNUNET_CHAT_KIND_INVITATION = 4, /**< GNUNET_CHAT_KIND_INVITATION */

  /**
   * TODO
   */
  GNUNET_CHAT_KIND_TEXT = 5,       /**< GNUNET_CHAT_KIND_TEXT */

  /**
   * TODO
   */
  GNUNET_CHAT_KIND_FILE = 6,       /**< GNUNET_CHAT_KIND_FILE */

  /**
   * TODO
   */
  GNUNET_CHAT_KIND_DELETION = 7,   /**< GNUNET_CHAT_KIND_DELETION */

  /**
   * TODO
   */
  GNUNET_CHAT_KIND_UNKNOWN = 0     /**< GNUNET_CHAT_KIND_UNKNOWN */
};

/**
 * TODO
 */
struct GNUNET_CHAT_Handle;

/**
 * TODO
 */
struct GNUNET_CHAT_Contact;

/**
 * TODO
 */
struct GNUNET_CHAT_Group;

/**
 * TODO
 */
struct GNUNET_CHAT_Context;

/**
 * TODO
 */
struct GNUNET_CHAT_Message;

/**
 * TODO
 */
struct GNUNET_CHAT_File;

/**
 * TODO
 */
struct GNUNET_CHAT_Invitation;

/**
 * TODO
 *
 * @param cls
 * @param handle
 * @param context
 * @param reason
 */
typedef void
(*GNUNET_CHAT_WarningCallback) (void *cls, struct GNUNET_CHAT_Handle *handle,
				struct GNUNET_CHAT_Context *context, int reason);

/**
 * TODO
 *
 * @param cls
 * @param handle
 * @param contact
 * @return
 */
typedef int
(*GNUNET_CHAT_ContactCallback) (void *cls, struct GNUNET_CHAT_Handle *handle,
				struct GNUNET_CHAT_Contact *contact);

/**
 * TODO
 *
 * @param cls
 * @param handle
 * @param group
 * @return
 */
typedef int
(*GNUNET_CHAT_GroupCallback) (void *cls, struct GNUNET_CHAT_Handle *handle,
			      struct GNUNET_CHAT_Group *group);

/**
 * TODO
 *
 * @param cls
 * @param group
 * @param contact
 * @return
 */
typedef int
(*GNUNET_CHAT_GroupContactCallback) (void *cls, struct GNUNET_CHAT_Group *group,
                                    struct GNUNET_CHAT_Contact *contact);

/**
 * TODO
 *
 * @param cls
 * @param context
 * @param message
 * @return
 */
typedef int
(*GNUNET_CHAT_ContextMessageCallback) (void *cls, struct GNUNET_CHAT_Context *context,
				       struct GNUNET_CHAT_Message *message);

/**
 * TODO
 *
 * @param cls
 * @param message
 * @param contact
 * @param read_receipt
 * @return
 */
typedef int
(*GNUNET_CHAT_MessageReadReceiptCallback) (void *cls, struct GNUNET_CHAT_Message *message,
					   struct GNUNET_CHAT_Contact *contact,
					   int read_receipt);

/**
 * TODO
 *
 * @param cls
 * @param file
 * @param completed
 */
typedef int
(*GNUNET_CHAT_MessageFileUploadCallback) (void *cls, const struct GNUNET_CHAT_File *file,
					  uint64_t completed);

/**
 * TODO
 *
 * @param cls
 * @param file
 * @param completed
 */
typedef int
(*GNUNET_CHAT_MessageFileDownloadCallback) (void *cls, struct GNUNET_CHAT_File *file,
					    uint64_t completed);

/**
 * TODO
 *
 * @param cfg
 * @param directory
 * @param name
 * @param warn_cb
 * @param warn_cls
 * @return
 */
struct GNUNET_CHAT_Handle*
GNUNET_CHAT_start (const struct GNUNET_CONFIGURATION_Handle *cfg,
		   const char *directory,
		   const char *name,
		   GNUNET_CHAT_WarningCallback warn_cb, void *warn_cls,
		   GNUNET_CHAT_ContextMessageCallback msg_cb, void *msg_cls);

/**
 * TODO
 *
 * @param handle
 */
void
GNUNET_CHAT_stop (struct GNUNET_CHAT_Handle *handle);

/**
 * TODO
 *
 * @param handle
 * @return
 */
int
GNUNET_CHAT_update (struct GNUNET_CHAT_Handle *handle);

/**
 * TODO
 *
 * @param handle
 * @param name
 * @return
 */
int
GNUNET_CHAT_set_name (struct GNUNET_CHAT_Handle *handle,
		      const char *name);

/**
 * TODO
 *
 * @param handle
 * @return
 */
const char*
GNUNET_CHAT_get_name (const struct GNUNET_CHAT_Handle *handle);

/**
 * TODO
 *
 * @param handle
 * @return
 */
const struct GNUNET_IDENTITY_PublicKey*
GNUNET_CHAT_get_key (const struct GNUNET_CHAT_Handle *handle);

/**
 * TODO
 *
 * @param handle
 * @param callback
 * @param cls
 * @return
 */
int
GNUNET_CHAT_iterate_contacts (struct GNUNET_CHAT_Handle *handle,
			      GNUNET_CHAT_ContactCallback callback,
			      void *cls);

/**
 * TODO
 *
 * @param handle
 * @return
 */
struct GNUNET_CHAT_Group *
GNUNET_CHAT_group_create (struct GNUNET_CHAT_Handle *handle,
			  const char* topic);

/**
 * TODO
 *
 * @param handle
 * @param callback
 * @param cls
 * @return
 */
int
GNUNET_CHAT_iterate_groups (struct GNUNET_CHAT_Handle *handle,
			    GNUNET_CHAT_GroupCallback callback,
			    void *cls);

/**
 * TODO
 *
 * @param contact
 * @return
 */
int
GNUNET_CHAT_contact_delete (struct GNUNET_CHAT_Contact *contact);

/**
 * TODO
 *
 * @param contact Contact
 * @param name Custom nick name
 */
void
GNUNET_CHAT_contact_set_name (struct GNUNET_CHAT_Contact *contact,
			      const char *name);

/**
 * TODO
 *
 * @param contact Contact
 * @return Name or custom nick name
 */
const char*
GNUNET_CHAT_contact_get_name (const struct GNUNET_CHAT_Contact *contact);

/**
 * TODO
 *
 * @param contact
 * @return
 */
const struct GNUNET_IDENTITY_PublicKey*
GNUNET_CHAT_contact_get_key (const struct GNUNET_CHAT_Contact *contact);

/**
 * TODO
 *
 * @param contact Contact
 * @return Chat context
 */
struct GNUNET_CHAT_Context*
GNUNET_CHAT_contact_get_context (struct GNUNET_CHAT_Contact *contact);

/**
 * TODO
 *
 * @param contact
 * @param user_pointer
 * @return
 */
void
GNUNET_CHAT_contact_set_user_pointer (struct GNUNET_CHAT_Contact *contact,
				      void *user_pointer);

/**
 * TODO
 *
 * @param contact
 * @return
 */
void*
GNUNET_CHAT_contact_get_user_pointer (struct GNUNET_CHAT_Contact *contact);

/**
 * TODO
 *
 * @param group
 * @return
 */
int
GNUNET_CHAT_group_leave (struct GNUNET_CHAT_Group *group);

/**
 * TODO
 *
 * @param group
 * @param name
 */
void
GNUNET_CHAT_group_set_name (struct GNUNET_CHAT_Group *group,
			    const char *name);

/**
 * TODO
 *
 * @param group
 * @return
 */
const char*
GNUNET_CHAT_group_get_name (const struct GNUNET_CHAT_Group *group);

/**
 * TODO
 *
 * @param group
 * @param user_pointer
 */
void
GNUNET_CHAT_group_set_user_pointer (struct GNUNET_CHAT_Group *group,
				    void *user_pointer);

/**
 * TODO
 *
 * @param group
 */
void*
GNUNET_CHAT_group_get_user_pointer (struct GNUNET_CHAT_Group *group);

/**
 * TODO
 *
 * @param group
 * @param contact
 */
void
GNUNET_CHAT_group_invite_contact (struct GNUNET_CHAT_Group *group,
				  struct GNUNET_CHAT_Contact *contact);

/**
 * TODO
 *
 * @param group
 * @param callback
 * @param cls
 * @return
 */
int
GNUNET_CHAT_group_iterate_contacts (struct GNUNET_CHAT_Group *group,
				    GNUNET_CHAT_GroupContactCallback callback,
				    void *cls);

/**
 * TODO
 *
 * @param group
 * @return
 */
struct GNUNET_CHAT_Context*
GNUNET_CHAT_group_get_context (struct GNUNET_CHAT_Group *group);

/**
 * TODO
 *
 * @param context
 * @param user_pointer
 */
void
GNUNET_CHAT_context_set_user_pointer (struct GNUNET_CHAT_Context *context,
				      void *user_pointer);

/**
 * TODO
 *
 * @param context
 */
void*
GNUNET_CHAT_context_get_user_pointer (const struct GNUNET_CHAT_Context *context);

/**
 * TODO
 *
 * @param context
 * @param text
 * @return
 */
int
GNUNET_CHAT_context_send_text (struct GNUNET_CHAT_Context *context,
			       const char *text);

/**
 * TODO
 *
 * @param context
 * @param path
 * @return
 */
int
GNUNET_CHAT_context_send_file (struct GNUNET_CHAT_Context *context,
			       const char *path);

/**
 * TODO
 *
 * @param context
 * @param uri
 * @return
 */
int
GNUNET_CHAT_context_send_uri (struct GNUNET_CHAT_Context *context,
			      const char *uri);

/**
 * TODO
 *
 * @param context
 * @param file
 * @return
 */
int
GNUNET_CHAT_context_share_file (struct GNUNET_CHAT_Context *context,
				const struct GNUNET_CHAT_File *file);

/**
 * TODO
 *
 * @param context
 * @param callback
 * @param cls
 * @return
 */
int
GNUNET_CHAT_context_iterate_messages (struct GNUNET_CHAT_Context *context,
				      GNUNET_CHAT_ContextMessageCallback callback,
				      void *cls);

/**
 * TODO
 *
 * @param message
 * @return
 */
enum GNUNET_CHAT_MessageKind
GNUNET_CHAT_message_get_kind (const struct GNUNET_CHAT_Message *message);

/**
 * TODO
 *
 * @param message
 * @return
 */
struct GNUNET_TIME_Absolute
GNUNET_CHAT_message_get_timestamp (const struct GNUNET_CHAT_Message *message);

/**
 * TODO
 *
 * @param message
 * @return
 */
const struct GNUNET_CHAT_Contact*
GNUNET_CHAT_message_get_sender (const struct GNUNET_CHAT_Message *message);

/**
 * TODO
 *
 * @param message
 * @return
 */
int
GNUNET_CHAT_message_is_sent (const struct GNUNET_CHAT_Message *message);

/**
 * TODO
 *
 * @param message
 * @return
 */
int
GNUNET_CHAT_message_is_private (const struct GNUNET_CHAT_Message *message);

/**
 * TODO
 *
 * @param message
 * @param callback
 * @param cls
 * @return
 */
int
GNUNET_CHAT_message_get_read_receipt (const struct GNUNET_CHAT_Message *message,
				      GNUNET_CHAT_MessageReadReceiptCallback callback,
				      void *cls);

/**
 * TODO
 *
 * @param message
 * @return
 */
const char*
GNUNET_CHAT_message_get_text (const struct GNUNET_CHAT_Message *message);


/**
 * TODO
 *
 * @param message
 * @return
 */
struct GNUNET_CHAT_File*
GNUNET_CHAT_message_get_file (const struct GNUNET_CHAT_Message *message);

/**
 * TODO
 *
 * @param message
 * @return
 */
struct GNUNET_CHAT_Invitation*
GNUNET_CHAT_message_get_invitation (const struct GNUNET_CHAT_Message *message);

/**
 * TODO
 *
 * @param message
 * @param delay
 * @return
 */
int
GNUNET_CHAT_message_delete (const struct GNUNET_CHAT_Message *message,
			    struct GNUNET_TIME_Relative delay);

/**
 * TODO
 *
 * @param file
 * @return
 */
const struct GNUNET_HashCode*
GNUNET_CHAT_file_get_hash (const struct GNUNET_CHAT_File *file);

/**
 * TODO
 *
 * @param file
 * @return
 */
uint64_t
GNUNET_CHAT_file_get_size (const struct GNUNET_CHAT_File *file);

/**
 * TODO
 *
 * @param file
 * @return
 */
int
GNUNET_CHAT_file_is_local (const struct GNUNET_CHAT_File *file);

/**
 * TODO
 *
 * @param file
 * @return
 */
int
GNUNET_CHAT_file_start_download (struct GNUNET_CHAT_File *file,
				 GNUNET_CHAT_MessageFileDownloadCallback callback,
				 void *cls);

/**
 * TODO
 *
 * @param file
 * @return
 */
int
GNUNET_CHAT_file_pause_download (struct GNUNET_CHAT_File *file);

/**
 * TODO
 *
 * @param file
 * @return
 */
int
GNUNET_CHAT_file_resume_download (struct GNUNET_CHAT_File *file);

/**
 * TODO
 *
 * @param file
 * @return
 */
int
GNUNET_CHAT_file_stop_download (struct GNUNET_CHAT_File *file);

/**
 * TODO
 *
 * @param file
 * @return
 */
int
GNUNET_CHAT_file_unindex (struct GNUNET_CHAT_File *file);

/**
 * TODO
 *
 * @param invitation
 */
void
GNUNET_CHAT_invitation_accept (struct GNUNET_CHAT_Invitation *invitation);

#endif /* GNUNET_CHAT_LIB_H_ */
