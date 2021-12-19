#include <fcntl.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

#include "keyboard.h"
#include "Logger.h"

#include <iostream>

Instruction::Instruction(int key, int wait)
: _key{key}, _wait{wait}
{};


Keyboard::Keyboard(const std::string device)
{
  if (openUInputDevice(device.c_str()) < 0)
  {
    LOGGER->LOG(1, LOGLEVEL_ERROR, "openUInputDevice failed");
  }
  sleep(3);
}

Keyboard::~Keyboard()
{
  libevdev_uinput_destroy(_uidev);
  libevdev_free(_dev);
  close(_fd);
}

int16_t Keyboard::openUInputDevice(const char *devicePath)
{
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

void Keyboard::event(int key, EventType et)
{
  LOGGER->LOG(1, LOGLEVEL_INFO, "Key %d event", key);

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
