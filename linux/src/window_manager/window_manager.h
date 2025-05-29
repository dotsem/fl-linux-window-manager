#pragma once

#include <gtk/gtk.h>
#include <vector>
#include <string>
#include <map>

#include <flutter_linux/flutter_linux.h>
#include <wayland-client.h>

/**
 * @brief Declaration for the function that is used to register the plugins for a
 * flutter application.
 *
 * The definition of this function is in the generated_plugin_registrant.cc file of the
 * flutter application that register this plugin.
 *
 * So the definition of this function is not available in this plugin.
 */
void fl_register_plugins(FlPluginRegistry *registry);

namespace FLWM
{
    struct Window
    {
        /**
         * A unique identifier for the window. This is used to identify the window in the window manager.
         */
        std::string id;

        /**
         * The actual GTK window object that is created by the window manager.
         */
        GtkWindow *window;

        /**
         * The input region associated with the window. This is used to control the area of the
         * window that can receive input events.
         */
        wl_region *inputRegion;

        /**
         * Stores the method channels created by the user for this window.
         */
        std::map<std::string, FlMethodChannel *> methodChannels;
    };

    enum __attribute__((visibility("default"))) Layer
    {
        LAYER_BACKGROUND = 1,
        LAYER_BOTTOM,
        LAYER_TOP,
        LAYER_OVERLAY
    };

    enum __attribute__((visibility("default"))) ScreenEdge
    {
        SCREEN_EDGE_TOP = 1 << 0,
        SCREEN_EDGE_RIGHT = 1 << 1,
        SCREEN_EDGE_BOTTOM = 1 << 2,
        SCREEN_EDGE_LEFT = 1 << 3,
    };

    enum __attribute__((visibility("default"))) KeyboardInteractivity
    {
        NONE,
        EXCLUSIVE,
        ON_DEMAND
    };

    class __attribute__((visibility("default"))) WindowManager
    {
    public:
        /**
         * Create a new window manager instance for the given window ID.
         */
        WindowManager(std::string id);

        /**
         * The compositor object that is used to create the wayland objects.
         */
        static wl_compositor *compositor;

    private:
        /**
         * A map of all windows created by this application. This is used to keep track of all windows
         * and manage them accordingly.
         */
        static std::map<std::string, Window> windows;

        /**
         * The window that needs to be managed, by this instance of window manager.
         */
        Window *window;

    public:
        /**
         * Add a new window to the list of windows managed by the window manager.
         */
        static void addWindow(GtkWindow *window, std::string id);

        /**
         * Converts the role of the window to a layer shell surface.
         */
        static void convertToLayer(GtkWindow *window);

        static void createWindow(std::string id,
                                 std::string title,
                                 unsigned int width,
                                 unsigned int height,
                                 bool isLayer,
                                 std::vector<std::string> args);

        /**
         * Change the layer of the window to the given layer.
         */
        void setLayer(Layer layer);

        /**
         * Set the size of the window to the given width and height.
         */
        void setSize(unsigned int width, unsigned int height);

        /**
         * Set the title of the window to the given title.
         */
        void setTitle(std::string title);

        /**
         * Set the margin of the window to the given values.
         */
        void setLayerMargin(unsigned int top, unsigned int right, unsigned int bottom, unsigned int left);

        /**
         * Set the anchor of the layer to the given value.
         */
        void setLayerAnchor(int anchor);

        /**
         * Enable the transparency of the window. So that the window can have a transparent background.
         */
        void enableTransparency();

        /**
         * Set if the window have title bar and border.
         */
        void setIsDecorated(bool isDecorated);

        /**
         * Set the keyboard interactivity of the window.
         */
        void setKeyboardInteractivity(KeyboardInteractivity interactivity);

        /**
         * Make the window's layer as exclusive region, so that no other windows can overlap this window.
         */
        void enableLayerAutoExclusive();

        /**
         * Set the exclusive zone of the window.
         * (See the docs of wl_layer_surface_set_exclusive_zone() for more information)
         */
        void setLayerExclusiveZone(int length);

        /**
         * Close the window and free all resources associated with the window.
         */
        void closeWindow();

        /**
         * Hide the window from the screen.
         */
        void hideWindow();

        /**
         * Show a window that is already hidden.
         */
        void showWindow();

        /**
         * Create a new method channel in the platform side for this window.
         */
        void createMethodChannel(std::string channelName, FlMethodChannelMethodCallHandler handler, void *userData);

        /**
         * Send a method call to the given channel.
         */
        void sendMethodCall(std::string channelName, std::string methodName, FlValue *args);

        /**
         * Disable inputs for the window.
         */
        void setInfinteInputRegion();

        /**
         * Add the given region to the input region of the window.
         * If the window already have infinte input region, then this will remove the infinite input region
         * and add the given region.
         */
        void addInputRegion(int x, int y, int width, int height);

        /**
         * Subtract the given region from the input region of the window.
         */
        void subtractInputRegion(int x, int y, int width, int height);

        /**
         * Method to get monitor list
         */
        FlValue *getMonitorList();
        /**
         * Method to set monitor list
         */
        void setMonitor(int monitor_index);
    };

}