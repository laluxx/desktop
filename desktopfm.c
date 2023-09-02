// COMPILING
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DESKTOP_PATH g_get_user_special_dir(G_USER_DIRECTORY_DESKTOP)
#define ICON_SIZE 64
#define LABEL_HEIGHT 20  // Assuming 20 pixels for the label, adjust as needed


/* VARIABLES */
double start_selection_x = 0, start_selection_y = 0, end_selection_x = 0, end_selection_y = 0;
bool is_selecting = FALSE;

typedef struct {
    GtkWidget *eventBox;
    gchar *itemPath;
    gchar *execCommand;
    int x;
    int y;
    int width;
    int height;
    bool selected;
} DesktopItem;

GtkWidget *window, *fixedContainer;
GtkWidget *drawingArea;

GList *desktopItems = NULL;

void executeCommand(const gchar *command) {
    system(command);
}

bool is_item_inside_selection(DesktopItem *item, double sx, double sy, double ex, double ey) {
    double left = MIN(sx, ex);
    double right = MAX(sx, ex);
    double top = MIN(sy, ey);
    double bottom = MAX(sy, ey);
    return (item->x + item->width > left) &&
           (item->x < right) &&
           (item->y + item->height > top) &&
           (item->y < bottom);
}

void select_items_within_rectangle() {
    GList *l;
    for (l = desktopItems; l != NULL; l = l->next) {
        DesktopItem *item = (DesktopItem *)l->data;
        if (is_item_inside_selection(item, start_selection_x, start_selection_y, end_selection_x, end_selection_y)) {
            item->selected = true;
            // Highlight the selected item, for example, you could change its background color
            // Alternatively, you can use other visual indicators to show that the item is selected.
        } else {
            item->selected = false;
        }
    }
}

gboolean onItemButtonClick(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    if (event->type == GDK_2BUTTON_PRESS && event->button == 1) {
        DesktopItem *item = (DesktopItem *)user_data;
        if (item->execCommand) {
            executeCommand(item->execCommand);
        } else {
            gchar *openCommand = g_strdup_printf("xdg-open \"%s\"", item->itemPath);
            executeCommand(openCommand);
            g_free(openCommand);
        }
    }
    return TRUE;
}

/* GtkWidget *createDesktopItem(const gchar *itemPath, const gchar *iconName, const gchar *labelText, const gchar *execCommand, int x, int y) { */
/*     GtkWidget *eventBox = gtk_event_box_new(); */
/*     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2); */
/*     gtk_container_add(GTK_CONTAINER(eventBox), vbox); */

/*     GtkIconTheme *iconTheme = gtk_icon_theme_get_default(); */
/*     GdkPixbuf *pixbuf = gtk_icon_theme_load_icon(iconTheme, iconName, ICON_SIZE, 0, NULL); */
/*     GtkWidget *icon = gtk_image_new_from_pixbuf(pixbuf); */
/*     g_object_unref(pixbuf); */

/*     GtkWidget *label = gtk_label_new(labelText); */

/*     gtk_box_pack_start(GTK_BOX(vbox), icon, TRUE, TRUE, 0); */
/*     gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0); */

/*     DesktopItem *item = g_malloc(sizeof(DesktopItem)); */
/*     item->eventBox = eventBox; */
/*     item->itemPath = g_strdup(itemPath); */
/*     item->execCommand = execCommand ? g_strdup(execCommand) : NULL; */
/*     item->x = x; */
/*     item->y = y; */
/*     item->width = ICON_SIZE; */
/*     item->height = ICON_SIZE + LABEL_HEIGHT; */
/*     item->selected = false; */

/*     g_signal_connect(eventBox, "button-press-event", G_CALLBACK(onItemButtonClick), item); */

/*     desktopItems = g_list_append(desktopItems, item); */

/*     return eventBox; */
/* } */

