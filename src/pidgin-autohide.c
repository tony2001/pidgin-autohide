/*
 * Release autohide Plugin
 *
 * Copyright (C) 2010, Andreas Bumen <andreas.bumen@dotnine.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02111-1301, USA.
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#ifdef ENABLE_NLS
#  include <locale.h>
#  include <libintl.h>
#  define _(x) dgettext(PACKAGE, x)
#  ifdef dgettext_noop
#    define N_(String) dgettext_noop (PACKAGE, String)
#  else
#    define N_(String) (String)
#  endif
#else
#  include <locale.h>
#  define N_(String) (String)
#  define _(x) (x)
#  define ngettext(Singular, Plural, Number) ((Number == 1) ? (Singular) : (Plural))
#endif

#ifndef PURPLE_PLUGINS
	#define PURPLE_PLUGINS
#endif

#define PLUGIN_VERSION "0.3b"

#include <gdk/gdk.h>

#include "version.h"
#include "debug.h"
#include "gtkblist.h"
#include "pidgin-autohide.h"

#define PREF_MY "/plugins/gtk/abu"
#define PREF_ROOT "/plugins/gtk/abu/autohide"
#define PREF_AUTOHIDE "/plugins/gtk/abu/autohide/autohide"
#define PREF_TIME "/plugins/gtk/abu/autohide/time"
#define PREF_TOPMOST "/plugins/gtk/abu/autohide/topmost"
#define PREF_HIDEMINMAX "/plugins/gtk/abu/autohide/hideminmax"

static PidginBuddyList *gtkblist = NULL;
static gboolean gtkblist_focused = FALSE;
static guint timerId = 0;
static guint focusInHandlerId = 0;
static guint focusOutHandlerId = 0;
static guint autoHideHandlerId = 0;
static guint timeHandlerId = 0;
static guint topMostHandlerId = 0;
static guint hideMinMaxHandlerId = 0;

/**************************************************************************
 * Autohide stuff
 **************************************************************************/

static gboolean do_focus_check(gpointer data)
{
	if (gtkblist && gtkblist->window)
	{
		if (GTK_WIDGET_VISIBLE(gtkblist->window))
		{
			if (!gtkblist_focused && purple_prefs_get_bool(PREF_AUTOHIDE))
			{
				purple_debug(PURPLE_DEBUG_INFO, "Autohide", "Hide buddy list\n");
				purple_blist_set_visible(FALSE);
				return FALSE;
			}
		}
	}

	return TRUE;
}

static gboolean blist_focus_cb(GtkWidget *widget, GdkEventFocus *event, PidginBuddyList *pidginBuddyList)
{
	// Save focused information
	if(event->in)
	{
		purple_debug(PURPLE_DEBUG_INFO, "Autohide", "Buddy list focused\n");
		gtkblist_focused = TRUE;
	}
	else
	{
		purple_debug(PURPLE_DEBUG_INFO, "Autohide", "Buddy list lost focus\n");
		gtkblist_focused = FALSE;
	}

	// Restart Timer
	purple_timeout_remove(timerId);
	
	if (purple_prefs_get_bool(PREF_AUTOHIDE) && !gtkblist_focused)
	{
		if (purple_prefs_get_int(PREF_TIME) > 0) 
		{
			timerId = purple_timeout_add_seconds(purple_prefs_get_int(PREF_TIME), do_focus_check, NULL);
		}		
		else
		{
			do_focus_check(NULL);
		}
	}
	
	return FALSE;
}


static void gtkblist_created_cb(PurpleBuddyList *blist)
{
	purple_debug(PURPLE_DEBUG_INFO, "Autohide", "Buddy list created\n");
	gtkblist = PIDGIN_BLIST(blist);
	
	// Update buddy list
	update_all_prefs();
	
	// Connect to focus events
	focusInHandlerId = g_signal_connect(G_OBJECT(gtkblist->window), "focus-in-event", G_CALLBACK(blist_focus_cb), NULL);
	focusOutHandlerId = g_signal_connect(G_OBJECT(gtkblist->window), "focus-out-event", G_CALLBACK(blist_focus_cb), NULL);
}

