#include <iostream>
#include <string.h>

#include <gdk/gdkwayland.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include <protocol_bindings/wlr_layer_shell_protocol_client.h>
#include <window_manager/window_manager.h>

/**
 * Static member initialization
 */
std::map<std::string, FLWM::Window> FLWM::WindowManager::windows;
wl_compositor *FLWM::WindowManager::compositor = NULL;

void _registryHandler(void *data, struct wl_registry *registry, uint32_t id,
                      const char *interface, uint32_t version) {
  if (strcmp(interface, "wl_compositor") == 0) {
    FLWM::WindowManager::compositor = (wl_compositor *)wl_registry_bind(
        registry, id, &wl_compositor_interface, 1);
  }
}

void FLWM::WindowManager::addWindow(GtkWindow *window, std::string id) {
  /// Initialize the wayland objects.
  if (FLWM::WindowManager::compositor == NULL) {

    GdkDisplay *gdk_display = gtk_widget_get_display(GTK_WIDGET(window));
    struct wl_display *wl_display =
        gdk_wayland_display_get_wl_display(gdk_display);

    struct wl_registry *registry = wl_display_get_registry(wl_display);
    static const struct wl_registry_listener registry_listener = {
        .global = _registryHandler,
        .global_remove = [](void *data, struct wl_registry *registry,
                            uint32_t name) {},
    };
    wl_registry_add_listener(registry, &registry_listener, NULL);
    /// Wait for fetching the registry data binding.
    wl_display_roundtrip(wl_display);
  }

  /// Check if the window is already added to the list of windows.
  if (windows.find(id) != windows.end()) {
    return;
  }

  /// Add the window to the list, becase the list do not have this window
  Window newWindow;
  newWindow.id = id;
  newWindow.window = window;
  newWindow.methodChannels = std::map<std::string, FlMethodChannel *>();
  newWindow.inputRegion = NULL;

  windows[id] = newWindow;
}

FLWM::WindowManager::WindowManager(std::string id) {
  if (windows.find(id) != windows.end()) {
    this->window = &(windows[id]);
  } else {
    std::cerr << "The window with the given ID is not found!" << std::endl;
    this->window = NULL;
  }
}

void FLWM::WindowManager::convertToLayer(GtkWindow *window) {
  /// Initialize the window for layer-shell
  gtk_layer_init_for_window(GTK_WINDOW(window));

  /// Setting the default layer to top
  gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);

  /// Setting default keybaord interactivity to on-demand
  gtk_layer_set_keyboard_mode(GTK_WINDOW(window),
                              GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND);
}

