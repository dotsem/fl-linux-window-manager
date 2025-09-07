#include <iostream>

#include <message_handler/message_handler.h>
#include <message_handler/method_call_arg_utils.h>
#include <message_handler/method_response_utils.h>
#include <window_manager/window_manager.h>

struct SharedChannelHandlerData {
  /// The window ID to whcich the method call needs to be forwarded.
  std::string forwardWindowId;

  /// The channel name that needs to be used for forwarding the method call.
  std::string channelName;
};

/**
 * A proxy method handler that forwards the method call to the given window.
 */
void sharedMethodChannelHandler(FlMethodChannel *channel,
                                FlMethodCall *methodCall, gpointer userData) {
  try {
    const char *methodName = fl_method_call_get_name(methodCall);
    SharedChannelHandlerData *handlerData =
        (SharedChannelHandlerData *)userData;

    /// Forward the method call into the given window
    FLWM::WindowManager manager(handlerData->forwardWindowId);
    manager.sendMethodCall(handlerData->channelName, methodName,
                           fl_method_call_get_args(methodCall));

    fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
  } catch (...) {
    std::cerr
        << "An error occurred in while handling shared method channel message"
        << std::endl;
    g_autoptr(FlMethodErrorResponse) error = fl_method_error_response_new(
        "method_not_implemented", "Method not implemented", nullptr);
    fl_method_call_respond(methodCall, FL_METHOD_RESPONSE(error), nullptr);
  }
}

