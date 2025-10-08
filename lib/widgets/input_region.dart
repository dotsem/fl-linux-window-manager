import 'package:fl_linux_window_manager/controller/input_region_controller.dart';
import 'package:flutter/cupertino.dart';

class InputRegion extends StatefulWidget {
  final Widget child;
  final bool _isNegative;
  final String? windowId;

  const InputRegion({super.key, required this.child, this.windowId})
      : _isNegative = false;
  const InputRegion.negative({super.key, required this.child, this.windowId})
      : _isNegative = true;

  @override
  State<InputRegion> createState() => _InputRegionState();
}

class _InputRegionState extends State<InputRegion> {
  late final GlobalKey _key;

  @override
  void initState() {
    super.initState();

    /// Setup a new global key for this widget.
    _key = GlobalKey();

    if (widget.windowId != null) {
      InputRegionController.configure(windowId: widget.windowId!);
    }

    InputRegionController.addKey(
      _key,
      isNegative: widget._isNegative,
    );

    /// Get the initial details of the widget after the initial frame.
    /// and update the input region.
    WidgetsBinding.instance.addPostFrameCallback((_) {
      InputRegionController.refreshInputRegion();
    });
  }

  @override
  void dispose() {
    /// Remove the input region key from the controller.
    InputRegionController.removeKey(
      _key,
      isNegative: widget._isNegative,
    );
    InputRegionController.refreshInputRegion();

    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return NotificationListener<SizeChangedLayoutNotification>(
      onNotification: (notification) {
        /// Update the input region when the size changes
        WidgetsBinding.instance.addPostFrameCallback((_) {
          InputRegionController.refreshInputRegion();
        });

        return false;
      },
      child: SizeChangedLayoutNotifier(
        key: _key,
        child: widget.child,
      ),
    );
  }
}
