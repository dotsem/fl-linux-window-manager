import 'package:flutter_test/flutter_test.dart';
import 'package:fl_linux_window_manager/fl_linux_window_manager.dart';
import 'package:fl_linux_window_manager/fl_linux_window_manager_platform_interface.dart';
import 'package:fl_linux_window_manager/fl_linux_window_manager_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockFlLinuxWindowManagerPlatform
    with MockPlatformInterfaceMixin
    implements FlLinuxWindowManagerPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final FlLinuxWindowManagerPlatform initialPlatform = FlLinuxWindowManagerPlatform.instance;

  test('$MethodChannelFlLinuxWindowManager is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelFlLinuxWindowManager>());
  });

  test('getPlatformVersion', () async {
    FlLinuxWindowManager flLinuxWindowManagerPlugin = FlLinuxWindowManager();
    MockFlLinuxWindowManagerPlatform fakePlatform = MockFlLinuxWindowManagerPlatform();
    FlLinuxWindowManagerPlatform.instance = fakePlatform;

    expect(await flLinuxWindowManagerPlugin.getPlatformVersion(), '42');
  });
}
