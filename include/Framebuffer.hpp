#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <ngl/Types.h>

#include <unordered_map>
#include <vector>

///
/// @file Framebuffer.hpp
/// @brief header file for the Framebuffer class
///

///
/// @class Framebuffer
/// @brief The framebuffer class acts as a wrapper around an opengl FBO, in an effort to
/// make it as modular and self contained as possible. Contains the functionality to create and
/// attach render targets, and draw to them.
///

class Framebuffer
{
public:
    /// @brief Destroys the framebuffer and all associated textures.
    ~Framebuffer();

    /// @brief Initialises the framebuffer to a given width and height. All textures added later will conform to these dimensions.
    /// @param [in] _w, The width of the framebuffer. All texture targets will conform to this.
    /// @param [in] _h, The height of the framebuffer. All texture targets will conform to this.
    void initialise(int _w, int _h);

    /// @brief Tells opengl to use all of the available colour attachments.
    void activeColourAttachments();

    /// @brief Tells opengl to use all of the specified colour attachments.
    /// @param [in] _bufs, The colour attachments to use.
    void activeColourAttachments(const std::vector<GLenum> _bufs);

    /// @brief Tells opengl to set a specific read attachment. This is used to blit parts of framebuffers to one another.
    /// @param [in] _buf, The attachment to use.
    void activeReadAttachment(const GLenum _buf);

    /// @brief Adds a pre-existing texture to the framebuffer with the given parameters.
    /// @param [in] _identifier, The string id.
    /// @param [in] _tex, The texture id.
    /// @param [in] _attachment, The attachment point.
    void addTexture(const std::string &_identifier, GLuint _tex, GLenum _attachment);

    /// @brief Creates and adds a texture to the framebuffer with the given parameters.
    /// @param [in] _identifier, The string id.
    /// @param [in] _format, The simplified format.
    /// @param [in] _iformat, The internal format.
    /// @param [in] _attachment, The attachment point.
    /// @param [in] _type, The data type, float by default.
    void addTexture(const std::string &_identifier, GLenum _format, GLenum _iformat, GLenum _attachment, GLint _type = GL_FLOAT);

    /// @brief Creates and adds a depth attachment to the framebuffer.
    /// @param [in] _identifier, The string id.
    void addDepthAttachment(const std::string &_identifier);

    /// @brief Binds the frambuffer for use (read and write).
    void bind();

    /// @brief Binds a texture for use in-shader.
    /// @param [in] _shaderID, The GLint id of the shader.
    /// @param [in] _tex, The string ID of the texture.
    /// @param [in] _uniform, The name of the uniform.
    /// @param [in] _target, The texture target number.
    void bindTexture(const GLint _shaderID, const std::string &_tex, const char *_uniform, int _target);

    /// @brief Checks that the famebuffer is complete. Should be checked before using.
    /// @return complete, Whether the framebuffer has been setup correctly.
    bool checkComplete();

    /// @brief Clears the entire framebuffer.
    void clear();

    /// @brief Unbinds the framebuffer.
    void unbind();

    /// @brief Gets the texture with the supplied ID.
    /// @param [in] _id, The string ID to search for.
    /// @return m_textures[_id], The texture with the supplied ID.
    GLuint get(const std::string _id) {return m_textures[_id];}

    /// @brief Returns the ID of the framebuffer.
    /// @return m_framebuffer, The GLuint ID of the framebuffer.
    GLuint getID() {return m_framebuffer;}
private:
    /// @brief Generates a texture, given a set of parameters.
    /// @param [in] _width, The width of the texture. Should always match the width of the framebuffer.
    /// @param [in] _height, The height of the texture. Should always match the height of the framebuffer.
    /// @param [in] _format, The simple format of the texture, such as GL_RGB
    /// @param [in] _internalFormat, The internal format of the texture, such as GL_RGB8.
    /// @param [in] _type, The data type of the texture, should normally be GL_FLOAT.
    GLuint genTexture(int _width, int _height, GLint _format, GLint _internalFormat, GLint _type);

    /// @brief Used to track the maximum colour attachments available on the current machine.
    GLenum m_maxColourTarget;

    /// @brief Used to track the active colour attachments.
    std::vector<GLenum> m_colorAttachments;

    /// @brief The framebuffer object. This class acts as its wrapper.
    GLuint m_framebuffer;

    /// @brief The height of the framebuffer.
    int m_h;

    /// @brief The width of the framebuffer.
    int m_w;

    /// @brief Keeps track of textures. Maps a string ID to a texture ID.
    std::unordered_map< std::string, GLuint > m_textures;
};
#endif
