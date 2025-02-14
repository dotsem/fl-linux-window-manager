import 'package:fl_linux_window_manager/models/keyboard_mode.dart';
import 'package:fl_linux_window_manager/models/layer.dart';
import 'package:flutter/services.dart';

class FlLinuxWindowManager {
  /// The ID of the main window of the app.
  static const String _mainWindowId = 'main';

  /// A counter to the window IDs to keep track of the windows.
  static int _windowIdCounter = 1;

  /// The method channel used to communicate with the platform side.
  final MethodChannel _methodChannel =
      const MethodChannel('fl_linux_window_manager');

  /// Private constructor
  FlLinuxWindowManager._();

  /// The single instance of the class
  static final FlLinuxWindowManager _instance = FlLinuxWindowManager._();

  /// Getter for the single instance of the class
  static FlLinuxWindowManager get instance => _instance;

  /// Create a window with the given title, width, and height.
  ///
  /// The [title] is the title of the window.
  /// The [width] is the width of the window.
  /// The [height] is the height of the window.
  /// The [isLayer] is a flag to indicate if the window is a layer window.
  /// The [windowId] is the ID of the window. If not provided a unique ID will be generated
  /// and returned as the result.
  ///
  /// Returns a future with the window ID of the created window.
  Future<String> createWindow({
    required String title,
    required int width,
    required int height,
    bool isLayer = false,
    List<String> args = const [],
    String? windowId,
  }) async {
    /// Setup the window ID for the new window
    windowId ??= 'window_$_windowIdCounter';

    /// Increment the window ID counter
    _windowIdCounter++;

    await _methodChannel.invokeMethod('createWindow', {
      'title': title,
      'width': width,
      'height': height,
      'isLayer': isLayer,
      'args': args,
      'windowId': windowId,
    });

    return windowId;
  }

  /// Set the layer of the window with the given window ID.
  ///
  /// The [layer] is the layer to set the window to.
  /// The [windowId] is the ID of the window.
  Future<void> setLayer(
    WindowLayer layer, {
    String windowId = _mainWindowId,
  }) {
    return _methodChannel.invokeMethod('setLayer', {
      'layer': layer.layerId,
      'windowId': windowId,
    });
  }

  /// Set the size of the window with the given window ID.
  ///
  /// The [width] is the width of the window.
  /// The [height] is the height of the window.
  /// The [windowId] is the ID of the window.
  Future<void> setSize({
    required int width,
    required int height,
    String windowId = _mainWindowId,
  }) {
    return _methodChannel.invokeMethod('setSize', {
      'width': width,
      'height': height,
      'windowId': windowId,
    });
  }

  /// Set the title of the window with the given window ID.
  ///
  /// The [title] is the title of the window.
  /// The [windowId] is the ID of the window.
  Future<void> setTitle({
    required String title,
    String windowId = _mainWindowId,
  }) {
    return _methodChannel.invokeMethod('setTitle', {
      'title': title,
      'windowId': windowId,
    });
  }

  /// Set the margin for the layer with the given window ID.
  ///
  /// The [left] is the left margin of the layer.
  /// The [top] is the top margin of the layer.
  /// The [right] is the right margin of the layer.
  /// The [bottom] is the bottom margin of the layer.
  /// The [windowId] is the ID of the window.
  Future<void> setLayerMargin({
    int left = 0,
    int top = 0,
    int right = 0,
    int bottom = 0,
    String windowId = _mainWindowId,
  }) {
    return _methodChannel.invokeMethod('setLayerMargin', {
      'left': left,
      'top': top,
      'right': right,
      'bottom': bottom,
      'windowId': windowId,
    });
  }

