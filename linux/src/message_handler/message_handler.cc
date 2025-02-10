#include <message_handler/message_handler.h>

#include <iostream>

void messageHandler(
    FlMethodChannel* channel,
    FlMethodCall* methodCall,
    gpointer userData) {
    try {

    }
    catch (...) {
        std::cerr << "An error occurred in while handling method channel message" << std::endl;
        g_autoptr(FlMethodErrorResponse) error =
            fl_method_error_response_new("method_not_implemented", "Method not implemented", nullptr);
        fl_method_call_respond(methodCall, FL_METHOD_RESPONSE(error), nullptr);
    }
}