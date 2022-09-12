
#include "keyboard.h"

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

/**
 * @brief Vector with instructions set to be repeted indefinitely.
 *    Instruction Structure:  
 *      - First field: Key to be pressed and released
 *      - Second field: Waiting time after presing the key (seconds)
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
    keyboard->event(it.base()->_key);
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
  while (std::getline(fileStream, line))
  {
    std::istringstream lineStream(line);

    std::string key;
    if (std::getline(lineStream, key, ' '))
    {
      std::string time;
      std::getline(lineStream, time);

      int keyValue = Keys.at(key);
      int timeValue = atoi(time.c_str());
      instructions.emplace_back(keyValue, timeValue);
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

  if (argc != 2) {

    LOGGER->LOG(1, LOGLEVEL_ERROR, "Invalid instructions file, please specify one.");
    LOGGER->LOG(1, LOGLEVEL_ERROR, "Usage example: RcuSimulator instructions");
    return 0;
  }

  if (parseFile(std::string(argv[1])) == -1)
    LOGGER->LOG(1, LOGLEVEL_ERROR, "Error parsing file");

  epollServer = new EpollServer();
  keyboard = new Keyboard();

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
