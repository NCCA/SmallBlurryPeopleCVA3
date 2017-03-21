#ifndef __GUI_HPP__
#define __GUI_HPP__

#include <vector>
#include <memory>
#include "ngl/Singleton.h"
#include "ngl/Vec2.h"
#include "Button.hpp"
#include "Commands.hpp"
#include "Scene.hpp"

///
/// \brief The Gui class contains button positions and managing their use
///
class Gui : public ngl::Singleton<Gui>
{
  friend class Singleton<Gui>;
public :
  ///
  /// \brief init initialise or reset Gui with specific resolution
  /// \param _res resolution to initialise with
  /// \param _shader_name name of gui shader
  ///
  void init(Scene *_scene, ngl::Vec2 _res, const std::string &_shader_name);
  ///
  /// \brief setResolution set the window width and window height
  ///
  void setResolution(ngl::Vec2 _res);
  ///
  /// \brief initGL initialise gl values
  ///
  void initGL();
  ///
  /// \brief click check if user has clicked inside any button area
  /// \param pos mouse position to compare buttons against
  ///
  void click();
  ///
  /// \brief generateCommand create a command to be executed
  /// \return command to be executed
  ///
  std::shared_ptr<Command> generateCommand(Action _action);
  ///
  /// \brief executeAction generate a command and execute it
  /// \param _action action to execute
  /// \return 0 for no errors, 1 for nothing executed
  ///
  int executeAction(Action _action);
  ///
  /// \brief mousePos update mouse position
  /// \param _pos current mouse position
  ///
  bool mousePos(ngl::Vec2 _pos);
  ///
  /// \brief wipeButtons used to clear buttons, and button id counter is reset to 0
  ///
  void wipeButtons();
  ///
  /// \brief createSceneButtons creates the set of default buttons for the scene
  ///
  void createSceneButtons();
  ///
  /// \brief createPauseButtons create the set of buttons for the pause menu
  ///
  void createPauseButtons();





  void addButton(Action _action, XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _offset, ngl::Vec2 _size);

  ///
  /// \brief updateButtonArrays updates positions and passes them to openGL, useful for changing buttons or resizing screen
  ///
  void updateButtonArrays();
  ///
  /// \brief drawButtons draw buttons to screen
  ///
  void drawButtons();
  ///
  /// \brief mouseDown register mouse as clicked
  ///
  void mouseDown();
  ///
  /// \brief mouseUp register mouse as not clicked
  ///
  void mouseUp();
  ///
  /// \brief bindTextureToShader bind the given texture to the button shader
  /// \param _tex texture id to use
  /// \param _uniform name of texture in shader
  /// \param _target which number texture unit to use
  ///
  void bindTextureToShader(const GLuint _tex, const char *_uniform, int _target);


private :
  ///
  /// \brief Gui private default constructor due to singleton pattern
  ///
  Gui();
  ///
  /// \brief m_scene pointer to scene
  ///
  Scene *m_scene;
  ///
  /// \brief m_buttons vector of buttons contained in the Gui
  ///
  std::vector<Button> m_buttons;
  ///
  /// \brief m_selected_button_id current button that mouse is over
  ///
  int m_selected_button_id;
  ///
  /// \brief m_win_w width of window
  ///
  int m_win_w;
  ///
  /// \brief m_win_h height of window
  ///
  int m_win_h;
  ///
  /// \brief m_shader_name string denoting shader that buttons use
  ///
  std::string m_shader_name;
  ///
  /// \brief m_vao_id geometry VAO for button points
  ///
  GLuint m_vao_id;
  ///
  /// \brief m_vbo_ids VBOs for various button attributes
  ///
  GLuint m_vbo_ids[4];
  ///
  /// \brief m_mouse_down whether mouse is pressed down or not
  ///
  bool m_mouse_down;
  ///
  /// \brief m_active_character pointer to currently selected character
  ///
  Character *m_active_character;
};

#endif//__GUI_HPP__
