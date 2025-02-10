#include <flutter_linux/flutter_linux.h>
#include <fl_linux_window_manager/fl_linux_window_manager_plugin.h>

#include <window_manager/window_manager.h>

/**
 * A callback that will be called when the plugin is registered with the Flutter application.
 * This is considered as the main entry point of the plugin. (Just like main() in programs).
 *
 * Here the parameter registrar represents the FlView* GTK_WIDGET that renders the flutter application.
 */
void fl_linux_window_manager_plugin_register_with_registrar(FlPluginRegistrar* registrar) {
    GtkWidget* view = GTK_WIDGET(fl_plugin_registrar_get_view(registrar));
    GtkWindow* window = GTK_WINDOW(gtk_widget_get_toplevel(view));

    /// Add the window to the list of windows
    FLWM::WindowManager::addWindow(window, "main");
}
