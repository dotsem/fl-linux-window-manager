
#include <iostream>

#include <window_manager/window_manager.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include <protocol_bindings/wlr_layer_shell_protocol_client.h>

/**
 * Static member initialization
 */
std::vector<FLWM::Window> FLWM::WindowManager::windows;

void FLWM::WindowManager::addWindow(GtkWindow* window, std::string id) {
    /// Check if the window is already added to the list of windows.
    for (Window window : windows) {
        if (window.id == id) {
            return;
        }
    }

    /// Add the window to the list, becase the list do not have this window
    Window newWindow;
    newWindow.id = id;
    newWindow.window = window;

    windows.push_back(newWindow);
}

FLWM::WindowManager::WindowManager(std::string id) {
    bool isFound = false;

    for (Window window : windows) {
        if (window.id == id) {
            this->window = window;
            isFound = true;
            break;
        }
    }

    if (!isFound) {
        std::cerr << "The window with the given ID is not found!" << std::endl;

        this->window.id = "";
        this->window.window = nullptr;
    }
}

void FLWM::WindowManager::convertToLayer(GtkWindow* window) {
    /// Initialize the window for layer-shell
    gtk_layer_init_for_window(GTK_WINDOW(window));

    /// Setting the default layer to top
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);

    /// Setting default keybaord interactivity to on-demand
    gtk_layer_set_keyboard_mode(GTK_WINDOW(window), GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND);
}

void FLWM::WindowManager::setSize(unsigned int width, unsigned int height) {
    gtk_widget_set_size_request(GTK_WIDGET(window.window), width, height);

    /// Change the size of the flutter view inside the window.
    /// Only If the view is already present in the window.
    /// For new windows, the FL_VIEW may not be attached as child, so this step will be skipped.
    GList* children = gtk_container_get_children(GTK_CONTAINER(window.window));
    for (GList* iter = children; iter != nullptr; iter = iter->next) {
        GtkWidget* child = GTK_WIDGET(iter->data);

        if (FL_IS_VIEW(child)) {
            gtk_widget_set_size_request(GTK_WIDGET(child), width, height);
        }
    }
    g_list_free(children);
}

/**
 * This function will try to find the GlArea widget that is internally used to
 * render the Flutter content from the Flutter engine.alignas
 *
 * This widget is required to make the window transparent.
 */
GtkWidget* _findGLAreaWidget(GtkWidget* widget) {
    if (GTK_IS_GL_AREA(widget)) {
        return widget;
    }

    if (GTK_IS_CONTAINER(widget)) {
        GList* children = gtk_container_get_children(GTK_CONTAINER(widget));
        for (GList* iter = children; iter != NULL; iter = g_list_next(iter)) {
            GtkWidget* child = GTK_WIDGET(iter->data);
            GtkWidget* gl_area = _findGLAreaWidget(child);
            if (gl_area != NULL) {
                g_list_free(children);
                return gl_area;
            }
        }
        g_list_free(children);
    }

    return NULL;
}

void FLWM::WindowManager::enableTransparency() {
    GtkWidget* firstChild =
        GTK_WIDGET(gtk_container_get_children(GTK_CONTAINER(window.window))->data);

    /// Enable alpha channel for the GLArea widget.
    GtkWidget* glAreaWidget = _findGLAreaWidget(GTK_WIDGET(firstChild));
    if (glAreaWidget != NULL) {
        gtk_gl_area_set_has_alpha(GTK_GL_AREA(glAreaWidget), TRUE);
    }

    /// Make the window transparent.
    gtk_widget_set_app_paintable(GTK_WIDGET(window.window), TRUE);
    GdkScreen* gtkScreen = gdk_screen_get_default();
    GdkVisual* visual = gdk_screen_get_rgba_visual(gtkScreen);

    if (visual != NULL && gdk_screen_is_composited(gtkScreen)) {
        gtk_widget_set_visual(GTK_WIDGET(window.window), visual);
    }
}

void FLWM::WindowManager::setTitle(std::string title) {
    gtk_window_set_title(window.window, title.c_str());

    if (gtk_layer_is_layer_window(GTK_WINDOW(window.window))) {
        gtk_layer_set_namespace(GTK_WINDOW(window.window), title.c_str());
    }
}

void FLWM::WindowManager::setIsDecorated(bool isDecorated) {
    gtk_window_set_decorated(window.window, isDecorated);
}

void FLWM::WindowManager::createWindow(
    std::string id,
    std::string title,
    unsigned int width,
    unsigned int height,
    bool isLayer,
    std::vector<std::string> args) {

    /// Check if the ID is already taken
    for (Window window : windows) {
        if (window.id == id) {
            std::cerr << "The ID is already taken! Cannot create new window" << std::endl;
            return;
        }
    }

    /// Create a new window for the application
    GtkWindow* newWindow = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    if (newWindow == NULL) {
        std::cerr << "Failed to create a new window!" << std::endl;
        return;
    }

    /// Add the window to the list of windows
    addWindow(newWindow, id);

    /// Set the default size of the window
    gtk_window_set_default_size(GTK_WINDOW(newWindow), width, height);

    FLWM::WindowManager manager = FLWM::WindowManager(id);
    manager.setSize(width, height);

    if (isLayer) {
        convertToLayer(newWindow);
    }

    /// Set the title of the window
    manager.setTitle(title);

    /// Enable or diable the title bar for the new window
    manager.setIsDecorated(!isLayer);

    /// Show the new window
    gtk_widget_show(GTK_WIDGET(newWindow));

    /// Create the dart VM and start the flutter engine
    g_autoptr(FlDartProject) project = fl_dart_project_new();

    /// CLI arguments to be passed to the dart entrypoint main() function
    /// The last item in this array must be a NULL pointer, to indicate the end of the array items.
    /// Otherwise errors will occur.
    int cli_args_size = args.size() + 1;
    char** cli_args = new char* [cli_args_size];

    for (size_t i = 0; i < args.size(); ++i) {
        cli_args[i] = new char[args[i].size() + 1];
        strcpy(cli_args[i], args[i].c_str());
    }

    /// CLI arguments list must be terminated with a NULL pointer
    /// Otherwise, the dart VM will crash
    cli_args[cli_args_size - 1] = nullptr;

    fl_dart_project_set_dart_entrypoint_arguments(project, cli_args);


    FlView* view = fl_view_new(project);
    gtk_widget_show(GTK_WIDGET(view));
    gtk_container_add(GTK_CONTAINER(newWindow), GTK_WIDGET(view));

    /// Register the plugins for the flutter application, to the new flutter view/engine
    fl_register_plugins(FL_PLUGIN_REGISTRY(view));

    gtk_widget_grab_focus(GTK_WIDGET(view));

    /// Free the CLI args array
    for (size_t i = 0; i < args.size(); ++i) {
        delete[] cli_args[i];
    }
    delete[] cli_args;
}

