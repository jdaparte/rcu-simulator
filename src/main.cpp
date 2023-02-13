
#include "keyboard.h"

#include "TimerHandle.h"
#include "EpollServer.h"
#include "Logger.h"

#include <signal.h>
#include <thread>
#include <vector>
#include <fstream>
#include <sstream>

#define DEFAULT_DEVICE "/dev/input/event1"
#define TICK_PERIOD 1

TimerHandle *timerHandle1 = nullptr;
EpollServer *epollServer = nullptr;
Keyboard    *keyboard = nullptr;



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
  static std::vector<Instruction>::iterator event {instructions.begin()};
  static std::uint64_t iteration {0};

  keyboard->event(*event.base());
  event++;
  
  if(event == instructions.end()) {
    iteration++;
    LOGGER->LOG(1, LOGLEVEL_INFO, "End of routine. Iteration %ld will begin.", iteration);
    event = instructions.begin();
  }

  timerHandle1->SetTimeout(event.base()->_wait, false);
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
          int seconds = atoi(time.c_str());
          instructions.emplace_back(keyValue, seconds, description);
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

  if (argc != 2) {

    LOGGER->LOG(1, LOGLEVEL_ERROR, "Invalid instructions file, please specify one.");
    LOGGER->LOG(1, LOGLEVEL_ERROR, "Usage example: RcuSimulator instructions");
    return 0;
  }

  if (parseFile(std::string(argv[1])) == -1)
    LOGGER->LOG(1, LOGLEVEL_ERROR, "Error parsing instructions file");

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

  if (timerHandle1) delete timerHandle1;
  if (keyboard)     delete keyboard;
  if (epollServer)  delete epollServer;
  
  return 0;
}
