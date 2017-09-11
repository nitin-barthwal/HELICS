#include "logger.h"
#include <iostream>

namespace utilities
{
logger::~logger ()
{
    if (loggingThread.joinable ())
    {
        loggingQueue.emplace ("!!!close");
        loggingThread.join ();
    }
}
void logger::openFile (const std::string &file)
{
    if (loggingThread.joinable ())
    {
        loggingQueue.emplace ("!!!close");
        loggingThread.join ();
        outFile.open (file.c_str ());
        startLogging ();
    }
    else
    {
        outFile.open (file.c_str ());
    }
}

void logger::startLogging (int cLevel, int fLevel)
{
    if (loggingThread.joinable ())
    {
        return;
    }
    consoleLevel = cLevel;
    fileLevel = fLevel;
    loggingThread = std::thread (&logger::loggerLoop, this);
}

void logger::changeLevels (int cLevel, int fLevel)
{
    consoleLevel = cLevel;
    fileLevel = fLevel;
}

void logger::log (int level, std::string logMessage)
{
    logMessage.push_back ((level <= consoleLevel) ? 'c' : 'n');
    logMessage.push_back ((level <= fileLevel) ? 'f' : 'n');
    loggingQueue.emplace (std::move (logMessage));
}

void logger::flush () { loggingQueue.emplace ("!!!flush"); }
bool logger::isRunning () const { return loggingThread.joinable (); }


void logger::loggerLoop ()
{
    while (true)
    {
        auto msg = loggingQueue.pop ();

		if (msg.size()<3)
		{
			continue;
		}
            if (msg.front () == '!')
            {
                if (msg.compare (3, 5, "close") == 0)
                {
                    break;  // break the loop
                }
                if (msg.compare (3, 5, "flush") == 0)
                {
                    if (outFile.is_open ())
                    {
                        outFile.flush ();
                    }
                    std::cout.flush ();
                    continue;
                }
            }
            // if a the file should be written there will be a 'f' at the end
            auto f = msg.back ();
            msg.pop_back ();
            // if a the console should be written there will be a 'c' at the end
            auto c = msg.back ();
            msg.pop_back ();
            if (f == 'f')
            {
                if (outFile.is_open ())
                {
                    outFile << msg << '\n';
                }
            }
            if (c == 'c')
            {
                std::cout << msg << '\n';
            }
      
    }
}

loggerNoThread::loggerNoThread () = default;

void loggerNoThread::openFile (const std::string &file) { outFile.open (file.c_str ()); }
void loggerNoThread::startLogging (int cLevel, int fLevel)
{
    consoleLevel = cLevel;
    fileLevel = fLevel;
}

void loggerNoThread::changeLevels (int cLevel, int fLevel)
{
    consoleLevel = cLevel;
    fileLevel = fLevel;
}

void loggerNoThread::log (int level, const std::string &logMessage)
{
    if (level < consoleLevel)
    {
        std::cout << logMessage << '\n';
    }
    if (level < fileLevel)
    {
        if (outFile.is_open ())
        {
            outFile << logMessage << '\n';
        }
    }
}

void loggerNoThread::flush ()
{
    if (outFile.is_open ())
    {
        outFile.flush ();
    }
    std::cout.flush ();
}

bool loggerNoThread::isRunning () const { return true; }
}  // namespace utilities