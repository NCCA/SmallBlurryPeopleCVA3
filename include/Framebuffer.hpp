#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <ngl/Types.h>

#include <unordered_map>
#include <vector>

class Framebuffer
{
public:
    /// @brief Destroys the framebuffer and all associated textures.
    ~Framebuffer();
    /// @brief Initialises the framebuffer to a given width and height. All textures added later will conform to these dimensions.
    void initialise(int _w, int _h);
    void activeColourAttachments();
    void activeColourAttachments(const std::vector<GLenum> _bufs);
    void activeReadAttachment(const GLenum _buf);
    void addTexture(const std::string &_identifier, GLuint _tex, GLenum _attachment);
    void addRenderbufferMultisampled(const std::string &_identifier, GLuint _tex, GLenum _attachment);
    void addTexture(const std::string &_identifier, GLenum _format, GLenum _iformat, GLenum _attachment, GLint _type = GL_FLOAT);
    void addDepthAttachment(const std::string &_identifier);
    void bind();
    void bindTexture(const GLint _shaderID, const std::string &_tex, const char *_uniform, int _target);
    bool checkComplete();
    void clear();
    void unbind();
    GLuint get(const std::string _id) {return m_textures[_id];}
    GLuint getID() {return m_framebuffer;}
private:
    GLuint genTexture(int _width, int _height, GLint _format, GLint _internalFormat, GLint _type);
    //Keeps track of textures.
    std::unordered_map< std::string, GLuint > m_textures;
    //Keeps track of the colour attachments that each texture uses.
    std::unordered_map< std::string, GLenum > m_attachments;
    GLuint m_framebuffer;
    int m_h;
    GLenum m_maxColourTarget;
    int m_w;
    std::vector<GLenum> m_colorAttachments;
};
#endif