  /// Set the anchor for the layer with the given window ID.
  ///
  /// The [anchor] is the anchor for the layer. (This must be an integer representation of the ScreenEdge enum)
  /// We can use multiple anchors by using the bitwise OR operator on the ScreenEdge enum values.
  ///
  /// If set to 0, then all anchors will be cleared.
  ///
  /// The [windowId] is the ID of the window.
  Future<void> setLayerAnchor({
    int anchor = 0,
    String windowId = _mainWindowId,
  }) {
    return _methodChannel.invokeMethod('setLayerAnchor', {
      'anchor': anchor,
      'windowId': windowId,
    });
  }

  /// Enable transparency for the window with the given window ID.
  Future<void> enableTransparency({String windowId = _mainWindowId}) {
    return _methodChannel.invokeMethod('enableTransparency', {
      'windowId': windowId,
    });
  }

  /// Set if the window with the given window ID is decorated.
  /// The decoration includes the title bar and the window controls like the close button, minimize button, etc.
  ///
  /// The [isDecorated] is a flag to indicate if the window is decorated.
  Future<void> setIsDecorated({
    required bool isDecorated,
    String windowId = _mainWindowId,
  }) {
    return _methodChannel.invokeMethod('setIsDecorated', {
      'isDecorated': isDecorated,
      'windowId': windowId,
    });
  }

  /// Set the keyboard interactivity for the window with the given window ID.
  ///
  /// The [mode] is the keyboard interactivity mode to set.
  /// The [windowId] is the ID of the window.
  Future<void> setKeyboardInteractivity(
    KeyboardMode mode, {
    String windowId = _mainWindowId,
  }) {
    return _methodChannel.invokeMethod('setKeyboardInteractivity', {
      'interactivity': mode.value,
      'windowId': windowId,
    });
  }

  /// Enable exclusive zone for the layer, with the given window ID.
  /// Exclusive zone size will be automatically calculated based on the layer size.
  ///
  /// The [windowId] is the ID of the window.
  Future<void> enableLayerAutoExclusive({String windowId = _mainWindowId}) {
    return _methodChannel.invokeMethod('enableLayerAutoExclusive', {
      'windowId': windowId,
    });
  }

  /// Set a manual exclusive zone for the layer, with the given window ID.
  ///
  /// The [value] is the length of the exclusive zone. (It has some more options, check the
  /// layer shell protocol docs for more information)
  ///
  /// The [windowId] is the ID of the window.
  Future<void> setLayerExclusiveZone(int value,
      {String windowId = _mainWindowId}) {
    return _methodChannel.invokeMethod('setLayerExclusiveZone', {
      'length': value,
      'windowId': windowId,
    });
  }

  /// Close the window with the given window ID.
  ///
  /// The [windowId] is the ID of the window.
  Future<void> closeWindow({String windowId = _mainWindowId}) {
    return _methodChannel.invokeMethod('closeWindow', {
      'windowId': windowId,
    });
  }

  /// Hides and window with the given window ID.
  ///
  /// The [windowId] is the ID of the window.
  Future<void> hideWindow({String windowId = _mainWindowId}) {
    return _methodChannel.invokeMethod('hideWindow', {
      'windowId': windowId,
    });
  }

  /// Shows the window with the given window ID.
  /// If the window is hidden, then it will be shown.
  ///
  /// The [windowId] is the ID of the window.
  Future<void> showWindow({String windowId = _mainWindowId}) {
    return _methodChannel.invokeMethod('showWindow', {
      'windowId': windowId,
    });
  }

  /// Creates a shared method channel with the given window, so that we an communicate with the window.
  /// The window must be created before calling this method.
  ///
  /// The [channelName] is the name of the new channel.
  /// The [shareWithWindowId] is the ID of the window to share the channel with.
  /// The [windowId] is the ID of the current window.
  Future<void> createSharedMethodChannel(
      {required String channelName,
      required String shareWithWindowId,
      String windowId = _mainWindowId}) {
    if (windowId == shareWithWindowId) {
      throw Exception('Cannot share a channel with the same window');
    }

    return _methodChannel.invokeMethod('createSharedMethodChannel', {
      'channelName': channelName,
      'shareWithWindowId': shareWithWindowId,
      'windowId': windowId,
    });
  }
}
