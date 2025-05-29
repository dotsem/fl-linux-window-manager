#pragma once

#include <flutter_linux/flutter_linux.h>

namespace FLWM
{
    /**
     * A utility class used to send a response back to the flutter code.
     *
     * A response can be a success response or an error response. This class allows
     * to create both types of responses.
     */
    class MethodResponseUtils
    {
    public:
        /**
         * @brief Create a method not implemented error back to the flutter code.
         *
         * @return FlMethodResponse* the error response that needs to be sent back to the flutter code.
         */
        static FlMethodResponse *methodNotImplementedError();

        /**
         * @brief Create a NULL success response back to the flutter code.
         *
         * @return FlMethodResponse* the response with the given value that needs to be
         * sent back to the flutter code.
         */
        static FlMethodResponse *successResponse();

        /**
         * @brief Create a INT success response back to the flutter code.
         *
         * @param value  The result that needs to be sent back to the flutter code.
         * @return FlMethodResponse* the response with the given value that needs to be
         * sent back to the flutter code.
         */
        static FlMethodResponse *successResponse(int value);

        /**
         * @brief Create a STRING success response back to the flutter code.
         *
         * @param value  The result that needs to be sent back to the flutter code.
         * @return FlMethodResponse* the response with the given value that needs to be
         * sent back to the flutter code.
         */
        static FlMethodResponse *successResponse(const char *value);

        /**
         * @brief Create a DOUBLE success response back to the flutter code.
         *
         * @param value  The result that needs to be sent back to the flutter code.
         * @return FlMethodResponse* the response with the given value that needs to be
         * sent back to the flutter code.
         */
        static FlMethodResponse *successResponse(double value);

        /**
         * @brief Create a BOOL success response back to the flutter code.
         *
         * @param value  The result that needs to be sent back to the flutter code.
         * @return FlMethodResponse* the response with the given value that needs to be
         * sent back to the flutter code.
         */
        static FlMethodResponse *successResponse(bool value);

        static FlMethodResponse *successResponse(FlValue *value);
        };
}