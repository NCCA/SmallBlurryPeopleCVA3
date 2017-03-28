#include "Commands.hpp"
#include "Gui.hpp"
//------------------------------------------------------------//
// Passive Command
//------------------------------------------------------------//

PassiveCommand::PassiveCommand()
{}

void PassiveCommand::execute()
{}

//------------------------------------------------------------//
// Quit Command
//------------------------------------------------------------//

QuitCommand::QuitCommand(Scene *_scene) :
  m_scene(_scene)
{}

void QuitCommand::execute()
{
  m_scene->quit();
}

//------------------------------------------------------------//
// Build Command
//------------------------------------------------------------//

BuildCommand::BuildCommand(Character *_character, BuildingType _building) :
  m_character(_character),
  m_building(_building)
{}

void BuildCommand::execute()
{
  m_character->build(m_building);
}

//------------------------------------------------------------//
// Centre Camera Command
//------------------------------------------------------------//

CentreCameraCommand::CentreCameraCommand(Scene *_scene) :
  m_scene(_scene)
{}


void CentreCameraCommand::execute()
{
  m_scene->centreCamera();
}

//------------------------------------------------------------//
// Pause Command
//------------------------------------------------------------//

EscapeCommand::EscapeCommand(Scene *_scene) :
  m_scene(_scene)
{}

void EscapeCommand::execute()
{
  m_scene->escapeState();
}

//------------------------------------------------------------//
// Prefs Command
//------------------------------------------------------------//

PrefsCommand::PrefsCommand(Scene *_scene) :
  m_scene(_scene)
{}

void PrefsCommand::execute()
{
  m_scene->prefsMode();
}

//------------------------------------------------------------//
// Zoom Command
//------------------------------------------------------------//

ZoomCommand::ZoomCommand(Scene *_scene, int _direction) :
  m_scene(_scene),
  m_direction(_direction)
{}

void ZoomCommand::execute()
{
  m_scene->zoom(m_direction);
}

//------------------------------------------------------------//
// Move Command
//------------------------------------------------------------//

MoveCommand::MoveCommand(Scene *_scene, Direction _d, bool _stop) :
  m_scene(_scene),
  m_d(_d),
  m_stop(_stop)
{}

void MoveCommand::execute()
{
  if(m_stop)
    m_scene->stopMove(m_d);
  else
    m_scene->startMove(m_d);
}

//------------------------------------------------------------//
// Set Prefs Command
//------------------------------------------------------------//

SetPrefsCommand::SetPrefsCommand(const std::string &_pref_name) :
  m_pref_name(_pref_name)
{}

void SetPrefsCommand::execute()
{
  //Prefs::instance()->setIntPref(m_pref_name, m_val);
}
