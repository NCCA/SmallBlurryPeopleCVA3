#include "Commands.hpp"

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