void FLWM::WindowManager::setSize(unsigned int width, unsigned int height) {
  gtk_widget_set_size_request(GTK_WIDGET(window->window), width, height);

  /// Change the size of the flutter view inside the window.
  /// Only If the view is already present in the window.
  /// For new windows, the FL_VIEW may not be attached as child, so this step
  /// will be skipped.
  GList *children = gtk_container_get_children(GTK_CONTAINER(window->window));
  for (GList *iter = children; iter != nullptr; iter = iter->next) {
    GtkWidget *child = GTK_WIDGET(iter->data);

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
GtkWidget *_findGLAreaWidget(GtkWidget *widget) {
  if (GTK_IS_GL_AREA(widget)) {
    return widget;
  }

  if (GTK_IS_CONTAINER(widget)) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(widget));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
      GtkWidget *child = GTK_WIDGET(iter->data);
      GtkWidget *gl_area = _findGLAreaWidget(child);
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
  GtkWidget *firstChild = GTK_WIDGET(
      gtk_container_get_children(GTK_CONTAINER(window->window))->data);

  /// Enable alpha channel for the GLArea widget.
  GtkWidget *glAreaWidget = _findGLAreaWidget(GTK_WIDGET(firstChild));
  if (glAreaWidget != NULL) {
    gtk_gl_area_set_has_alpha(GTK_GL_AREA(glAreaWidget), TRUE);
  }

  /// Make the window transparent.
  gtk_widget_set_app_paintable(GTK_WIDGET(window->window), TRUE);
  GdkScreen *gtkScreen = gdk_screen_get_default();
  GdkVisual *visual = gdk_screen_get_rgba_visual(gtkScreen);

  if (visual != NULL && gdk_screen_is_composited(gtkScreen)) {
    gtk_widget_set_visual(GTK_WIDGET(window->window), visual);
  }
}

void FLWM::WindowManager::setTitle(std::string title) {
  gtk_window_set_title(window->window, title.c_str());

  if (gtk_layer_is_layer_window(GTK_WINDOW(window->window))) {
    gtk_layer_set_namespace(GTK_WINDOW(window->window), title.c_str());
  }
}

void FLWM::WindowManager::setIsDecorated(bool isDecorated) {
  gtk_window_set_decorated(window->window, isDecorated);
}

FlValue *FLWM::WindowManager::isWindowIdUsed(std::string id) {
  bool used = windows.find(id) != windows.end();
  return fl_value_new_bool(used);
}

void FLWM::WindowManager::createWindow(std::string id, std::string title,
                                       unsigned int width, unsigned int height,
                                       bool isLayer,
                                       std::vector<std::string> args) {

  /// Check if the ID is already taken
  if (windows.find(id) != windows.end()) {
    std::cerr << "The ID is already taken! Cannot create new window"
              << std::endl;
    return;
  }

  /// Create a new window for the application
  GtkWindow *newWindow = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
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
  /// The last item in this array must be a NULL pointer, to indicate the end of
  /// the array items. Otherwise errors will occur.
  int cli_args_size = args.size() + 1;
  char **cli_args = new char *[cli_args_size];

  for (size_t i = 0; i < args.size(); ++i) {
    cli_args[i] = new char[args[i].size() + 1];
    strcpy(cli_args[i], args[i].c_str());
  }

  /// CLI arguments list must be terminated with a NULL pointer
  /// Otherwise, the dart VM will crash
  cli_args[cli_args_size - 1] = nullptr;

  fl_dart_project_set_dart_entrypoint_arguments(project, cli_args);

  FlView *view = fl_view_new(project);
  gtk_widget_show(GTK_WIDGET(view));
  gtk_container_add(GTK_CONTAINER(newWindow), GTK_WIDGET(view));

  /// Register the plugins for the flutter application, to the new flutter
  /// view/engine
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
  gtk_layer_set_layer(GTK_WINDOW(window->window), _convertLayerEnum(layer));
}

void FLWM::WindowManager::setLayerMargin(unsigned int top, unsigned int right,
                                         unsigned int bottom,
                                         unsigned int left) {

  gtk_layer_set_margin(GTK_WINDOW(window->window), GTK_LAYER_SHELL_EDGE_TOP,
                       top);
  gtk_layer_set_margin(GTK_WINDOW(window->window), GTK_LAYER_SHELL_EDGE_RIGHT,
                       right);
  gtk_layer_set_margin(GTK_WINDOW(window->window), GTK_LAYER_SHELL_EDGE_BOTTOM,
                       bottom);
  gtk_layer_set_margin(GTK_WINDOW(window->window), GTK_LAYER_SHELL_EDGE_LEFT,
                       left);
}

void FLWM::WindowManager::setLayerAnchor(int anchor) {
  bool anchorTop = (anchor & SCREEN_EDGE_TOP) != 0;
  bool anchorRight = (anchor & SCREEN_EDGE_RIGHT) != 0;
  bool anchorBottom = (anchor & SCREEN_EDGE_BOTTOM) != 0;
  bool anchorLeft = (anchor & SCREEN_EDGE_LEFT) != 0;

  gtk_layer_set_anchor(GTK_WINDOW(window->window), GTK_LAYER_SHELL_EDGE_TOP,
                       anchorTop);
  gtk_layer_set_anchor(GTK_WINDOW(window->window), GTK_LAYER_SHELL_EDGE_RIGHT,
                       anchorRight);
  gtk_layer_set_anchor(GTK_WINDOW(window->window), GTK_LAYER_SHELL_EDGE_BOTTOM,
                       anchorBottom);
  gtk_layer_set_anchor(GTK_WINDOW(window->window), GTK_LAYER_SHELL_EDGE_LEFT,
                       anchorLeft);
}

GtkLayerShellKeyboardMode
_convertKeyboardInteractivity(FLWM::KeyboardInteractivity kbInteractivity) {
  switch (kbInteractivity) {
  case FLWM::KeyboardInteractivity::NONE:
    return GTK_LAYER_SHELL_KEYBOARD_MODE_NONE;
  case FLWM::KeyboardInteractivity::ON_DEMAND:
    return GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND;
  case FLWM::KeyboardInteractivity::EXCLUSIVE:
    return GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE;
  }
}

void FLWM::WindowManager::setKeyboardInteractivity(
    KeyboardInteractivity interactivity) {
  gtk_layer_set_keyboard_mode(GTK_WINDOW(window->window),
                              _convertKeyboardInteractivity(interactivity));
}

void FLWM::WindowManager::enableLayerAutoExclusive() {
  gtk_layer_auto_exclusive_zone_enable(GTK_WINDOW(window->window));
}

void FLWM::WindowManager::setLayerExclusiveZone(int length) {
  GtkWindow *w = GTK_WINDOW(window->window);

  struct zwlr_layer_surface_v1 *layerSurface =
      gtk_layer_get_zwlr_layer_surface_v1(w);
  zwlr_layer_surface_v1_set_exclusive_zone(layerSurface, length);
}

void FLWM::WindowManager::closeWindow() {
  /// Destroy the input region if it is not NULL
  if (window->inputRegion != NULL) {
    wl_region_destroy(window->inputRegion);
  }

  GtkWidget *fl_view = GTK_WIDGET(
      gtk_container_get_children(GTK_CONTAINER(window->window))->data);
  gtk_container_remove(GTK_CONTAINER(window->window), fl_view);
  gtk_window_close(window->window);

  /// Remove the window from the list of windows
  windows.erase(window->id);

  /// Free the window object
  window->id = "";
  window->window = nullptr;

  /// Clear the method channels for this window
  for (auto const &[key, val] : window->methodChannels) {
    fl_method_channel_set_method_call_handler(val, NULL, NULL, NULL);
    g_object_unref(val);
  }
  window->methodChannels.clear();
}

void FLWM::WindowManager::hideWindow() {
  gtk_widget_hide(GTK_WIDGET(window->window));
}

void FLWM::WindowManager::showWindow() {
  gtk_widget_show(GTK_WIDGET(window->window));
}

FlValue *FLWM::WindowManager::isVisible() {
  if (window == nullptr || window->window == nullptr) {
    return fl_value_new_bool(false);
  }

  bool visible = gtk_widget_is_visible(GTK_WIDGET(window->window));
  return fl_value_new_bool(
      visible); 
}

void FLWM::WindowManager::setFocus() {
  gtk_widget_grab_focus(GTK_WIDGET(window->window));
}

void FLWM::WindowManager::createMethodChannel(
    std::string channelName, FlMethodChannelMethodCallHandler handler,
    void *userData) {
  /// Get the flutter view from the window. This is used as the registrar for
  /// getting the messenger.
  GtkWindow *gtkWindow = window->window;
  FlView *flView =
      FL_VIEW(gtk_container_get_children(GTK_CONTAINER(gtkWindow))->data);
  if (!FL_IS_VIEW(flView)) {
    std::cerr << "The flutter view is not found in the window!" << std::endl;
  }
  FlPluginRegistry *registry = FL_PLUGIN_REGISTRY(flView);
  g_autoptr(FlPluginRegistrar) registrar =
      fl_plugin_registry_get_registrar_for_plugin(registry,
                                                  "FlLinuxWindowManagerPlugin");

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            channelName.c_str(), FL_METHOD_CODEC(codec));

  fl_method_channel_set_method_call_handler(channel, handler, userData, free);

  /// Add the channel to the window's method channels list
  window->methodChannels[channelName] = channel;
}

