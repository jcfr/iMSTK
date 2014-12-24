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

#include "smCore/smErrorLog.h"

smErrorLog::smErrorLog()
{

    time.start();
    errorCount = -1;
    isOutputtoConsoleEnabled = true;
}

///add the error in the repository.It is thread safe. It can be called by multiple threads.
smBool smErrorLog::addError(smCoreClass *p_param, const smChar *p_text)
{

    smInt textLength;

    if (p_text == NULL)
    {
        return false;
    }

    textLength = strlen(p_text);

    if (textLength<SIMMEDTK_MAX_ERRORLOG_TEXT&textLength>0)
    {
        mutex.lock();

        if (isOutputtoConsoleEnabled)
        {
            cout << p_text << endl;
        }

        strcpy(errors[errorCount], p_text);
        timeStamp[errorCount] = time.elapsed();
        lastError = errorCount;

        errorCount++;

        if (errorCount >= SIMMEDTK_MAX_ERRORLOG)
        {
            lastError = SIMMEDTK_MAX_ERRORLOG - 1;
            errorCount = 0;
        }

        mutex.unlock();
        return true;
    }
    else
    {
        return false;
    }
}

smBool smErrorLog::addError(smCoreClass *p_param, const string p_text)
{
    return addError(p_param, p_text.c_str());
}

smBool smErrorLog::addError(const smChar *p_text)
{

    smInt textLength;

    if (p_text == NULL)
    {
        return false;
    }

    textLength = strlen(p_text);

    if (textLength<SIMMEDTK_MAX_ERRORLOG_TEXT&textLength>0)
    {
        mutex.lock();
        errorCount++;
        strcpy(errors[errorCount], p_text);
        timeStamp[errorCount] = time.elapsed();
        lastError = errorCount;

        if (errorCount >= SIMMEDTK_MAX_ERRORLOG)
        {
            lastError = SIMMEDTK_MAX_ERRORLOG - 1;
            errorCount = 0;
        }

        mutex.unlock();
        return true;
    }
    else
    {
        return false;
    }
}

smBool smErrorLog::addError(const string p_text)
{
    return addError(p_text.c_str());
}

///Clean up all the errors in the repository.It is thread safe.
void smErrorLog::cleanAllErrors()
{

    mutex.lock();

    for (smInt i = 0; i < SIMMEDTK_MAX_ERRORLOG; i++)
    {
        memset(errors[i], '\0', SIMMEDTK_MAX_ERRORLOG_TEXT);
    }

    mutex.unlock();
}

///Print the last error.It is not thread safe.
void smErrorLog::printLastErr()
{

    if (errorCount != -1)
    {
        cout << "Last Error:" << errors[errorCount] << " Time:" << timeStamp[errorCount - 1] << " ms" << endl;
    }
}

///Print  the last error in Thread Safe manner.
void smErrorLog::printLastErrSafe()
{

    mutex.lock();
    cout << "Last Error:" << errors[errorCount - 1] << " Time:" << timeStamp[errorCount - 1] << " ms" << endl;
    mutex.unlock();
}
