#pragma once

#include <string>
#include <glad/glad.h>

namespace orc
{
    class Texture
    {
        public:
        Texture();

        ~Texture();

        // Disable copy construction and assignment to avoid destruction of managed 
        // OpenGL objects
        Texture(const Texture &other) = delete;
        void operator=(const Texture &other) = delete;

        virtual void Use() = 0;

        unsigned int GetId() const;

        // This is a temporary measure to allow sorted rendering of objects. In
        // the future, the material system will drive part of the sort order,
        // and the position of each object will drive the other part.
        virtual int64_t GetRenderSortKey() const = 0;

        protected:
        void Bind(GLenum target);

        private:
        unsigned int id;
    };

    class TextureRef
    {
        public:
        virtual Texture &Load() = 0;

        virtual ~TextureRef() = default;
    };
}
