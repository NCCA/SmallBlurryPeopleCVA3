#ifndef __UI_HPP__
#define __UI_HPP__

#include <vector>
#include "ngl/Singleton.h"
#include "Button.hpp"

class Ui : public ngl::Singleton<Ui>
{
  friend class Singleton<Ui>;
public :
  ///
  /// \brief init initialises or resets ui
  ///
  void init();
  ///
  /// \brief click check if user has clicked inside any button area
  /// \param pos mouse position to compare buttons against
  ///
  void click();
  ///
  /// \brief mousePos update mouse position
  /// \param _pos current mouse position
  ///
  void mousePos(ngl::Vec2 _pos);
  ///
  /// \brief wipeButtons used to clear buttons, and button id counter is reset to 0
  ///
  void wipeButtons();

  void createTestButtons();

  void addButton(double _x, double _y, double _w, double _h);
private :
  ///
  /// \brief Ui private default constructor due to singleton pattern
  ///
  Ui();
  ///
  /// \brief m_buttons vector of buttons contained in the ui
  ///
  std::vector<Button> m_buttons;
  ///
  /// \brief m_selected_button_id current button that mouse is over
  ///
  int m_selected_button_id;
};

#endif//__UI_HPP__
