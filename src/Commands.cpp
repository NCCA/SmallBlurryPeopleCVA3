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

PauseCommand::PauseCommand(Scene *_scene) :
  m_scene(_scene)
{}

void PauseCommand::execute()
{
  m_scene->togglePause();
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
