#include "Commands.hpp"
#include "Gui.hpp"
#include "MapList.hpp"
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

BuildCommand::BuildCommand(Character *_character, TileType _building) :
  m_character(_character),
  m_building(_building)
{}

void BuildCommand::execute()
{
	m_character->buildState(m_building);
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
	m_character->setForageState();
}

//------------------------------------------------------------//
// Eat Berries Command
//------------------------------------------------------------//

EatBerriesCommand::EatBerriesCommand(Character *_character) :
	m_character(_character)
{}

void EatBerriesCommand::execute()
{
	m_character->eatBerriesState();
}

//------------------------------------------------------------//
// Eat Fish Command
//------------------------------------------------------------//

EatFishCommand::EatFishCommand(Character *_character) :
	m_character(_character)
{}

void EatFishCommand::execute()
{
	m_character->eatFishState();
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
  m_scene->focusCamToGridPos(m_map_pos);
}

//------------------------------------------------------------//
// Save Preferences Command
//------------------------------------------------------------//

SavePreferencesCommand::SavePreferencesCommand()
{}

void SavePreferencesCommand::execute()
{
  Gui::instance()->notify("Preferences Saved!", ngl::Vec2(0,0));
  Prefs::instance()->savePrefs();
}

//------------------------------------------------------------//
// Change Map Command
//------------------------------------------------------------//

ChangeMapCommand::ChangeMapCommand(int _dir) :
  m_dir(_dir)
{}

void ChangeMapCommand::execute()
{
  if(m_dir > 0)
  {
    MapList::instance()->nextMap();
  }
  else
  {
    MapList::instance()->prevMap();
  }
  Gui::instance()->mapChanged();
}

//------------------------------------------------------------//
// Change Width Command
//------------------------------------------------------------//

ChangeWidthCommand::ChangeWidthCommand(int _dir) :
  m_dir(_dir)
{}

void ChangeWidthCommand::execute()
{
  MapList::instance()->addWidth(m_dir);
  Gui::instance()->mapChanged();
}

//------------------------------------------------------------//
// Change Height Command
//------------------------------------------------------------//

ChangeHeightCommand::ChangeHeightCommand(int _dir) :
  m_dir(_dir)
{}

void ChangeHeightCommand::execute()
{
  MapList::instance()->addHeight(m_dir);
  Gui::instance()->mapChanged();
}

//------------------------------------------------------------//
// Change Seed Command
//------------------------------------------------------------//

ChangeSeedCommand::ChangeSeedCommand(int _dir) :
  m_dir(_dir)
{}

void ChangeSeedCommand::execute()
{
  MapList::instance()->addSeed(m_dir);
  Gui::instance()->mapChanged();
}