GtkWidget *createDesktopItem(const gchar *itemPath, const gchar *iconName,
                             const gchar *labelText, const gchar *execCommand,
                             int x, int y) {
    // Create event box and vertical box container
    GtkWidget *eventBox = gtk_event_box_new();
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_container_add(GTK_CONTAINER(eventBox), vbox);

    // Load the icon from theme and create image widget from it
    GtkIconTheme *iconTheme = gtk_icon_theme_get_default();
    GdkPixbuf *pixbuf = gtk_icon_theme_load_icon(iconTheme, iconName, ICON_SIZE, 0, NULL);
    GtkWidget *icon = gtk_image_new_from_pixbuf(pixbuf);
    g_object_unref(pixbuf);

    // Create the label
    GtkWidget *label = gtk_label_new(labelText);

    // Pack the icon and label into the vbox
    gtk_box_pack_start(GTK_BOX(vbox), icon, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

    // Create and populate the DesktopItem structure
    DesktopItem *item = g_malloc(sizeof(DesktopItem));
    item->eventBox = eventBox;
    item->itemPath = g_strdup(itemPath);
    item->execCommand = execCommand ? g_strdup(execCommand) : NULL;
    item->x = x;
    item->y = y;
    item->width = ICON_SIZE;
    item->height = ICON_SIZE + LABEL_HEIGHT;
    item->selected = false;

    // Connect the button-press event
    g_signal_connect(eventBox, "button-press-event", G_CALLBACK(onItemButtonClick), item);

    // Append the item to the global list
    desktopItems = g_list_append(desktopItems, item);

    return eventBox;
}



/* void populateDesktop() { */
/*     GDir *dir = g_dir_open(DESKTOP_PATH, 0, NULL); */
/*     if (!dir) return; */

/*     const gchar *filename; */
/*     int x = 10; */
/*     int y = 10; */
/*     while ((filename = g_dir_read_name(dir))) { */
/*         gchar *fullPath = g_build_filename(DESKTOP_PATH, filename, NULL); */

/*         if (g_str_has_suffix(filename, ".desktop")) { */
/*             GKeyFile *keyfile = g_key_file_new(); */
/*             if (g_key_file_load_from_file(keyfile, fullPath, G_KEY_FILE_NONE, NULL)) { */
/*                 gchar *name = g_key_file_get_string(keyfile, "Desktop Entry", "Name", NULL); */
/*                 gchar *icon = g_key_file_get_string(keyfile, "Desktop Entry", "Icon", NULL); */
/*                 gchar *exec = g_key_file_get_string(keyfile, "Desktop Entry", "Exec", NULL); */

/*                 GtkWidget *item = createDesktopItem(fullPath, icon, name, exec); */
/*                 gtk_fixed_put(GTK_FIXED(fixedContainer), item, x, y); */

/*                 g_free(name); */
/*                 g_free(icon); */
/*                 g_free(exec); */
/*             } */
/*             g_key_file_free(keyfile); */
/*         } else { */
/*             gchar *basename = g_path_get_basename(fullPath); */
/*             GtkWidget *item = createDesktopItem(fullPath, "folder", basename, NULL); */
/*             gtk_fixed_put(GTK_FIXED(fixedContainer), item, x, y); */
/*             g_free(basename); */
/*         } */

/*         x += ICON_SIZE + 20; // Adjust for the next item's position */
/*         if (x > 700) { */
/*             x = 10; */
/*             y += ICON_SIZE + 50; // Move to next row */
/*         } */

/*         g_free(fullPath); */
/*     } */
/*     g_dir_close(dir); */
/* } */

void populateDesktop() {
    GDir *dir = g_dir_open(DESKTOP_PATH, 0, NULL);
    if (!dir) return;

    const gchar *filename;
    int x = 10;
    int y = 10;
    while ((filename = g_dir_read_name(dir))) {
        gchar *fullPath = g_build_filename(DESKTOP_PATH, filename, NULL);

        if (g_str_has_suffix(filename, ".desktop")) {
            GKeyFile *keyfile = g_key_file_new();
            if (g_key_file_load_from_file(keyfile, fullPath, G_KEY_FILE_NONE, NULL)) {
                gchar *name = g_key_file_get_string(keyfile, "Desktop Entry", "Name", NULL);
                gchar *icon = g_key_file_get_string(keyfile, "Desktop Entry", "Icon", NULL);
                gchar *exec = g_key_file_get_string(keyfile, "Desktop Entry", "Exec", NULL);

                // Pass x and y to createDesktopItem
                GtkWidget *item = createDesktopItem(fullPath, icon, name, exec, x, y);
                gtk_fixed_put(GTK_FIXED(fixedContainer), item, x, y);

                g_free(name);
                g_free(icon);
                g_free(exec);
            }
            g_key_file_free(keyfile);
        } else {
            gchar *basename = g_path_get_basename(fullPath);

            // Pass x and y to createDesktopItem
            GtkWidget *item = createDesktopItem(fullPath, "folder", basename, NULL, x, y);
            gtk_fixed_put(GTK_FIXED(fixedContainer), item, x, y);
            g_free(basename);
        }

        x += ICON_SIZE + 20; // Adjust for the next item's position
        if (x > 700) {
            x = 10;
            y += ICON_SIZE + 50; // Move to next row
        }

        g_free(fullPath);
    }
    g_dir_close(dir);
}


/* SELECTION */

gboolean on_mouse_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->button == 1) {
        is_selecting = TRUE;
        start_selection_x = event->x;
        start_selection_y = event->y;
        end_selection_x = event->x;
        end_selection_y = event->y;
        gtk_widget_queue_draw(widget);
    }
    return TRUE;
}

