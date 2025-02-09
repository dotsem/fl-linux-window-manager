
import 'fl_linux_window_manager_platform_interface.dart';

class FlLinuxWindowManager {
  Future<String?> getPlatformVersion() {
    return FlLinuxWindowManagerPlatform.instance.getPlatformVersion();
  }
}
