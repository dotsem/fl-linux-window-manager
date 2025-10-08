import 'package:fl_linux_window_manager/fl_linux_window_manager.dart';
import 'package:flutter/cupertino.dart';

class InputRegionController {
  /// A list of global keys attached to the InputRegion widgets.
  ///
  /// The widgets attched to this key are representing positive input regions,
  /// that is we should enable inputs in this region.
  static final List<GlobalKey> _positiveRegionKeys = [];

  /// A list of global keys attached to the InputRegion widgets.
  ///
  /// The widgets attched to this key are representing negative input regions,
  /// that is we should disable inputs in this region.
  static final List<GlobalKey> _negativeRegionKeys = [];

  /// The window ID whose input region will be updated by this controller.
  static String _windowId = 'main';

  /// Configure the controller to target the input region of the given window.
  ///
  /// Each Flutter window runs in its own isolate, so invoking this from the
  /// respective window isolate will scope all subsequent input region updates
  /// to that window only.
  static void configure({required String windowId}) {
    _windowId = windowId;
  }

  /// Adds a global key to the list of keys.
  static void addKey(
    GlobalKey key, {
    bool isNegative = false,
  }) {
    if (isNegative) {
      _negativeRegionKeys.add(key);
    } else {
      _positiveRegionKeys.add(key);
    }
  }

  /// Removes a global key from the list of keys.
  static void removeKey(GlobalKey key, {bool isNegative = false}) {
    if (isNegative) {
      _negativeRegionKeys.remove(key);
    } else {
      _positiveRegionKeys.remove(key);
    }
  }

  /// This will be called whenever there is a change in the InputRegion widget.
  /// like size change, position change, etc.
  ///
  /// This will change the native window input region to the new input region.
  static void refreshInputRegion() {
    /// Sort the keys based on the depth of the widget.
    final List<({GlobalKey key, bool isNegative})> keys = [
      ..._positiveRegionKeys.map((key) => (key: key, isNegative: false)),
      ..._negativeRegionKeys.map((key) => (key: key, isNegative: true)),
    ];
    keys.sort((a, b) => _findDepth(a.key).compareTo(_findDepth(b.key)));

    for (final item in keys) {
      /// Get the size and position of the widget.
      final RenderBox renderBox =
          item.key.currentContext!.findRenderObject() as RenderBox;
      final size = renderBox.size;
      final position = renderBox.localToGlobal(Offset.zero);

      /// Set the input region to the size and position of the widget.
      final Rect region = Rect.fromLTWH(
        position.dx,
        position.dy,
        size.width,
        size.height,
      );

      if (item.isNegative) {
        FlLinuxWindowManager.instance
            .subtractInputRegion(inputRegion: region, windowId: _windowId);
      } else {
        FlLinuxWindowManager.instance
            .addInputRegion(inputRegion: region, windowId: _windowId);
      }
    }
  }

  /// Find the depth of the InputRegion widget with the given key.
  static int _findDepth(GlobalKey key) {
    /// If the key is not attached to any widget, then return -1.
    if (key.currentContext == null || !key.currentContext!.mounted) {
      return -1;
    }

    Element element = key.currentContext! as Element;
    return element.depth;
  }
}