gboolean on_mouse_drag(GtkWidget *widget, GdkEventMotion *event, gpointer data) {
    if (is_selecting) {
        end_selection_x = event->x;
        end_selection_y = event->y;
        gtk_widget_queue_draw(widget);  // Request a redraw to update the selection rectangle
    }
    return TRUE;
}


gboolean on_mouse_release(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->button == 1) {
        is_selecting = FALSE;
        select_items_within_rectangle();
        gtk_widget_queue_draw(widget);
    }
    return TRUE;
}

gboolean on_fixed_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    // Draw selection rectangle
    if (is_selecting) {
        cairo_set_source_rgba(cr, 0.2, 0.4, 0.6, 0.4); // semi-transparent blue
        cairo_rectangle(cr, start_selection_x, start_selection_y,
                        end_selection_x - start_selection_x, end_selection_y - start_selection_y);
        cairo_fill(cr);
    }

    // Optionally draw highlights or other visual indications for selected items
    return FALSE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Desktop File Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_widget_set_app_paintable(window, TRUE);

    GtkWidget *overlay = gtk_overlay_new();
    gtk_container_add(GTK_CONTAINER(window), overlay);

    fixedContainer = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(overlay), fixedContainer);

    drawingArea = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawingArea, 800, 600);  // Set to the size of your window
    gtk_widget_set_halign(drawingArea, GTK_ALIGN_FILL);
    gtk_widget_set_valign(drawingArea, GTK_ALIGN_FILL);
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), drawingArea);

    // Enable mouse events for the drawingArea
    gtk_widget_add_events(drawingArea, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

    // Connect the event handlers for drawing and mouse events
    g_signal_connect(drawingArea, "draw", G_CALLBACK(on_fixed_draw), NULL);
    g_signal_connect(drawingArea, "button-press-event", G_CALLBACK(on_mouse_press), NULL);
    g_signal_connect(drawingArea, "motion-notify-event", G_CALLBACK(on_mouse_drag), NULL);
    g_signal_connect(drawingArea, "button-release-event", G_CALLBACK(on_mouse_release), NULL);

    populateDesktop();

    // Set the window to have a transparent background
    GdkScreen *screen = gtk_widget_get_screen(window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(window), visual);
    }
    gtk_widget_set_opacity(window, 0.7); // Adjust for desired transparency

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    // Cleanup
    g_list_free_full(desktopItems, (GDestroyNotify) g_free);

    return 0;
}
// SEGMENTATION FAULT
/* #include <gtk/gtk.h> */
/* #include <gio/gio.h> */
/* #include <stdlib.h> */
/* #include <string.h> */