/**************************************************************************
 * Prefs stuff
 **************************************************************************/

static void update_autohide_prefs()
{
	purple_debug(PURPLE_DEBUG_INFO, "Autohide", "Update autohide prefs\n");
	purple_timeout_remove(timerId);

	if (purple_prefs_get_bool(PREF_AUTOHIDE))
	{
		if (purple_prefs_get_int(PREF_TIME) > 0) 
		{
			timerId = purple_timeout_add_seconds(purple_prefs_get_int(PREF_TIME), do_focus_check, NULL);
		}			
	}
}

static void update_topmost_prefs()
{
	if (GTK_WINDOW(gtkblist->window) != NULL)
	{
		purple_debug(PURPLE_DEBUG_INFO, "Autohide", "Update topmost prefs\n");
		gtk_window_set_keep_above(GTK_WINDOW(gtkblist->window), purple_prefs_get_bool(PREF_TOPMOST));
	}
}

static void update_hideminmax_prefs()
{
	gboolean blistWasVisible = FALSE;
	
	if (GTK_WINDOW(gtkblist->window) != NULL)
	{
		purple_debug(PURPLE_DEBUG_INFO, "Autohide", "Update hideminmax prefs\n");
		
		blistWasVisible = GTK_WIDGET_VISIBLE(gtkblist->window);

		if (blistWasVisible) purple_blist_set_visible(FALSE);

		if (purple_prefs_get_bool(PREF_HIDEMINMAX))
		{
			gtk_window_set_type_hint(GTK_WINDOW(gtkblist->window), GDK_WINDOW_TYPE_HINT_DIALOG);
		}
		else
		{
			gtk_window_set_type_hint(GTK_WINDOW(gtkblist->window), GDK_WINDOW_TYPE_HINT_NORMAL);
		}

		if (blistWasVisible) purple_blist_set_visible(TRUE);
	}
}

static void update_autohide_prefs_cb(const char *pref, PurplePrefType type, gconstpointer value, gpointer user_data)
{
	update_autohide_prefs();
}

static void update_topmost_prefs_cb(const char *pref, PurplePrefType type, gconstpointer value, gpointer user_data)
{
	update_topmost_prefs();
}

static void update_hideminmax_prefs_cb(const char *pref, PurplePrefType type, gconstpointer value, gpointer user_data)
{
	update_hideminmax_prefs();
}

static void update_all_prefs()
{
	update_autohide_prefs();
	update_topmost_prefs();
	update_hideminmax_prefs();
}

static PurplePluginPrefFrame* get_plugin_pref_frame(PurplePlugin *plugin)
{
	PurplePluginPrefFrame *frame;
	PurplePluginPref *ppref;

	frame = purple_plugin_pref_frame_new();

	ppref = purple_plugin_pref_new_with_label(_("General options"));
	purple_plugin_pref_frame_add(frame, ppref);

        ppref = purple_plugin_pref_new_with_name_and_label(PREF_AUTOHIDE,"Autohide buddy list");
        purple_plugin_pref_frame_add(frame, ppref);

	ppref = purple_plugin_pref_new_with_name_and_label(PREF_TIME,"Time in seconds");
	purple_plugin_pref_set_bounds(ppref, 0, 255);
	purple_plugin_pref_frame_add(frame, ppref);

	ppref = purple_plugin_pref_new_with_label(_("Buddy list options"));
	purple_plugin_pref_frame_add(frame, ppref);

	ppref = purple_plugin_pref_new_with_name_and_label(PREF_TOPMOST,"Always on top");
	purple_plugin_pref_frame_add(frame, ppref);

        ppref = purple_plugin_pref_new_with_name_and_label(PREF_HIDEMINMAX,"Remove minimize and maximize buttons");
        purple_plugin_pref_frame_add(frame, ppref);

	return frame;
}