void FLWM::WindowManager::sendMethodCall(std::string channelName,
                                         std::string methodName,
                                         FlValue *args) {
  /// Get the method channel from the window
  FlMethodChannel *channel = window->methodChannels[channelName];

  if (channel == NULL) {
    std::cerr << "The method channel is not found for the given name!"
              << std::endl;
    return;
  }

  /// Send the method call to the channel
  fl_method_channel_invoke_method(channel, methodName.c_str(), args, NULL, NULL,
                                  NULL);
}

void FLWM::WindowManager::setInfinteInputRegion() {
  if (window->inputRegion != NULL) {
    wl_region_destroy(window->inputRegion);
    window->inputRegion = NULL;
  }

  /// Set the input region to NULL for the window
  GdkWindow *gdkWindow = gtk_widget_get_window(GTK_WIDGET(window->window));
  struct wl_surface *wlSurface = gdk_wayland_window_get_wl_surface(gdkWindow);

  wl_surface_set_input_region(wlSurface, NULL);
  wl_surface_commit(wlSurface);
}

void FLWM::WindowManager::addInputRegion(int x, int y, int width, int height) {
  if (window->inputRegion == NULL) {
    window->inputRegion =
        wl_compositor_create_region(FLWM::WindowManager::compositor);
  }

  wl_region_add(window->inputRegion, x, y, width, height);

  /// Set the input region to the window
  GdkWindow *gdkWindow = gtk_widget_get_window(GTK_WIDGET(window->window));
  struct wl_surface *wlSurface = gdk_wayland_window_get_wl_surface(gdkWindow);

  wl_surface_set_input_region(wlSurface, window->inputRegion);
  wl_surface_commit(wlSurface);
}

