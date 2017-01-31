#include <iostream>
#include <string>

#include "Framebuffer.hpp"

void Framebuffer::initialise(int _w, int _h)
{
    m_w = _w;
    m_h = _h;

    glGenFramebuffers(1, &m_Framebuffer);
    bind();

    GLint numColAttachments = 0;
    glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &numColAttachments );
    m_maxColourTarget = numColAttachments + GL_COLOR_ATTACHMENT0;
}

void Framebuffer::activeColourAttachments(const std::vector<GLenum> _bufs)
{
    for(auto &buf : _bufs)
    {
        if(buf > m_maxColourTarget)
        {
            std::cerr <<  "Error! Attempting to use colour target " << std::to_string((int)buf) << "that is not available on this system (max is " << std::to_string((int)m_maxColourTarget) << ").\n";
            exit(EXIT_FAILURE);
        }
    }
    glDrawBuffers(_bufs.size(), &_bufs[0]);
}

void Framebuffer::addDepthAttachment(const std::string &_identifier)
{
    GLuint depth;
    glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, m_w, m_h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    std::pair<std::string, GLuint> tex ( _identifier, depth );
    m_textures.insert( tex );
}

void Framebuffer::addTexture(const std::string &_identifier, GLenum _format, GLenum _iformat , GLenum _attachment)
{
    //Create texture.
    std::pair<std::string, GLuint> tex ( _identifier, genTexture(m_w, m_h, _format, _iformat) );
    m_textures.insert( tex );

    glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, GL_TEXTURE_2D, m_textures[ _identifier ], 0);
}

void Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
}

void Framebuffer::bindTexture(const GLint _shaderID, const std::string &_tex, const char *_uniform, int _target)
{
    GLint loc = glGetUniformLocation(_shaderID, _uniform);

    if(loc == -1)
        std::cerr << "Uh oh! Invalid uniform location in Framebuffer::bindTexture!! " << _uniform << '\n';

    glUniform1i(loc, _target);

    glActiveTexture(GL_TEXTURE0 + _target);
    glBindTexture(GL_TEXTURE_2D, m_textures[_tex]);
}

bool Framebuffer::checkComplete()
{
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

GLuint Framebuffer::genTexture(int _width, int _height, GLint _format, GLint _internalFormat)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _format, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return tex;
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