static PurplePluginUiInfo prefs_info = {
	get_plugin_pref_frame,
	0,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


/**************************************************************************
 * Plugin stuff
 **************************************************************************/
static gboolean plugin_load(PurplePlugin *plugin)
{
	purple_debug(PURPLE_DEBUG_INFO, "Autohide", "Plugin loaded\n");
	purple_signal_connect(pidgin_blist_get_handle(), "gtkblist-created", plugin, PURPLE_CALLBACK(gtkblist_created_cb), NULL);

        autoHideHandlerId = purple_prefs_connect_callback(plugin, PREF_AUTOHIDE, update_autohide_prefs_cb, NULL);
	timeHandlerId = purple_prefs_connect_callback(plugin, PREF_TIME, update_autohide_prefs_cb, NULL); 
	topMostHandlerId = purple_prefs_connect_callback(plugin, PREF_TOPMOST, update_topmost_prefs_cb, NULL);
	hideMinMaxHandlerId = purple_prefs_connect_callback(plugin, PREF_HIDEMINMAX, update_hideminmax_prefs_cb, NULL);

	if (pidgin_blist_get_default_gtk_blist()) gtkblist_created_cb(purple_get_blist());

	return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin)
{
	// Disconnect events
        g_signal_handler_disconnect(G_OBJECT(gtkblist->window), focusInHandlerId);
        g_signal_handler_disconnect(G_OBJECT(gtkblist->window), focusOutHandlerId);
        purple_prefs_disconnect_callback(autoHideHandlerId);
        purple_prefs_disconnect_callback(timeHandlerId);
        purple_prefs_disconnect_callback(topMostHandlerId);
        purple_prefs_disconnect_callback(hideMinMaxHandlerId);

	// Remove always on top
	if (purple_prefs_get_bool(PREF_HIDEMINMAX) && (GTK_WINDOW(gtkblist->window) != NULL))
	{
		gtk_window_set_keep_above(GTK_WINDOW(gtkblist->window), FALSE);
	}

	// Remove timer
	purple_timeout_remove(timerId);

	// Clean variables
	gtkblist = NULL;
	gtkblist_focused = FALSE;

	purple_debug(PURPLE_DEBUG_INFO, "Autohide", "Plugin unloaded\n");

	return TRUE;
}

static PurplePluginInfo info =
{
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,                             			/**< major version  */
	PURPLE_MINOR_VERSION,                             			/**< minor version  */
	PURPLE_PLUGIN_STANDARD,                             			/**< type           */
	NULL,                                             			/**< ui_requirement */
	0,                                                			/**< flags          */
	NULL,                                             			/**< dependencies   */
	PURPLE_PRIORITY_DEFAULT,                            			/**< priority       */
	"gtk-abu-autohide",                                     		/**< id             */
	N_("Autohide"),                       					/**< name           */
	PLUGIN_VERSION,                                  			/**< version        */	                                                  	
	N_("Autohide buddy list and more."),					/**< summary        */
	N_("This plugin autohides an unfocused buddy list after a specified time. It also improves the buddy list with features like 'always on top' and 'hide minimize/maximize buttons'"),	/**< description    */
	"Andreas Bumen <andreas.bumen@dotnine.de>",          			/**< author         */
	"http://www.dotnine.de",                                     		/**< homepage       */
	plugin_load,                                      			/**< load           */
	plugin_unload,                                             		/**< unload         */
	NULL,                                             			/**< destroy        */
	NULL,                                             			/**< ui_info        */
	NULL,                                             			/**< extra_info     */
	&prefs_info,								/**< pref_info     */
	NULL,									/* padding */
	NULL,									/* padding */
	NULL,									/* padding */
	NULL,									/* padding */
	NULL									/* padding */
};

static void init_plugin(PurplePlugin *plugin)
{
	purple_prefs_add_none(PREF_MY);
	purple_prefs_add_none(PREF_ROOT);
	purple_prefs_add_bool(PREF_AUTOHIDE, TRUE);
	purple_prefs_add_int(PREF_TIME, 5);
	purple_prefs_add_bool(PREF_TOPMOST, FALSE);
	purple_prefs_add_bool(PREF_HIDEMINMAX, FALSE);
}

PURPLE_INIT_PLUGIN(relnot, init_plugin, info)