void FLWM::WindowManager::subtractInputRegion(int x, int y, int width,
                                              int height) {
  if (window->inputRegion == NULL) {
    window->inputRegion =
        wl_compositor_create_region(FLWM::WindowManager::compositor);
    wl_region_add(window->inputRegion, 0, 0, INT32_MAX, INT32_MAX);
  }

  wl_region_subtract(window->inputRegion, x, y, width, height);

  /// Set the input region to the window
  GdkWindow *gdkWindow = gtk_widget_get_window(GTK_WIDGET(window->window));
  struct wl_surface *wlSurface = gdk_wayland_window_get_wl_surface(gdkWindow);

  wl_surface_set_input_region(wlSurface, window->inputRegion);
  wl_surface_commit(wlSurface);
}

/// get monitor list
FlValue *FLWM::WindowManager::getMonitorList() {
  GdkDisplay *display = gdk_display_get_default();
  if (!display) {
    std::cerr << "Failed to get default GDK display." << std::endl;
    return fl_value_new_list(); // Return empty list on error
  }

  g_autoptr(FlValue) result_list = fl_value_new_list();
  int n_monitors = gdk_display_get_n_monitors(display);

  for (int i = 0; i < n_monitors; i++) {
    GdkMonitor *monitor = gdk_display_get_monitor(display, i);
    if (monitor) {
      // Simple version like in wayland_layer_shell_plugin:
      const char *model = gdk_monitor_get_model(monitor);
      if (!model)
        model = "Unknown"; // Handle NULL model

      gchar *val = g_strdup_printf("%i:%s", i, model);
      fl_value_append_take(result_list, fl_value_new_string(val));
      g_free(val); // g_strdup_printf allocates, so we need to free
    }
  }
  return fl_value_ref(result_list); // Return a new reference
}

void FLWM::WindowManager::setMonitor(int monitor_index) {
  if (!this->window || !this->window->window) {
    std::cerr << "FLWM::WindowManager::setMonitor: Window is not valid."
              << std::endl;
    return;
  }

  GtkWindow *gtk_window = this->window->window;

  // Ensure the window is a layer surface. If not, this call might not have the
  // intended effect or could be an error depending on gtk-layer-shell's
  // behavior. flwm usually calls gtk_layer_init_for_window via convertToLayer
  // or in createWindow.
  if (!gtk_layer_is_layer_window(gtk_window)) {
    std::cerr << "FLWM::WindowManager::setMonitor: Window is not a layer shell "
                 "window. Call convertToLayer() first."
              << std::endl;
    // Optionally, you could try to call gtk_layer_init_for_window(gtk_window);
    // here if desired, but it's better if the Dart side ensures the window is
    // already a layer. gtk_layer_init_for_window(gtk_window); // Uncomment if
    // you want to auto-initialize
  }

  GdkDisplay *display = gtk_widget_get_display(
      GTK_WIDGET(gtk_window)); // Get display associated with the window
  if (!display) {
    std::cerr << "FLWM::WindowManager::setMonitor: Failed to get GDK display "
                 "for window."
              << std::endl;
    return;
  }

  if (monitor_index == -1) { // Special value to unset or use default
    gtk_layer_set_monitor(gtk_window, NULL);
    std::cout << "FLWM::WindowManager::setMonitor: Unset monitor for window ID "
              << this->window->id << std::endl;
  } else {
    int n_monitors = gdk_display_get_n_monitors(display);
    if (monitor_index < 0 || monitor_index >= n_monitors) {
      std::cerr << "FLWM::WindowManager::setMonitor: Invalid monitor index "
                << monitor_index << ". Available monitors: " << n_monitors
                << std::endl;
      return;
    }
    GdkMonitor *monitor = gdk_display_get_monitor(display, monitor_index);
    if (!monitor) {
      std::cerr << "FLWM::WindowManager::setMonitor: Failed to get GdkMonitor "
                   "for index "
                << monitor_index << std::endl;
      return;
    }
    gtk_layer_set_monitor(gtk_window, monitor);
    std::cout << "FLWM::WindowManager::setMonitor: Set monitor "
              << monitor_index << " for window ID " << this->window->id
              << std::endl;
  }
}