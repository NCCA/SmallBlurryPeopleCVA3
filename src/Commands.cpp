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
// Escape Command
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
// Move Cam Command
//------------------------------------------------------------//

MoveCamCommand::MoveCamCommand(Scene *_scene, Direction _d, bool _stop) :
  m_scene(_scene),
  m_d(_d),
  m_stop(_stop)
{}

void MoveCamCommand::execute()
{
  if(m_stop)
    m_scene->stopMove(m_d);
  else
    m_scene->startMove(m_d);
}

//------------------------------------------------------------//
// Set Prefs Command
//------------------------------------------------------------//

// setPrefsCommand is a template, so is defined in the header

//------------------------------------------------------------//
// Forage Command
//------------------------------------------------------------//

ForageCommand::ForageCommand(Character *_character) :
  m_character(_character)
{}

void ForageCommand::execute()
{
  m_character->forage();
}

//------------------------------------------------------------//
// Centre Notification Command
//------------------------------------------------------------//

CentreNotificationCommand::CentreNotificationCommand(Scene *_scene, ngl::Vec2 _map_pos) :
  m_scene(_scene),
  m_map_pos(_map_pos)
{}

void CentreNotificationCommand::execute()
{
  m_scene->moveCamToPos(m_map_pos);
}
