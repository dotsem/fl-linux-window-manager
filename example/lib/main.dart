import 'package:fl_linux_window_manager/fl_linux_window_manager.dart';
import 'package:flutter/material.dart';

import 'package:flutter/services.dart';

MethodChannel channel = MethodChannel('shared_example');
List<String> arggg = [];

void main(List<String> args) {
  WidgetsFlutterBinding.ensureInitialized();

  print("Args received: $args");
  arggg.addAll(args);

  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final String _platformVersion = 'Unknown';

  @override
  void initState() {
    super.initState();

    channel.setMethodCallHandler((call) async {
      print("Received call in main window: ${call.method}, Args: $arggg");
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Column(
            spacing: 10,
            mainAxisSize: MainAxisSize.min,
            children: [
              Text('Running on: $_platformVersion\n'),
              ElevatedButton(
                onPressed: () {
                  FlLinuxWindowManager.instance.createWindow(
                    windowId: "new_window",
                    title: "Sample",
                    width: 500,
                    height: 300,
                    args: ["--class=sample", "--name=sample"],
                  );
                },
                child: const Text('Create Window'),
              ),
              ElevatedButton(
                  onPressed: () {
                    FlLinuxWindowManager.instance
                        .hideWindow(windowId: "new_window");
                  },
                  child: const Text('Hide Window')),
              ElevatedButton(
                  onPressed: () {
                    FlLinuxWindowManager.instance
                        .showWindow(windowId: "new_window");
                  },
                  child: const Text('Show Window')),
              ElevatedButton(
                  onPressed: () async {
                    await FlLinuxWindowManager.instance
                        .createSharedMethodChannel(
                            channelName: "shared_example",
                            shareWithWindowId: "main",
                            windowId: "new_window");

                    channel.invokeMethod("SampleMethodName");
                  },
                  child: Text("Create shared message"))
            ],
          ),
        ),
      ),
    );
  }
}
