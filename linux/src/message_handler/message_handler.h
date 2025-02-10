#pragma once

#include <flutter_linux/flutter_linux.h>

/**
 * A callback that will be called when a method is called from the Flutter side.
 * This is the callback that will be registered with the FlutterMethodChannel.
 *
 * Here the parameter channel represents the FlMethodChannel* that is used to communicate with the Flutter side.
 * The parameter methodCall represents the FlMethodCall* that contains the method name and arguments.
 *
 * The parameter userData is the user data that was passed when the callback was registered.
 * (In this case it will be NULL).
 */
void messageHandler(FlMethodChannel* channel, FlMethodCall* methodCall,
    gpointer userData);