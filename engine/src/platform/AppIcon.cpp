#include "platform/AppIcon.h"

#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace ScrapGameEngine
{
    namespace
    {
        /** The sizes Windows and Linux desktops actually request. */
        constexpr int kIconSizes[] = {16, 24, 32, 48, 64, 128};

        struct Image
        {
            int width = 0;
            int height = 0;
            std::vector<unsigned char> pixels;   // RGBA8
        };

        /**
         * Box-filters the source into a square of `size`, preserving aspect ratio and
         * padding with transparency. Stretching a logo to square would distort it, and
         * cropping would cut it off, so letterboxing is the only option that keeps it
         * recognisable.
         */
        Image resizeSquare(const unsigned char* src, int srcW, int srcH, int size)
        {
            Image out;
            out.width = size;
            out.height = size;
            out.pixels.assign(static_cast<size_t>(size) * size * 4, 0);

            const float scale = std::min(static_cast<float>(size) / static_cast<float>(srcW),
                                         static_cast<float>(size) / static_cast<float>(srcH));
            const int drawW = std::max(1, static_cast<int>(std::lround(srcW * scale)));
            const int drawH = std::max(1, static_cast<int>(std::lround(srcH * scale)));
            const int offsetX = (size - drawW) / 2;
            const int offsetY = (size - drawH) / 2;

            for (int y = 0; y < drawH; y++)
            {
                // Source span this destination row averages over.
                const int sy0 = y * srcH / drawH;
                const int sy1 = std::max(sy0 + 1, (y + 1) * srcH / drawH);

                for (int x = 0; x < drawW; x++)
                {
                    const int sx0 = x * srcW / drawW;
                    const int sx1 = std::max(sx0 + 1, (x + 1) * srcW / drawW);

                    int accum[4] = {0, 0, 0, 0};
                    int samples = 0;
                    for (int sy = sy0; sy < sy1; sy++)
                    {
                        for (int sx = sx0; sx < sx1; sx++)
                        {
                            const size_t i = (static_cast<size_t>(sy) * srcW + sx) * 4;
                            for (int c = 0; c < 4; c++) accum[c] += src[i + c];
                            samples++;
                        }
                    }
                    if (samples == 0) continue;

                    const size_t d = ((static_cast<size_t>(offsetY + y) * size) + offsetX + x) * 4;
                    for (int c = 0; c < 4; c++)
                    {
                        out.pixels[d + c] = static_cast<unsigned char>(accum[c] / samples);
                    }
                }
            }
            return out;
        }
    }

    std::string AppIcon::locateBrandingIcon()
    {
        const fs::path candidates[] = {
            "resources/ScrapEngine.png",
            "../resources/ScrapEngine.png",
            "../../resources/ScrapEngine.png",
            "../../../resources/ScrapEngine.png",
        };

        std::error_code ec;
        for (const auto& candidate : candidates)
        {
            if (fs::exists(candidate, ec)) return candidate.string();
        }
        return {};
    }

    bool AppIcon::setFromPng(GLFWwindow* window, const std::string& pngPath)
    {
        if (window == nullptr || pngPath.empty()) return false;

        int width = 0, height = 0, channels = 0;
        // Forced to 4 channels: GLFW's GLFWimage is RGBA8 with no stride, so anything
        // else would need converting here anyway.
        unsigned char* data = stbi_load(pngPath.c_str(), &width, &height, &channels, 4);
        if (data == nullptr)
        {
            std::cerr << "[ICON] could not load " << pngPath << ": "
                      << stbi_failure_reason() << std::endl;
            return false;
        }

        std::vector<Image> images;
        images.reserve(std::size(kIconSizes));
        for (int size : kIconSizes)
        {
            // No point upscaling past the source; it only wastes memory.
            if (size > std::max(width, height) && !images.empty()) break;
            images.push_back(resizeSquare(data, width, height, size));
        }
        stbi_image_free(data);

        if (images.empty()) return false;

        std::vector<GLFWimage> glfwImages;
        glfwImages.reserve(images.size());
        for (auto& image : images)
        {
            glfwImages.push_back({image.width, image.height, image.pixels.data()});
        }

        // GLFW picks the closest size per request, so handing it the whole set means
        // the 16px title bar and the 48px alt-tab each get something purpose-made.
        glfwSetWindowIcon(window, static_cast<int>(glfwImages.size()), glfwImages.data());
        return true;
    }
}