/* #define DESKTOP_PATH g_get_user_special_dir(G_USER_DIRECTORY_DESKTOP) */
/* #define ICON_SIZE 64 */

/* typedef struct { */
/*     GtkWidget *eventBox; */
/*     GtkWidget *icon; */
/*     GtkWidget *label; */
/*     gchar *execCommand; */
/*     gchar *itemPath; */
/*     gboolean isSelected; */
/* } DesktopItem; */

/* // Global variables */
/* DesktopItem *draggedItem = NULL; */
/* int originalX = 0; */
/* int originalY = 0; */
/* gboolean isDragging = FALSE; */
/* GtkWidget *window, *fixedContainer; */
/* GList *desktopItems = NULL; */

/* // Define your custom CSS string */
/* const gchar *CSS = "button { background-color: blue; color: white; }"; */

/* // Function Prototypes */
/* void executeCommand(const gchar *command); */
/* DesktopItem* getDesktopItemAtPos(gdouble x, gdouble y); */
/* gboolean onItemButtonClick(GtkWidget *widget, GdkEventButton *event, DesktopItem *item); */
/* GtkWidget *createDesktopItem(const gchar *itemPath, const gchar *iconName, const gchar *labelText, const gchar *execCommand); */
/* void populateDesktop(); */
/* void on_mouse_press(GtkWidget *widget, GdkEventButton *event, gpointer data); */
/* void on_mouse_release(GtkWidget *widget, GdkEventButton *event, gpointer data); */
/* gboolean on_mouse_move(GtkWidget *widget, GdkEventMotion *event, gpointer data); */
/* void setupSignalsAndEvents(); */
/* void cleanupDesktopItems(); */

/* // Function Definitions */

/* void executeCommand(const gchar *command) { */
/*     system(command); */
/* } */

/* DesktopItem* getDesktopItemAtPos(gdouble x, gdouble y) { */
/*     for (GList *l = desktopItems; l != NULL; l = l->next) { */
/*         DesktopItem *item = (DesktopItem *)l->data; */
/*         GdkRectangle allocation; */
/*         gtk_widget_get_allocation(item->eventBox, &allocation); */

/*         if (allocation.x <= x && x <= (allocation.x + allocation.width) && */
/*             allocation.y <= y && y <= (allocation.y + allocation.height)) { */
/*             return item; */
/*         } */
/*     } */
/*     return NULL; */
/* } */


/* gboolean onItemButtonClick(GtkWidget *widget, GdkEventButton *event, DesktopItem *item) { */
/*     if (event->type == GDK_2BUTTON_PRESS && event->button == 1) { */
/*         if (item->execCommand) { */
/*             executeCommand(item->execCommand); */
/*         } else { */
/*             gchar *openCommand = g_strdup_printf("xdg-open \"%s\"", item->itemPath); */
/*             executeCommand(openCommand); */
/*             g_free(openCommand); */
/*         } */
/*     } else if (event->type == GDK_BUTTON_PRESS && event->button == 1) { */
/*         item->isSelected = !item->isSelected; */
/*         g_print("Item %s %s\n", item->itemPath, item->isSelected ? "selected" : "deselected"); */
/*     } */
/*     return TRUE; */
/* } */

/* // ... [other functions remain largely unchanged] */
/* GtkWidget *createDesktopItem(const gchar *itemPath, const gchar *iconName, const gchar *labelText, const gchar *execCommand) { */
/*     GtkWidget *eventBox = gtk_event_box_new(); */
/*     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2); */
/*     gtk_container_add(GTK_CONTAINER(eventBox), vbox); */

/*     GtkIconTheme *iconTheme = gtk_icon_theme_get_default(); */
/*     GdkPixbuf *pixbuf = gtk_icon_theme_load_icon(iconTheme, iconName, ICON_SIZE, 0, NULL); */
/*     GtkWidget *icon = gtk_image_new_from_pixbuf(pixbuf); */
/*     g_object_unref(pixbuf); */

