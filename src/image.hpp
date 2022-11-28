#pragma once

#include <string>

/**
 * Loads an image from a file path
 */
class Image
{
    public:
    Image(std::string path);

    ~Image();

    const unsigned char *getData();
    int getWidth();
    int getHeight();
    int getChannels();

    private:
    int width, height, channels;
    unsigned char *data;

    bool hasLoaded();
};
