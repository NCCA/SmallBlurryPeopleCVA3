#include "Commands.hpp"

//------------------------------------------------------------//
// Passive Command
//------------------------------------------------------------//

PassiveCommand::PassiveCommand(int x)
{x+= 1;}

PassiveCommand::~PassiveCommand()
{}

void PassiveCommand::execute()
{}

//------------------------------------------------------------//
// Quit Command
//------------------------------------------------------------//

QuitCommand::QuitCommand(Scene *_scene) :
  m_scene(_scene)
{}

QuitCommand::~QuitCommand()
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

BuildCommand::~BuildCommand()
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

CentreCameraCommand::~CentreCameraCommand()
{}

void CentreCameraCommand::execute()
{
  m_scene->centreCamera();
}
