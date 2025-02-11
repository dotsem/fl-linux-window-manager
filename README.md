# Flutter Linux Window Manager (FLWM)

A flutter plugin that can be used to manage windows in linux platform. The plugin's main intension is make flutter suitable for the development of shells for linux window managers (e.g: GNOME Shell).

All necessary features needed to create panels and other shell components using flutter are provided in this plugin.

# Usage

1. To use this plugin, first add this plugin to your application dependencies.
2. Open the `my_application.cc` from your flutter app's `linux` folder
3. Import the required header files at the top of this file

```cpp
#include <fl_linux_window_manager/fl_linux_window_manager_plugin.h>
```

4. If you want to make the initial window of the app as a wayland layer, then add the code given below just before the `gtk_widget_show(GTK_WIDGET(window));` line.

```cpp
FLWM::WindowManager::convertToLayer(GTK_WINDOW(window));
```

5. For all other property changes for the main window, create new window, etc. you can use the Dart API of this plugin.
