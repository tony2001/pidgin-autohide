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

static gboolean do_focus_check(gpointer data);
static gboolean blist_focus_cb(GtkWidget *widget, GdkEventFocus *event, PidginBuddyList *pidginBuddyList);
static void gtkblist_created_cb(PurpleBuddyList *blist);
static void update_all_prefs(void);
static void update_autohide_prefs(void);
static void update_topmost_prefs(void);
static void update_hideminmax_prefs(void);
static void update_autohide_prefs_cb(const char *pref, PurplePrefType type, gconstpointer value, gpointer user_data);
static void update_topmost_prefs_cb(const char *pref, PurplePrefType type, gconstpointer value, gpointer user_data);
static void update_hideminmax_prefs_cb(const char *pref, PurplePrefType type, gconstpointer value, gpointer user_data);