/**
 * Convert the layer enum to the layer shell library layer enum value
 */
GtkLayerShellLayer _convertLayerEnum(FLWM::Layer layer) {
    switch (layer) {
    case FLWM::Layer::LAYER_TOP:
        return GTK_LAYER_SHELL_LAYER_TOP;
    case FLWM::Layer::LAYER_BOTTOM:
        return GTK_LAYER_SHELL_LAYER_BOTTOM;
    case FLWM::Layer::LAYER_OVERLAY:
        return GTK_LAYER_SHELL_LAYER_OVERLAY;
    case FLWM::Layer::LAYER_BACKGROUND:
        return GTK_LAYER_SHELL_LAYER_BACKGROUND;
    }
}

void FLWM::WindowManager::setLayer(Layer layer) {
    gtk_layer_set_layer(GTK_WINDOW(window.window), _convertLayerEnum(layer));
}

void FLWM::WindowManager::setLayerMargin(
    unsigned int top,
    unsigned int right,
    unsigned int bottom,
    unsigned int left) {

    gtk_layer_set_margin(GTK_WINDOW(window.window), GTK_LAYER_SHELL_EDGE_TOP, top);
    gtk_layer_set_margin(GTK_WINDOW(window.window), GTK_LAYER_SHELL_EDGE_RIGHT, right);
    gtk_layer_set_margin(GTK_WINDOW(window.window), GTK_LAYER_SHELL_EDGE_BOTTOM, bottom);
    gtk_layer_set_margin(GTK_WINDOW(window.window), GTK_LAYER_SHELL_EDGE_LEFT, left);
}

void FLWM::WindowManager::setLayerAnchor(int anchor) {
    bool anchorTop = (anchor & SCREEN_EDGE_TOP) != 0;
    bool anchorRight = (anchor & SCREEN_EDGE_RIGHT) != 0;
    bool anchorBottom = (anchor & SCREEN_EDGE_BOTTOM) != 0;
    bool anchorLeft = (anchor & SCREEN_EDGE_LEFT) != 0;

    gtk_layer_set_anchor(GTK_WINDOW(window.window), GTK_LAYER_SHELL_EDGE_TOP, anchorTop);
    gtk_layer_set_anchor(GTK_WINDOW(window.window), GTK_LAYER_SHELL_EDGE_RIGHT, anchorRight);
    gtk_layer_set_anchor(GTK_WINDOW(window.window), GTK_LAYER_SHELL_EDGE_BOTTOM, anchorBottom);
    gtk_layer_set_anchor(GTK_WINDOW(window.window), GTK_LAYER_SHELL_EDGE_LEFT, anchorLeft);
}

GtkLayerShellKeyboardMode _convertKeyboardInteractivity(FLWM::KeyboardInteractivity kbInteractivity) {
    switch (kbInteractivity) {
    case FLWM::KeyboardInteractivity::NONE:
        return GTK_LAYER_SHELL_KEYBOARD_MODE_NONE;
    case FLWM::KeyboardInteractivity::ON_DEMAND:
        return GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND;
    case FLWM::KeyboardInteractivity::EXCLUSIVE:
        return GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE;
    }
}

void FLWM::WindowManager::setKeyboardInteractivity(KeyboardInteractivity interactivity) {
    gtk_layer_set_keyboard_mode(GTK_WINDOW(window.window), _convertKeyboardInteractivity(interactivity));
}

void FLWM::WindowManager::enableLayerAutoExclusive() {
    gtk_layer_auto_exclusive_zone_enable(GTK_WINDOW(window.window));
}

void FLWM::WindowManager::setLayerExclusiveZone(int length) {
    GtkWindow* w = GTK_WINDOW(window.window);

    struct zwlr_layer_surface_v1* layerSurface = gtk_layer_get_zwlr_layer_surface_v1(w);
    zwlr_layer_surface_v1_set_exclusive_zone(layerSurface, length);
}

void FLWM::WindowManager::closeWindow() {
    GtkWidget* fl_view = GTK_WIDGET(gtk_container_get_children(GTK_CONTAINER(window.window))->data);
    gtk_container_remove(GTK_CONTAINER(window.window), fl_view);
    gtk_window_close(window.window);

    /// Remove the window from the list of windows
    for (auto iter = windows.begin(); iter != windows.end(); iter++) {
        if (iter->id == window.id) {
            windows.erase(iter);
            break;
        }
    }
}