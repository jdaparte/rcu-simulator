
#include "keyboard.h"

#include "TimerHandle.h"
#include "EpollServer.h"
#include "Logger.h"
#include "routine.h"

#include <signal.h>
#include <thread>
#include <vector>
#include <fstream>
#include <sstream>

#define DEFAULT_DEVICE "/dev/input/event1"

TimerHandle *timerHandle1 = nullptr;
EpollServer *epollServer = nullptr;
Keyboard    *keyboard = nullptr;
Routine     *routine = nullptr;

bool keepRunning = true;

void intHandler(int dummy)
{
  keepRunning = false;
}

void StartEpollServer()
{
  epollServer->Start();
}

static void tick(void *timerHd)
{
  const Instruction& instruction = routine->getNextInstruction();
  LOGGER->LOG(1, LOGLEVEL_INFO, "Key %d event: %s", instruction._key, instruction._description.c_str());
  keyboard->event(instruction._key);
  auto timeout = instruction._wait;
  timerHandle1->SetTimeout(timeout, false);
}

int main(int argc, char* argv[])
{
  printf("\033[0;33m ********************************************************************* \033[0m\n");
  printf("\033[0;33m **                      RCU Events Simulator                       ** \033[0m\n");
  printf("\033[0;33m **  tick Period is 2 seconds (minimum time between 2 key events)   ** \033[0m\n");
  printf("\033[0;33m ********************************************************************* \033[0m\n");

  signal(SIGINT, intHandler);
  LOGGER->SetLogLevel(LOGLEVEL_ALL, 1);

  if (argc != 2) {

    LOGGER->LOG(1, LOGLEVEL_ERROR, "Invalid instructions file, please specify one.");
    LOGGER->LOG(1, LOGLEVEL_ERROR, "Usage example: RcuSimulator instructions");
    return 0;
  }

  routine = new Routine(std::string(argv[1]));

  const std::string device = DEFAULT_DEVICE;//std::string(argv[2]).empty() ? DEFAULT_DEVICE : std::string(argv[2]);

  epollServer = new EpollServer();
  keyboard = new Keyboard();
  timerHandle1 = new TimerHandle();

  timespec timeOut2Seconds{1,0};

  timerHandle1->RegisterCallback(tick);
  timerHandle1->SetTimeout(timeOut2Seconds, false);
  epollServer->Add(timerHandle1);

  if(keyboard->init(device.c_str())) {
    LOGGER->LOG(1, LOGLEVEL_ERROR, "init keyboard failed");
    keepRunning = false;
  }
  
  std::thread t([] { StartEpollServer(); });
  t.detach();

  while (keepRunning)
  {
    sleep(1);
  }
  
  LOGGER->LOG(1, LOGLEVEL_DEBUG, "Stop program");
  epollServer->Stop();

  if (routine)      delete routine;
  if (timerHandle1) delete timerHandle1;
  if (keyboard)     delete keyboard;
  if (epollServer)  delete epollServer;
  
  return 0;
}
