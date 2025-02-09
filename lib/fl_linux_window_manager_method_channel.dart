import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'fl_linux_window_manager_platform_interface.dart';

/// An implementation of [FlLinuxWindowManagerPlatform] that uses method channels.
class MethodChannelFlLinuxWindowManager extends FlLinuxWindowManagerPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('fl_linux_window_manager');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