void messageHandler(FlMethodChannel *channel, FlMethodCall *methodCall,
                    gpointer userData) {
  try {
    /// The method name that needs to be called. This is passed from the dart
    /// code. We need to compare this string and call the required methods.
    const char *methodName = fl_method_call_get_name(methodCall);

    /// The ID of the window that needs to be used for performing the action
    const char *windowId =
        FLWM::MethodCallArgUtils::getString(methodCall, "windowId");

    if (strcmp(methodName, "createSharedMethodChannel") == 0) {
      std::string channelName =
          FLWM::MethodCallArgUtils::getString(methodCall, "channelName");
      std::string shareWithWindowId =
          FLWM::MethodCallArgUtils::getString(methodCall, "shareWithWindowId");

      SharedChannelHandlerData *destHandlerData =
          new SharedChannelHandlerData();
      destHandlerData->forwardWindowId = shareWithWindowId;
      destHandlerData->channelName = channelName;

      SharedChannelHandlerData *srcHandlerData = new SharedChannelHandlerData();
      srcHandlerData->forwardWindowId = std::string(windowId);
      srcHandlerData->channelName = channelName;

      FLWM::WindowManager managerSrc(windowId);
      FLWM::WindowManager managerDest(shareWithWindowId);

      managerSrc.createMethodChannel(channelName, sharedMethodChannelHandler,
                                     destHandlerData);
      managerDest.createMethodChannel(channelName, sharedMethodChannelHandler,
                                      srcHandlerData);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "createWindow") == 0) {
      std::string title =
          FLWM::MethodCallArgUtils::getString(methodCall, "title");
      unsigned int width =
          FLWM::MethodCallArgUtils::getInt(methodCall, "width");
      unsigned int height =
          FLWM::MethodCallArgUtils::getInt(methodCall, "height");
      bool isLayer = FLWM::MethodCallArgUtils::getBool(methodCall, "isLayer");
      std::vector<std::string> args =
          FLWM::MethodCallArgUtils::getStringList(methodCall, "args");

      FLWM::WindowManager::createWindow(windowId, title, width, height, isLayer,
                                        args);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "setLayer") == 0) {
      FLWM::WindowManager manager(windowId);
      FLWM::Layer layer =
          (FLWM::Layer)FLWM::MethodCallArgUtils::getInt(methodCall, "layer");

      manager.setLayer(layer);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "setSize") == 0) {
      FLWM::WindowManager manager(windowId);
      unsigned int width =
          FLWM::MethodCallArgUtils::getInt(methodCall, "width");
      unsigned int height =
          FLWM::MethodCallArgUtils::getInt(methodCall, "height");

      manager.setSize(width, height);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "setTitle") == 0) {
      FLWM::WindowManager manager(windowId);
      std::string title =
          FLWM::MethodCallArgUtils::getString(methodCall, "title");

      manager.setTitle(title);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "setLayerMargin") == 0) {
      FLWM::WindowManager manager(windowId);
      unsigned int top = FLWM::MethodCallArgUtils::getInt(methodCall, "top");
      unsigned int right =
          FLWM::MethodCallArgUtils::getInt(methodCall, "right");
      unsigned int bottom =
          FLWM::MethodCallArgUtils::getInt(methodCall, "bottom");
      unsigned int left = FLWM::MethodCallArgUtils::getInt(methodCall, "left");

      manager.setLayerMargin(top, right, bottom, left);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "setLayerAnchor") == 0) {
      FLWM::WindowManager manager(windowId);
      int anchor = FLWM::MethodCallArgUtils::getInt(methodCall, "anchor");

      manager.setLayerAnchor(anchor);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "enableTransparency") == 0) {
      FLWM::WindowManager manager(windowId);
      manager.enableTransparency();
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "setIsDecorated") == 0) {
      FLWM::WindowManager manager(windowId);
      bool isDecorated =
          FLWM::MethodCallArgUtils::getBool(methodCall, "isDecorated");

      manager.setIsDecorated(isDecorated);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "setKeyboardInteractivity") == 0) {
      FLWM::WindowManager manager(windowId);
      FLWM::KeyboardInteractivity interactivity =
          (FLWM::KeyboardInteractivity)FLWM::MethodCallArgUtils::getInt(
              methodCall, "interactivity");

      manager.setKeyboardInteractivity(interactivity);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "enableLayerAutoExclusive") == 0) {
      FLWM::WindowManager manager(windowId);
      manager.enableLayerAutoExclusive();
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "setLayerExclusiveZone") == 0) {
      FLWM::WindowManager manager(windowId);
      int length = FLWM::MethodCallArgUtils::getInt(methodCall, "length");

      manager.setLayerExclusiveZone(length);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "closeWindow") == 0) {
      FLWM::WindowManager manager(windowId);
      manager.closeWindow();
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "hideWindow") == 0) {
      FLWM::WindowManager manager(windowId);
      manager.hideWindow();
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "showWindow") == 0) {
      FLWM::WindowManager manager(windowId);
      manager.showWindow();
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "setFocus") == 0) {
      FLWM::WindowManager manager(windowId);
      manager.setFocus();
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    }
    else if (strcmp(methodName, "isVisible") == 0) {
      FlValue *result = nullptr;
      FlMethodResponse *response = nullptr;

      try {
        FLWM::WindowManager manager(windowId);
        result = manager.isVisible();
        response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
      } catch (...) {
        response = FL_METHOD_RESPONSE(fl_method_error_response_new(
            "WINDOW_ERROR", "Failed to check visibility", nullptr));
      }

      if (response != nullptr) {
        fl_method_call_respond(methodCall, response, nullptr);
        g_object_unref(response);
      }
      return;
    } else if (strcmp(methodName, "setInfinteInputRegion") == 0) {
      FLWM::WindowManager manager(windowId);
      manager.setInfinteInputRegion();
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "addInputRegion") == 0) {
      FLWM::WindowManager manager(windowId);
      int x = FLWM::MethodCallArgUtils::getInt(methodCall, "x");
      int y = FLWM::MethodCallArgUtils::getInt(methodCall, "y");
      int width = FLWM::MethodCallArgUtils::getInt(methodCall, "width");
      int height = FLWM::MethodCallArgUtils::getInt(methodCall, "height");

      manager.addInputRegion(x, y, width, height);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "subtractInputRegion") == 0) {
      FLWM::WindowManager manager(windowId);
      int x = FLWM::MethodCallArgUtils::getInt(methodCall, "x");
      int y = FLWM::MethodCallArgUtils::getInt(methodCall, "y");
      int width = FLWM::MethodCallArgUtils::getInt(methodCall, "width");
      int height = FLWM::MethodCallArgUtils::getInt(methodCall, "height");

      manager.subtractInputRegion(x, y, width, height);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else if (strcmp(methodName, "getMonitorList") == 0) {
      FLWM::WindowManager manager(windowId);
      g_autoptr(FlValue) monitors_value = manager.getMonitorList();

      fl_method_call_respond(
          methodCall,
          FLWM::MethodResponseUtils::successResponse(monitors_value), NULL);
      return;
    } else if (strcmp(methodName, "setMonitor") == 0) {
      FLWM::WindowManager manager(windowId);
      int monitor_id_arg =
          FLWM::MethodCallArgUtils::getInt(methodCall, "monitorId");
      manager.setMonitor(monitor_id_arg);
      
      fl_method_call_respond(methodCall,
                           FLWM::MethodResponseUtils::successResponse(), NULL);
      return;
    } else {
      std::cerr << "Method not implemented: " << methodName << std::endl;
      fl_method_call_respond(
          methodCall, FLWM::MethodResponseUtils::methodNotImplementedError(),
          NULL);
      return;
    }
  }
  catch (...) {
    std::cerr << "An error occurred while handling method channel message"
              << std::endl;
    g_autoptr(FlMethodErrorResponse) error = fl_method_error_response_new(
        "method_not_implemented", "Method not implemented", nullptr);
    fl_method_call_respond(methodCall, FL_METHOD_RESPONSE(error), nullptr);
  }
}