#include <fcntl.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "keyboard.h"
#include "Logger.h"

#define BOXINFO_MANUFACTURERNAME "/boxinfo/ManufacturerName"
#define DEV_INPUT_ARRIS          "/dev/input/event1"
#define DEV_INPUT_HUMAX          "/dev/input/event1"
#define DEV_INPUT_SKYWORTH       "/dev/input/event3"
#define HUMAX                    "HUMAX"
#define ARRIS                    "ARRIS"
#define SKYWORTH                 "SKYWORTH"

Keyboard::~Keyboard()
{
  libevdev_uinput_destroy(_uidev);
  libevdev_free(_dev);
  close(_fd);
}

void Keyboard::selectInputDevice() 
{
  std::ifstream fileStream;
  std::string manufacturerName;
  fileStream.open(BOXINFO_MANUFACTURERNAME, std::ifstream::in);

  if (fileStream.is_open()) {
    std::getline(fileStream, manufacturerName);
    fileStream.close();

    if (manufacturerName == ARRIS)         _device = DEV_INPUT_ARRIS;
    else if (manufacturerName == HUMAX)    _device = DEV_INPUT_HUMAX;
    else if (manufacturerName == SKYWORTH) _device = DEV_INPUT_SKYWORTH;

    LOGGER->LOG(1, LOGLEVEL_INFO, "Manufacturer %s, using %s", manufacturerName.c_str(), _device.c_str());
  }
  else { 
    _device = DEV_INPUT_ARRIS;
    LOGGER->LOG(1, LOGLEVEL_INFO, "Manufacturer not supported, try open %s dev.", _device.c_str());
  }
}

int16_t Keyboard::init(std::string dev)
{
  if (!dev.empty()) 
  {
    _device = dev;
  }
  else
  {
    selectInputDevice();
  }

  if ((_fd = open(_device.c_str(), O_WRONLY | O_NONBLOCK)) < 0)
  {
    LOGGER->LOG(1, LOGLEVEL_WARNING, "Error creating FD to %s, check if is a valid device.", _device.c_str());
    return -1;
  }
  sleep(1);

  if (libevdev_new_from_fd(_fd, &_dev) < 0)
  {
    LOGGER->LOG(1, LOGLEVEL_WARNING, "Error initializing libevdev on FD: %d", _fd);
    return -1;
  }
  sleep(1);

  if (libevdev_uinput_create_from_device(_dev,
                                         LIBEVDEV_UINPUT_OPEN_MANAGED,
                                         &_uidev) < 0)
  {
    LOGGER->LOG(1, LOGLEVEL_WARNING, "Error libevdev device");
    return -1;
  }
  LOGGER->LOG(1, LOGLEVEL_INFO, "Device %s opened. In case the keys are not inyected, the device can be specified as the second parameter.", _device.c_str());
  sleep(4);
  
  return 0;
}

void Keyboard::event(const std::string keyString, const EventType et)
{
  try
  {
    const int key = Keys.at(keyString);
    event(key, et);
  }
  catch(const std::exception& e)
  {
    LOGGER->LOG(1, LOGLEVEL_ERROR, "%s, Invalid key %s, skipped", e.what(), keyString.c_str());
  }
}

void Keyboard::event(const int key, const EventType et)
{
  if (et == EventType::PRESS || et == EventType::PRESSRELEASE)
  {
    libevdev_uinput_write_event(_uidev, EV_KEY, static_cast<uint32_t>(key), 1);
    libevdev_uinput_write_event(_uidev, EV_SYN, SYN_REPORT, 0);
  }

  if (et == EventType::RELEASE || et == EventType::PRESSRELEASE)
  {
    libevdev_uinput_write_event(_uidev, EV_KEY, static_cast<uint32_t>(key), 0);
    libevdev_uinput_write_event(_uidev, EV_SYN, SYN_REPORT, 0);
  }
}
