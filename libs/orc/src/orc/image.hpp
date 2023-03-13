#pragma once

#include <string>

namespace orc
{
    /**
     * Loads an image from a file path
     */
    class Image
    {
        public:
        Image(std::string path);

        ~Image();

        // Disable copy construction and assignment to avoid copying entire images
        Image(const Image &other) = delete;
        void operator=(const Image &other) = delete;

        const unsigned char *GetData();
        int GetWidth();
        int GetHeight();
        int GetChannels();

        private:
        int width, height, channels;
        unsigned char *data;
    };
}
