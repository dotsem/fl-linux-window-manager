import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'fl_linux_window_manager_method_channel.dart';

abstract class FlLinuxWindowManagerPlatform extends PlatformInterface {
  /// Constructs a FlLinuxWindowManagerPlatform.
  FlLinuxWindowManagerPlatform() : super(token: _token);

  static final Object _token = Object();

  static FlLinuxWindowManagerPlatform _instance = MethodChannelFlLinuxWindowManager();

  /// The default instance of [FlLinuxWindowManagerPlatform] to use.
  ///
  /// Defaults to [MethodChannelFlLinuxWindowManager].
  static FlLinuxWindowManagerPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [FlLinuxWindowManagerPlatform] when
  /// they register themselves.
  static set instance(FlLinuxWindowManagerPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
