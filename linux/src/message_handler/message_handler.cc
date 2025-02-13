#include <iostream>

#include <message_handler/message_handler.h>
#include <window_manager/window_manager.h>
#include <message_handler/method_call_arg_utils.h>
#include <message_handler/method_response_utils.h>

void messageHandler(
    FlMethodChannel* channel,
    FlMethodCall* methodCall,
    gpointer userData) {
    try {
        /// The method name that needs to be called. This is passed from the dart code.
        /// We need to compare this string and call the required methods.
        const char* methodName = fl_method_call_get_name(methodCall);

        /// The ID of the window that needs to be used for performing the action
        const char* windowId = FLWM::MethodCallArgUtils::getString(methodCall, "windowId");
        FLWM::WindowManager manager(windowId);

        if (strcmp(methodName, "createWindow") == 0) {
            std::string title = FLWM::MethodCallArgUtils::getString(methodCall, "title");
            unsigned int width = FLWM::MethodCallArgUtils::getInt(methodCall, "width");
            unsigned int height = FLWM::MethodCallArgUtils::getInt(methodCall, "height");
            bool isLayer = FLWM::MethodCallArgUtils::getBool(methodCall, "isLayer");
            std::vector<std::string> args = FLWM::MethodCallArgUtils::getStringList(methodCall, "args");

            manager.createWindow(windowId, title, width, height, isLayer, args);
        }
        else if (strcmp(methodName, "setLayer") == 0) {
            FLWM::Layer layer = (FLWM::Layer)FLWM::MethodCallArgUtils::getInt(methodCall, "layer");

            manager.setLayer(layer);
        }
        else if (strcmp(methodName, "setSize") == 0) {
            unsigned int width = FLWM::MethodCallArgUtils::getInt(methodCall, "width");
            unsigned int height = FLWM::MethodCallArgUtils::getInt(methodCall, "height");

            manager.setSize(width, height);
        }
        else if (strcmp(methodName, "setTitle") == 0) {
            std::string title = FLWM::MethodCallArgUtils::getString(methodCall, "title");

            manager.setTitle(title);
        }
        else if (strcmp(methodName, "setLayerMargin") == 0) {
            unsigned int top = FLWM::MethodCallArgUtils::getInt(methodCall, "top");
            unsigned int right = FLWM::MethodCallArgUtils::getInt(methodCall, "right");
            unsigned int bottom = FLWM::MethodCallArgUtils::getInt(methodCall, "bottom");
            unsigned int left = FLWM::MethodCallArgUtils::getInt(methodCall, "left");

            manager.setLayerMargin(top, right, bottom, left);
        }
        else if (strcmp(methodName, "setLayerAnchor") == 0) {
            int anchor = FLWM::MethodCallArgUtils::getInt(methodCall, "anchor");

            manager.setLayerAnchor(anchor);
        }
        else if (strcmp(methodName, "enableTransparency") == 0) {
            manager.enableTransparency();
        }
        else if (strcmp(methodName, "setIsDecorated") == 0) {
            bool isDecorated = FLWM::MethodCallArgUtils::getBool(methodCall, "isDecorated");

            manager.setIsDecorated(isDecorated);
        }
        else if (strcmp(methodName, "setKeyboardInteractivity") == 0) {
            FLWM::KeyboardInteractivity interactivity =
                (FLWM::KeyboardInteractivity)FLWM::MethodCallArgUtils::getInt(methodCall, "interactivity");

            manager.setKeyboardInteractivity(interactivity);
        }
        else if (strcmp(methodName, "enableLayerAutoExclusive") == 0) {
            manager.enableLayerAutoExclusive();
        }
        else if (strcmp(methodName, "setLayerExclusiveZone") == 0) {
            int length = FLWM::MethodCallArgUtils::getInt(methodCall, "length");

            manager.setLayerExclusiveZone(length);
        }
        else if (strcmp(methodName, "closeWindow") == 0) {
            manager.closeWindow();
        }
        else {
            std::cerr << "Method not implemented: " << methodName << std::endl;
            fl_method_call_respond(methodCall, FLWM::MethodResponseUtils::methodNotImplementedError(), NULL);
            return;
        }

        /// Respond to the method call with a success message.
        fl_method_call_respond(methodCall, FLWM::MethodResponseUtils::successResponse(), NULL);
    }

    catch (...) {
        std::cerr << "An error occurred in while handling method channel message" << std::endl;
        g_autoptr(FlMethodErrorResponse) error =
            fl_method_error_response_new("method_not_implemented", "Method not implemented", nullptr);
        fl_method_call_respond(methodCall, FL_METHOD_RESPONSE(error), nullptr);
    }
}