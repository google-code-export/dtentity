/* -*-c++-*-
* testEntity - testEntity(.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* Martin Scheffler
*/

#include <dtEntity/entitymanager.h>
#include <dtEntity/commandmessages.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/logmanager.h>
#include <dtEntity/initosgviewer.h>
#include <dtEntity/messagefactory.h>
#include <dtEntityNet/enetcomponent.h>


#ifdef _WIN32
#include <windows.h>
#define SLEEPFUN Sleep
#else
#define SLEEPFUN sleep
#endif

#define PORT_NUMBER 6789

int main()
{
   using namespace dtEntity;

   LogManager::GetInstance().AddListener(new ConsoleLogHandler());


   EntityManager em;

   RegisterCommandMessages(MessageFactory::GetInstance());
   RegisterSystemMessages(MessageFactory::GetInstance());

   dtEntityNet::ENetSystem* enetsys = new dtEntityNet::ENetSystem(em);
   em.AddEntitySystem(*enetsys);

   enetsys->InitializeServer(PORT_NUMBER);

   TickMessage tickmsg;


   while(true)
   {
      em.EmitQueuedMessages(1.0f);
      em.EmitMessage(tickmsg);
      SLEEPFUN(1);
   }

   return 0;

}
