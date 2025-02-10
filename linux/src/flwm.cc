#include <flutter_linux/flutter_linux.h>
#include <fl_linux_window_manager/fl_linux_window_manager_plugin.h>

#include <window_manager/window_manager.h>
#include <message_handler/message_handler.h>

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

    /// Create the method channel for the communication between the Flutter and the plugin
    g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
    g_autoptr(FlMethodChannel) channel =
        fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
            "fl_linux_window_manager",
            FL_METHOD_CODEC(codec));

    /// Setting the callback function to execute when a method call is recieved from dart code.
    /// 
    /// Here we are setting the user_data for the callback as the Plugin object itself.
    fl_method_channel_set_method_call_handler(channel, messageHandler, NULL, NULL);
}
