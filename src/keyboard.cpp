#include <fcntl.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <iostream>

#include "keyboard.h"
#include "Logger.h"


Keyboard::~Keyboard()
{
  libevdev_uinput_destroy(_uidev);
  libevdev_free(_dev);
  close(_fd);
}

int16_t Keyboard::init(const char *devicePath)
{
  LOGGER->LOG(1, LOGLEVEL_INFO, "Keyboard init %s", devicePath);
  if ((_fd = open(devicePath, O_WRONLY | O_NONBLOCK)) < 0)
  {
    LOGGER->LOG(1, LOGLEVEL_WARNING, "Error creating FD to %s", devicePath);
    return -1;
  }

  if (libevdev_new_from_fd(_fd, &_dev) < 0)
  {
    LOGGER->LOG(1, LOGLEVEL_WARNING, "Error initializing libevdev on FD: %d", _fd);
    return -1;
  }

  if (libevdev_uinput_create_from_device(_dev,
                                         LIBEVDEV_UINPUT_OPEN_MANAGED,
                                         &_uidev) < 0)
  {
    LOGGER->LOG(1, LOGLEVEL_WARNING, "Error libevdev device");
    return -1;
  }

  return 0;
}

void Keyboard::event(const Instruction instruction) {
  event(instruction._key, instruction._description);
}

void Keyboard::event(const std::string keyString, const std::string description, const EventType et)
{
  try
  {
    const int key = Keys.at(keyString);
    event(key, description, et);
  }
  catch(const std::exception& e)
  {
    LOGGER->LOG(1, LOGLEVEL_ERROR, "%s, Invalid key %s, skipped", e.what(), keyString.c_str());
  }
}

void Keyboard::event(const int key, const std::string description, const EventType et)
{
  LOGGER->LOG(1, LOGLEVEL_INFO, "Key %d event: %s", key, description.c_str());

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