/*     GtkWidget *label = gtk_label_new(labelText); */

/*     gtk_box_pack_start(GTK_BOX(vbox), icon, TRUE, TRUE, 0); */
/*     gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0); */

/*     DesktopItem *item = g_malloc0(sizeof(DesktopItem)); */
/*     item->eventBox = eventBox; */
/*     item->itemPath = g_strdup(itemPath); */
/*     item->execCommand = execCommand ? g_strdup(execCommand) : NULL; */
/*     item->isSelected = FALSE; */

/*     g_signal_connect(eventBox, "button-press-event", G_CALLBACK(onItemButtonClick), item); */

/*     desktopItems = g_list_append(desktopItems, item); */

/*     return eventBox; */
/* } */

/* void populateDesktop() { */
/*     GDir *dir = g_dir_open(DESKTOP_PATH, 0, NULL); */
/*     if (!dir) return; */

/*     const gchar *filename; */
/*     int x = 10; */
/*     int y = 10; */
/*     while ((filename = g_dir_read_name(dir))) { */
/*         gchar *fullPath = g_build_filename(DESKTOP_PATH, filename, NULL); */

/*         if (g_str_has_suffix(filename, ".desktop")) { */
/*             GKeyFile *keyfile = g_key_file_new(); */
/*             if (g_key_file_load_from_file(keyfile, fullPath, G_KEY_FILE_NONE, NULL)) { */
/*                 gchar *name = g_key_file_get_string(keyfile, "Desktop Entry", "Name", NULL); */
/*                 gchar *icon = g_key_file_get_string(keyfile, "Desktop Entry", "Icon", NULL); */
/*                 gchar *exec = g_key_file_get_string(keyfile, "Desktop Entry", "Exec", NULL); */

/*                 GtkWidget *item = createDesktopItem(fullPath, icon, name, exec); */
/*                 gtk_fixed_put(GTK_FIXED(fixedContainer), item, x, y); */

/*                 g_free(name); */
/*                 g_free(icon); */
/*                 g_free(exec); */
/*             } */
/*             g_key_file_free(keyfile); */
/*         } else { */
/*             gchar *basename = g_path_get_basename(fullPath); */
/*             GtkWidget *item = createDesktopItem(fullPath, "folder", basename, NULL); */
/*             gtk_fixed_put(GTK_FIXED(fixedContainer), item, x, y); */
/*             g_free(basename); */
/*         } */

/*         x += ICON_SIZE + 20; */
/*         if (x > 700) { */
/*             x = 10; */
/*             y += ICON_SIZE + 50; */
/*         } */

/*         g_free(fullPath); */
/*     } */
/*     g_dir_close(dir); */
/* } */

/* // CSS */
/* void on_mouse_press(GtkWidget *widget, GdkEventButton *event, gpointer data) { */
/*     if (event->type == GDK_BUTTON_PRESS && event->button == 1) { */
/*         isDragging = FALSE; */

/*         DesktopItem *item = getDesktopItemAtPos(event->x, event->y); */
/*         if (item) { */
/*             // Handle the drag start */
/*             draggedItem = item; */

/*             GtkAllocation allocation; */
/*             gtk_widget_get_allocation(item->eventBox, &allocation); */

/*             originalX = allocation.width * 0.5 + allocation.x; */
/*             originalY = allocation.height * 0.5 + allocation.y; */
/*         } */
/*     } */
/* } */

/* // CSS */
/* void on_mouse_release(GtkWidget *widget, GdkEventButton *event, gpointer data) { */
/*     if (event->type == GDK_BUTTON_RELEASE && event->button == 1) { */
/*         DesktopItem *item = (DesktopItem *)data; */

