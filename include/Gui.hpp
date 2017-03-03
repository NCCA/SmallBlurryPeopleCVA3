#ifndef __GUI_HPP__
#define __GUI_HPP__

#include <vector>
#include "ngl/Singleton.h"
#include "ngl/Vec2.h"
#include "Button.hpp"

class Gui : public ngl::Singleton<Gui>
{
  friend class Singleton<Gui>;
public :
  ///
  /// \brief init initialise or reset Gui with specific resolution
  /// \param _res resolution to initialise with
  /// \param _shader_name name of gui shader
  ///
  void init(ngl::Vec2 _res, const std::string &_shader_name);
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
  /// \brief mousePos update mouse position
  /// \param _pos current mouse position
  ///
  bool mousePos(ngl::Vec2 _pos);
  ///
  /// \brief wipeButtons used to clear buttons, and button id counter is reset to 0
  ///
  void wipeButtons();
  ///
  /// \brief createTestButtons create a set of buttons for debugging
  ///
  void createTestButtons();




  void addButton(XAlignment _x_align, YAlignment _y_align, ngl::Vec2 _offset, ngl::Vec2 _size);

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

private :
  ///
  /// \brief Gui private default constructor due to singleton pattern
  ///
  Gui();
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
  GLuint m_vbo_ids[3];
  ///
  /// \brief m_mouse_down whether mouse is pressed down or not
  ///
  bool m_mouse_down;
};

#endif//__GUI_HPP__
