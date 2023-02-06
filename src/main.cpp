
#include "keyboard.h"
#include "routine.h"

#include "TimerHandle.h"
#include "EpollServer.h"
#include "Logger.h"

#include <signal.h>
#include <thread>
#include <vector>
#include <fstream>
#include <sstream>

TimerHandle *timerHandle1 = nullptr;
EpollServer *epollServer = nullptr;
Keyboard *keyboard = nullptr;
#define DEFAULT_DEVICE "/dev/input/event1"

/**
 * @brief Vector with instructions set to be repeted indefinitely.
 *    Instruction Structure:  
 *      - First field: Key to be pressed and released
 *      - Second field: Waiting time after presing the key (seconds)
 *      - Third field: Description to be printed in logs (optional)
 */
std::vector<Instruction> instructions {};

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
  static std::vector<Instruction>::const_iterator it {instructions.begin()};
  static int waitTime {0};

  if(it == instructions.end())
    it = instructions.begin();

  if (waitTime <= 2)
  {
    keyboard->event(*it.base());
    waitTime = it.base()->_wait;

    it++;
  }
  else
  {
    waitTime -= 2;
  }
}

int parseFile(std::string instructionsFileName)
{
  std::ifstream fileStream;
  fileStream.open(instructionsFileName.c_str(), std::ifstream::in);
  if (!fileStream.is_open())
  {
    LOGGER->LOG(1, LOGLEVEL_ERROR, "The file %s cannot be opened", instructionsFileName.c_str());
    return 1;
  }

  std::string line;
  while (std::getline(fileStream, line, '\n'))
  {
    if (line[0] != '#' && line[0] != ' ' && line[0] != '\n') {   // Skip comments or empty lines
      
      std::istringstream lineStream(line);
      std::string key{}, time{}, description{};
      if (std::getline(lineStream, key, ' ')) {
        std::getline(lineStream, time, ' ');
        std::getline(lineStream, description);
        try
        {
          int keyValue = Keys.at(key);
          int timeValue = atoi(time.c_str());
          instructions.emplace_back(keyValue, timeValue, description);
        }
        catch(const std::exception& e)
        {
          LOGGER->LOG(1, LOGLEVEL_ERROR, "%s, Invalid instruction %s, skipped", e.what(), key.c_str());
        }
      }
    }
  }

  return 0;
}

int main(int argc, char* argv[])
{
  printf("\033[0;33m ********************************************************************* \033[0m\n");
  printf("\033[0;33m **                      RCU Events Simulator                       ** \033[0m\n");
  printf("\033[0;33m **  tick Period is 2 seconds (minimum time between 2 key events)   ** \033[0m\n");
  printf("\033[0;33m ********************************************************************* \033[0m\n");

  signal(SIGINT, intHandler);
  LOGGER->SetLogLevel(LOGLEVEL_ALL, 1);

  if (argc <= 2) {

    LOGGER->LOG(1, LOGLEVEL_ERROR, "Invalid instructions file, please specify one.");
    LOGGER->LOG(1, LOGLEVEL_ERROR, "Usage example: RcuSimulator instructions");
    return 0;
  }

  if (parseFile(std::string(argv[1])) == -1)
    LOGGER->LOG(1, LOGLEVEL_ERROR, "Error parsing instructions file");

  std::string device = (std::string(argv[1]).empty() ? DEFAULT_DEVICE : std::string(argv[1]);

  epollServer = new EpollServer();
  keyboard = new Keyboard();
  if(!keyboard->init(device)) {
    LOGGER->LOG(1, LOGLEVEL_ERROR, "openUInputDevice failed");
    delete keyboard;
  }

  timespec timeOut2Seconds;
  timeOut2Seconds.tv_sec = 2;
  timeOut2Seconds.tv_nsec = 0;

  timerHandle1 = new TimerHandle();
  timerHandle1->RegisterCallback(tick);
  timerHandle1->SetTimeout(timeOut2Seconds, true);
  epollServer->Add(timerHandle1);

  std::thread t([] { StartEpollServer(); });
  t.detach();

  while (keepRunning)
  {
    sleep(1);
  }
  
  LOGGER->LOG(1, LOGLEVEL_DEBUG, "Stop program");

  epollServer->Stop();

  delete timerHandle1;
  delete keyboard;
  delete epollServer;
  return 0;
}
