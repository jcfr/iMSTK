/*=========================================================================
 * Copyright (c) Center for Modeling, Simulation, and Imaging in Medicine,
 *                        Rensselaer Polytechnic Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 /=========================================================================
 
 /**
  *  \brief
  *  \details
  *  \author
  *  \author
  *  \copyright Apache License, Version 2.0.
  */

#ifndef SMSYNCHRONIZATION_H
#define SMSYNCHRONIZATION_H

#include "smCore/smConfig.h"
#include "smCore/smCoreClass.h"
#include <QWaitCondition>
#include <QMutex>

/// \brief Synchronization class for sync the start/end of multiple threads
///simply set number of worker threads in the constructor
///then each worker threads should call waitTaskStart function when the taks
///is completed they should call signalTaskDone
class smSynchronization: public smCoreClass
{

    QWaitCondition taskDone;
    QWaitCondition taskStart;
    QMutex serverMutex;
    smInt totalWorkers;
    smInt finishedWorkerCounter;
    smInt startedWorkerCounter;
    smBool workerCounterUpdated;
    smInt newWorkerCounter;

public:

    /// \param p_threadsForWorkers  choose the number of worker threads
    smSynchronization(smInt p_threadsForWorkers)
    {
        type =   SIMMEDTK_SMSYNCHRONIZATION;
        totalWorkers = p_threadsForWorkers;
        finishedWorkerCounter = 0;
        startedWorkerCounter = 0;
        workerCounterUpdated = false;
    }

    /// \brief before starting tasks worker threads should wait
    void waitTaskStart()
    {
        serverMutex.lock();
        startedWorkerCounter++;

        if (startedWorkerCounter == totalWorkers)
        {
            startedWorkerCounter = 0;
            taskDone.wakeAll();
        }

        taskStart.wait(&serverMutex);
        serverMutex.unlock();
    }

    /// \brief when the task ends the worker should call this function
    void signalTaskDone()
    {

        serverMutex.lock();
        finishedWorkerCounter++;

        if (finishedWorkerCounter == totalWorkers)
        {
            finishedWorkerCounter = 0;
        }

        serverMutex.unlock();
    }

    /// \brief You could change the number of worker threads synchronization
    ///Call this function after in the main thread where orchestration is done.
    void setWorkerCounter(smInt p_workerCounter)
    {
        newWorkerCounter = p_workerCounter;
        workerCounterUpdated = true;
    }

    smInt getTotalWorkers()
    {
        return totalWorkers;
    }

    /// \brief the server thread should call this for to start exeuction of the worker threads
    void startTasks()
    {
        serverMutex.lock();

        if (workerCounterUpdated)
        {
            finishedWorkerCounter = newWorkerCounter;
            workerCounterUpdated = false;
        }

        taskStart.wakeAll();
        Sleep(100);
        taskDone.wait(&serverMutex);
        serverMutex.unlock();
    }

    /// \brief this function is fore signalling the events after waking up the worker threads.
    void startTasksandSignalEvent(smInt moduleId)
    {

        smEvent *eventSynch;
        eventSynch = new smEvent();
        eventSynch->eventType = SIMMEDTK_EVENTTYPE_SYNCH;
        eventSynch->senderId = moduleId;
        eventSynch->senderType = SIMMEDTK_SENDERTYPE_EVENTSOURCE;
        serverMutex.lock();
        taskStart.wakeAll();
        taskDone.wait(&serverMutex);
        serverMutex.unlock();
    }
};

#endif
