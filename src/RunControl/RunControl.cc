/**
 * @file    RunControl.cc
 * @author  Adam O'Brien <obrienadam89@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This file contains the implementations for class RunControl.
 */

#include <sstream>
#include <boost/filesystem.hpp>

#include "RunControl.h"
#include "Output.h"

RunControl::RunControl()
    :
      itrs_(0),
      simTime_(0.),
      startRealTime_(boost::posix_time::microsec_clock::local_time()),
      maxElapsedRealTime_(boost::posix_time::hours(48)),
      terminationCondition_("iterations")
{

}

void RunControl::initialize(Input &input)
{
    terminationCondition_ = input.inputStrings["terminationCondition"];
    maxItrs_ = input.inputInts["maxItrs"];
    timeStep_ = input.inputDoubles["timeStep"];
    maxSimTime_ = input.inputDoubles["maxSimTime"];
    screenWriteInterval_ = input.inputInts["screenWriteInterval"];
    fileWriteInterval_ = input.inputInts["fileWriteInterval"];

    //- Create a directory for the solution output

    createDirectory("solution");
}

bool RunControl::continueRun()
{
    using namespace boost::posix_time;

    ++itrs_;
    simTime_ += timeStep_;
    elapsedRealTime_ = microsec_clock::local_time() - startRealTime_;

    if(terminationCondition_ == "iterations")
    {
        if(itrs_ >= maxItrs_)
            return false;
    }
    else if(terminationCondition_ == "simTime")
    {
        if(simTime_ >= maxSimTime_)
            return false;
    }
    else if (terminationCondition_ == "realTime")
    {
        if(elapsedRealTime_ >= maxElapsedRealTime_)
            return false;
    }
    else
    {
        Output::raiseException("RunControl", "continueRun", "invalid termination condition \"" + terminationCondition_ + "\" selected.");
    }

    return true;
}

bool RunControl::writeToScreen()
{
    if(itrs_%screenWriteInterval_ == 0)
        return true;

    return false;
}

bool RunControl::writeToFile()
{
    if(itrs_%fileWriteInterval_ == 0)
        return true;

    return false;
}

void RunControl::reset()
{
    Output::raiseException("RunControl", "reset", "Method not yet implemented.");
}

void RunControl::createDirectory(std::string directoryName)
{
    boost::filesystem::path dir(directoryName);

    if(!boost::filesystem::exists(dir))
    {
        if(!boost::filesystem::create_directory(dir))
            Output::raiseException("RunControl", "createDirectory", "Creation of directory \"" + directoryName + "\" failed.");
    }
}

void RunControl::displayStartMessage()
{
    using namespace std;

    ostringstream message;

    Output::printLine();

    message << "Beginning simulation. Terminating on condition: " << terminationCondition_ << "." << endl
            << "Iterations beginning on " << startRealTime_ << ".";

    Output::print(message.str());
}

void RunControl::displayUpdateMessage()
{
    using namespace std;
    using namespace boost::posix_time;

    ostringstream message;
    double completionPercentage;

    elapsedRealTime_ = microsec_clock::local_time() - startRealTime_;

    if(terminationCondition_ == "iterations")
    {
        completionPercentage = double(itrs_)/double(maxItrs_)*100.;
    }
    else if(terminationCondition_ == "simTime")
    {
        completionPercentage = simTime_/maxSimTime_*100.;
    }
    else if (terminationCondition_ == "realTime")
    {
        completionPercentage = elapsedRealTime_.total_microseconds()/maxElapsedRealTime_.total_microseconds()*100.;
    }

    message << "Simulation completion (%) |      " << completionPercentage << endl
            << "Iterations completed      |      " << itrs_ << endl
            << "Simulation time (sec)     |      " << simTime_ << endl
            << "Elapsed time (hh:mm:ss)   |      " << elapsedRealTime_ << endl
            << "Residual norm             |      " << residualNorm;

    Output::print(message.str());
    Output::printLine();
}

void RunControl::displayEndMessage()
{
    using namespace std;
    using namespace boost::posix_time;

    elapsedRealTime_ = microsec_clock::local_time() - startRealTime_;

    ostringstream message;

    Output::printLine();

    message << "Iterations completed on " << startRealTime_ + elapsedRealTime_ << endl
            << "Iterations completed: " << itrs_ << endl
            << "Simulation time: " << simTime_ << endl
            << "Elapsed time: " << elapsedRealTime_;

    Output::print(message.str());
    Output::printLine();
}
