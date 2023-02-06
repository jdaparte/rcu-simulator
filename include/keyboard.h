#pragma once

#include <string>
#include <map>

enum class EventType
{
  PRESS,
  RELEASE,
  PRESSRELEASE
};

static const std::map<std::string, int>  Keys
{
  {"MENU"      , 102},
  {"GUIDE"     , 365},
  {"UP"        , 103},
  {"LEFT"      , 105},
  {"RIGHT"     , 106},
  {"DOWN"      , 108},
  {"OK"        , 353},
  {"BACK"      , 158},
  {"INFO"      , 358},
  {"RED"       , 398},
  {"GREEN"     , 399},
  {"YELLOW"    , 400},
  {"BLUE"      , 401},
  {"DVR"       , 354},
  {"REC"       , 167},
  {"MOVISTAR"  , 627},
  {"REWIND"    , 168},
  {"PLAYPAUSE" , 164},
  {"FF"        , 159},
  {"NEXT"      , 407}, 
  {"STOP"      , 128},
  {"PREVIOUS"  , 412},
  {"VOL_UP"    , 115},
  {"VOL_DOWN"  , 114},
  {"SEARCH"    , 227},
  {"MUTE"      , 113},
  {"CH_UP"     , 402},
  {"CH_DOWN"   , 403},
  {"K_1"       ,   2},
  {"K_2"       ,   3},
  {"K_3"       ,   4},
  {"K_4"       ,   5},
  {"K_5"       ,   6},
  {"K_6"       ,   7},
  {"K_7"       ,   8},
  {"K_8"       ,   9},
  {"K_9"       ,  10},
  {"K_0"       ,  11},
  {"DELETE"    , 111},
  {"FAV"       , 364},
  {"VIEW"      , 226},
  {"SUB"       , 370},
  {"AUDIO"     , 392},
  {"EXIT"      , 174}
};

class Keyboard
{
private:
  int16_t _fd;
  struct libevdev *_dev;
  struct libevdev_uinput *_uidev;

public:
  ~Keyboard();
  int16_t init(const char *devicePath);

  void event(Instruction instruction);
  void event(int key, std::string description, EventType et = EventType::PRESSRELEASE);
};