/*         if (isDragging) { */
/*             // Handle dragging code... */
/*             int windowWidth, windowHeight; */
/*             gtk_window_get_size(GTK_WINDOW(window), &windowWidth, &windowHeight); */
/*             if (event->x < 0 || event->x > windowWidth || event->y < 0 || event->y > windowHeight) { */
/*                 // Move the dragged item back to its original position */
/*                 gtk_fixed_move(GTK_FIXED(fixedContainer), draggedItem->eventBox, originalX, originalY); */
/*             } */
/*             draggedItem = NULL; */
/*         } else { */
/*             // Handle click code... */
/*             if (item->isSelected) { */
/*                 // Deselect the item */
/*                 const gchar *deselectCSS = "background-color: none;"; // Reset the background color */
/*                 GtkCssProvider *provider = gtk_css_provider_new(); */
/*                 gtk_css_provider_load_from_data(provider, deselectCSS, -1, NULL); */
/*                 gtk_style_context_add_provider(GTK_STYLE_CONTEXT(item->eventBox), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION); */
/*                 g_object_unref(provider); */
/*                 item->isSelected = FALSE; */
/*             } else { */
/*                 // Select the item */
/*                 const gchar *selectCSS = "background-color: rgba(0,120,215,0.3);"; */
/*                 GtkCssProvider *provider = gtk_css_provider_new(); */
/*                 gtk_css_provider_load_from_data(provider, selectCSS, -1, NULL); */
/*                 gtk_style_context_add_provider(GTK_STYLE_CONTEXT(item->eventBox), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION); */
/*                 g_object_unref(provider); */
/*                 item->isSelected = TRUE; */
/*             } */
/*         } */
/*     } */
/* } */



/* gboolean on_mouse_move(GtkWidget *widget, GdkEventMotion *event, gpointer data) { */
/*     if (draggedItem) { */
/*         isDragging = TRUE; */
/*         gtk_fixed_move(GTK_FIXED(fixedContainer), draggedItem->eventBox, event->x - ICON_SIZE / 2, event->y - ICON_SIZE / 2); */
/*     } */
/*     return FALSE; // let the event propagate further */
/* } */

/* void setupSignalsAndEvents() { */
/*     g_signal_connect(window, "button-press-event", G_CALLBACK(on_mouse_press), NULL); */
/*     g_signal_connect(window, "button-release-event", G_CALLBACK(on_mouse_release), NULL); */
/*     g_signal_connect(window, "motion-notify-event", G_CALLBACK(on_mouse_move), NULL); */
/*     gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK); */
/*     g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); */
/* } */

/* void cleanupDesktopItems() { */
/*     for (GList *l = desktopItems; l != NULL; l = l->next) { */
/*         DesktopItem *item = (DesktopItem *)l->data; */
/*         g_free(item->itemPath); */
/*         if (item->execCommand) { */
/*             g_free(item->execCommand); */
/*         } */
/*         g_free(item); */
/*     } */
/*     g_list_free(desktopItems); */
/* } */

/* int main(int argc, char **argv) { */
/*     gtk_init(&argc, &argv); */

/*     // Window and container setup */
/*     window = gtk_window_new(GTK_WINDOW_TOPLEVEL); */
/*     gtk_window_set_default_size(GTK_WINDOW(window), 800, 600); */
/*     gtk_window_maximize(GTK_WINDOW(window)); */
/*     fixedContainer = gtk_fixed_new(); */
/*     gtk_container_add(GTK_CONTAINER(window), fixedContainer); */

/*     // Signal setup */
/*     setupSignalsAndEvents(); */

/*     // Populate the desktop with items */
/*     populateDesktop(); */

/*     // Apply custom CSS to the button */
/*     const gchar *CSS = "button { background-color: blue; color: white; }"; */
/*     GdkDisplay *display = gdk_display_get_default(); */
/*     GdkScreen *screen = gdk_display_get_default_screen(display); */
/*     GtkCssProvider *provider = gtk_css_provider_new(); */
/*     gtk_css_provider_load_from_data(provider, CSS, -1, NULL); */
/*     gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION); */
/*     g_object_unref(provider); */

/*     // Main GTK loop */
/*     gtk_widget_show_all(window); */
/*     gtk_main(); */

/*     // Cleanup before exit */
/*     cleanupDesktopItems(); */

/*     return 0; */
/* } */




