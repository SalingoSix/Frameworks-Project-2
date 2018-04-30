#include "cLuaBrain.h"

#include <iostream>
#include <math.h>

float cLuaBrain::myDeltaTime = 0.0f;

float cLuaBrain::timeSoFarForBezier = 0.0f;

std::vector < float > cLuaBrain::currentParams;

std::vector < CommandGroup* > cLuaBrain::commandGroups;

std::vector < Command* > cLuaBrain::activeCommands;

std::map < std::string /*file name*/, std::string /*THE SCRIPT*/ > cLuaBrain::m_luaScripts;

std::map < std::string /*name*/, std::vector <float> /*params*/ > cLuaBrain::m_params;

cLuaBrain::cLuaBrain()
{
	this->m_p_vecGOs = nullptr;

	// Create new Lua state.
	// NOTE: this is common to ALL script in this case
	this->m_pLuaState = luaL_newstate();

	luaL_openlibs(this->m_pLuaState);					/* Lua 5.3.3 */


	lua_pushcfunction( this->m_pLuaState, cLuaBrain::l_UpdateObject );
	lua_setglobal( this->m_pLuaState, "setObjectState" );

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_getParams);
	lua_setglobal(this->m_pLuaState, "getParams");

	lua_pushcfunction( this->m_pLuaState, cLuaBrain::l_GetObjectState );
	lua_setglobal( this->m_pLuaState, "getObjectState" );

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_GetObjectStateRot);
	lua_setglobal(this->m_pLuaState, "getObjectStateRotation");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_SetObjectStateRot);
	lua_setglobal(this->m_pLuaState, "setObjectStateRotation");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_getParamsRot);
	lua_setglobal(this->m_pLuaState, "getParamsRotatation");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_GetObjectStateCurve);
	lua_setglobal(this->m_pLuaState, "getObjectStateBezier");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_SetObjectStateCurve);
	lua_setglobal(this->m_pLuaState, "setObjectStateBezier");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_getParamsCurve);
	lua_setglobal(this->m_pLuaState, "getParamsBezier");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_GetObjectStateFollow);
	lua_setglobal(this->m_pLuaState, "getObjectStateFollow");

	lua_pushcfunction(this->m_pLuaState, cLuaBrain::l_getParamsFollow);
	lua_setglobal(this->m_pLuaState, "getParamsFollow");

	return;
}

cLuaBrain::~cLuaBrain()
{
	lua_close(this->m_pLuaState);
	return;
}


// Saves (and overwrites) any script
// scriptName is just so we can delete them later
void cLuaBrain::LoadScript( std::string scriptName, 
					        std::string scriptSource )
{
	this->m_mapScripts[scriptName] = scriptSource;
	return;
}


void cLuaBrain::DeleteScript( std::string scriptName )
{
	std::map<std::string, std::string>::iterator it;

	it = m_mapScripts.find(scriptName);
	m_mapScripts.erase(it);

	return;
}

// Passes a pointer to the game object vector
void cLuaBrain::SetObjectVector( std::vector< cGameObject* >* p_vecGOs )
{
	this->m_p_vecGOs = p_vecGOs;
	return;
}

void cLuaBrain::setDeltaTime(float theTime)
{
	myDeltaTime = theTime;
	return;
}

void cLuaBrain::retrieveMasterGroup(CommandGroup* master, std::map < std::string /*name*/, std::vector <float> /*params*/ > theParams,
										std::map < std::string /*file name*/, std::string /*THE SCRIPT*/ > theScripts)
{
	this->commandGroups.push_back(master);

	m_params = theParams;

	m_luaScripts = theScripts;

	return;
}

// Call all the active scripts that are loaded
void cLuaBrain::Update(float deltaTime)
{
	setDeltaTime(deltaTime);

	//First, must check if we need to add or remove scripts

	for (int groupIndex = 0; groupIndex < commandGroups.size(); groupIndex++)
	{
		CommandGroup* thisGroup = commandGroups[groupIndex];
		if (thisGroup->getComplete())
			continue;
		
		std::vector < iCommand* > theseCommands = thisGroup->v_Commands;

		for (int commandIndex = 0; commandIndex < theseCommands.size(); commandIndex++)
		{
			iCommand* thisCommand = theseCommands[commandIndex];
			if (thisCommand->getParallel())
			{
				if (thisCommand->getIssued()) 
				{// If the command was already issued, check if it's finished, and if it's a command, see if it needs to be deleted
					if (!thisCommand->getCommandGroup())
					{
						if (thisCommand->getComplete())
						{
							if (!thisCommand->getDeleted())
							{
								Command* casted = (Command*)thisCommand;
								casted->scriptComplete();
								this->DeleteScript(casted->commandName);
								thisCommand->setDeleted(true);
							}
						}
					}
					continue;
				}
				else // This command has not been issued yet
				{
					if (!thisCommand->getCommandGroup())
					{
						Command* casted = (Command*)thisCommand;
						if (casted->fileName != "FollowObject.lua") // Any other script required the initial position of the model
						{
							std::map< std::string, std::vector < float > >::iterator itParams;
							itParams = m_params.find(casted->commandName);
							std::vector <float> paramsToEdit = itParams->second;

							if (casted->fileName == "FollowCurveByTime.lua")
							{
								float elapsedTime = 0;
								paramsToEdit.push_back(elapsedTime);
							}

							int theObjectID = (int)paramsToEdit[1]; // Value 1 is CmdID, value 2 is ObjectID
							cGameObject* pGO = cLuaBrain::m_findObjectByID(theObjectID);

							if (casted->fileName == "RotateByTime.lua" || casted->fileName == "RotateByPercent.lua")
							{
								paramsToEdit.push_back(pGO->post_Rotation.x); // We are grabbing the position of the object at the moment the script is being called
								paramsToEdit.push_back(pGO->post_Rotation.y);
								paramsToEdit.push_back(pGO->post_Rotation.z);
							}

							else
							{
								paramsToEdit.push_back(pGO->x); // We are grabbing the position of the object at the moment the script is being called
								paramsToEdit.push_back(pGO->y);
								paramsToEdit.push_back(pGO->z);
							}

							m_params[casted->commandName] = paramsToEdit;
						} //Once we have added the position values, time to load the script
						std::map< std::string, std::string >::iterator itScripts;
						itScripts = m_luaScripts.find(casted->fileName);
						std::string myScript = itScripts->second;
						//FINALLY loading the script
						LoadScript(casted->commandName, myScript);
						casted->setIssued(true);
						activeCommands.push_back(casted);
						continue;
					}
					else // This is a parallel command group
					{
						CommandGroup* casted = (CommandGroup*)thisCommand;
						casted->setIssued(true);
						commandGroups.push_back(casted);
						continue;
					}
				}
			}
			else // This command is a serial
			{
				if (thisCommand->getIssued())
				{
					if (thisCommand->getComplete())
					{
						if (!thisCommand->getCommandGroup())
						{
							if (!thisCommand->getDeleted())
							{
								Command* casted = (Command*)thisCommand;
								casted->scriptComplete();
								this->DeleteScript(casted->commandName);
								thisCommand->setDeleted(true);
							}
						}
						continue;
					}
					else // Not complete yet
						break;
				}
				else // Not issued...
				{
					CommandGroup* castedParent = (CommandGroup*)thisCommand->getMyParent();
					int parentCompleted = castedParent->completedCommands;
					if (parentCompleted < commandIndex) // ie has every command before this one been completed?
						break;
					else // We will only issue this command if everything before it has been completed
					{
						if (!thisCommand->getCommandGroup())
						{
							Command* casted = (Command*)thisCommand;
							if (casted->fileName != "FollowObject.lua") // Any other script required the initial position of the model
							{
								std::map< std::string, std::vector < float > >::iterator itParams;
								itParams = m_params.find(casted->commandName);
								std::vector <float> paramsToEdit = itParams->second;

								if (casted->fileName == "FollowCurveByTime.lua")
								{
									float elapsedTime = 0;
									paramsToEdit.push_back(elapsedTime);
								}

								int theObjectID = (int)paramsToEdit[1]; // Value 1 is CmdID, value 2 is ObjectID
								cGameObject* pGO = cLuaBrain::m_findObjectByID(theObjectID);

								if (casted->fileName == "RotateByTime.lua" || casted->fileName == "RotateByPercent.lua")
								{
									paramsToEdit.push_back(pGO->post_Rotation.x); // We are grabbing the position of the object at the moment the script is being called
									paramsToEdit.push_back(pGO->post_Rotation.y);
									paramsToEdit.push_back(pGO->post_Rotation.z);
								}

								else
								{
									paramsToEdit.push_back(pGO->x); // We are grabbing the position of the object at the moment the script is being called
									paramsToEdit.push_back(pGO->y);
									paramsToEdit.push_back(pGO->z);
								}

								m_params[casted->commandName] = paramsToEdit;
							} //Once we have added the position values, time to load the script
							std::map< std::string, std::string >::iterator itScripts;
							itScripts = m_luaScripts.find(casted->fileName);
							std::string myScript = itScripts->second;
							//FINALLY loading the script
							LoadScript(casted->commandName, myScript);
							casted->setIssued(true);
							activeCommands.push_back(casted);
							break;
						}

						else // It's a serial command group
						{
							CommandGroup* casted = (CommandGroup*)thisCommand;
							casted->setIssued(true);
							commandGroups.push_back(casted);
							break;
						}
					}
				}
			}
		} // Cycling through all commands within a group...
	} // Cycling through all our command groups...

	//Iterating through all active scripts right now
	for( std::map< std::string /*name*/, std::string /*source*/>::iterator itScript = 
		 this->m_mapScripts.begin(); itScript != this->m_mapScripts.end(); itScript++ )
	{

		std::map< std::string, std::vector < float > >::iterator itParams;

		itParams = m_params.find(itScript->first);

		currentParams = itParams->second;

		std::string curLuaScript = itScript->second;

		int error = luaL_loadstring( this->m_pLuaState, curLuaScript.c_str() );

		if ( error != 0 /*no error*/)	
		{
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			continue;
		}

		// execute funtion in "protected mode", where problems are 
		//  caught and placed on the stack for investigation
		error = lua_pcall(this->m_pLuaState,	/* lua state */
						  0,	/* nargs: number of arguments pushed onto the lua stack */
						  0,	/* nresults: number of results that should be on stack at end*/
						  0);	/* errfunc: location, in stack, of error function. 
								   if 0, results are on top of stack. */
		if ( error != 0 /*no error*/)	
		{
			std::cout << "Lua: There was an error..." << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

			std::string luaError;
			// Get error information from top of stack (-1 is top)
			luaError.append( lua_tostring(this->m_pLuaState, -1) );

			// Make error message a little more clear
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << luaError << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			// We passed zero (0) as errfunc, so error is on stack)
			lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */

			continue;
		}

	}
	return;
}

/*static*/ int cLuaBrain::l_UpdateObject( lua_State *L )
{
	int commandID = lua_tonumber(L, 1);	/* get argument */

	int objectID = lua_tonumber(L, 2);	/* get argument */

	
	// Exist? 
	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if ( pGO == nullptr )
	{	// No, it's invalid
		lua_pushboolean( L,  false );
		// I pushed 1 thing on stack, so return 1;
		return 1;	
	}
	
	//Update object with returned values
	pGO->x = lua_tonumber(L, 3);	/* get argument */
	pGO->y = lua_tonumber(L, 4);	/* get argument */
	pGO->z = lua_tonumber(L, 5);	/* get argument */
	pGO->Vx = lua_tonumber(L, 6);	/* get argument */
	pGO->Vy = lua_tonumber(L, 7);	/* get argument */
	pGO->Vz = lua_tonumber(L, 8);	/* get argument */
	bool finished = lua_toboolean(L, 9);

	if (finished)
	{
		Command* myCommand;
		for (int index = 0; index < activeCommands.size(); index++)
		{
			if (activeCommands[index]->getCommandID() == commandID)
			{
				myCommand = activeCommands[index];
				break;
			}
		}
		//myCommand->scriptComplete(); // Calling this functions traps us in a loop of terror FOR SOME REASON
		myCommand->setComplete(true);
	}

	lua_pushboolean( L, true );	// index is OK
	
	return 9;		// There were 7 things on the stack

}

/*static*/  int cLuaBrain::l_SetObjectStateRot(lua_State *L)
{
	int commandID = lua_tonumber(L, 1);	/* get argument */

	int objectID = lua_tonumber(L, 2);	/* get argument */

										// Exist? 
	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	float orientX, orientY, orientZ;
	// Object ID is valid
	// Get the values that lua pushed and update object
	orientX = lua_tonumber(L, 3);	/* get argument */
	orientY = lua_tonumber(L, 4);	/* get argument */
	orientZ = lua_tonumber(L, 5);	/* get argument */
	pGO->rotSpeed.x = lua_tonumber(L, 6);	/* get argument */
	pGO->rotSpeed.y = lua_tonumber(L, 7);	/* get argument */
	pGO->rotSpeed.z = lua_tonumber(L, 8);	/* get argument */
	bool finished = lua_toboolean(L, 9);

	pGO->post_Rotation = glm::vec3(orientX, orientY, orientZ);

	if (finished)
	{
		Command* myCommand;
		for (int index = 0; index < activeCommands.size(); index++)
		{
			if (activeCommands[index]->getCommandID() == commandID)
			{
				myCommand = activeCommands[index];
				break;
			}
		}
		//myCommand->scriptComplete();
		myCommand->setComplete(true);
	}

	lua_pushboolean(L, true);	// index is OK

	return 9;		// There were 7 things on the stack
}

/*static*/ int cLuaBrain::l_SetObjectStateCurve(lua_State *L)
{
	int commandID = lua_tonumber(L, 1);
	
	int objectID = lua_tonumber(L, 2);	/* get argument */

										// Exist? 
	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	float timeSoFar;
	bool isFinished;

	// Object ID is valid
	// Get the values that lua pushed and update object
	pGO->x = lua_tonumber(L, 3);	/* get argument */
	pGO->y = lua_tonumber(L, 4);	/* get argument */
	pGO->z = lua_tonumber(L, 5);	/* get argument */
	timeSoFar = lua_tonumber(L, 6);	/* get argument */
	isFinished = lua_toboolean(L, 7);

	timeSoFarForBezier = timeSoFar;

	Command* myCommand;
	for (int index = 0; index < activeCommands.size(); index++)
	{
		if (activeCommands[index]->getCommandID() == commandID)
		{
			myCommand = activeCommands[index];
			break;
		}
	}

	std::map< std::string, std::vector < float > >::iterator itParams;
	itParams = m_params.find(myCommand->commandName);
	std::vector <float> paramsToEdit = itParams->second;

	paramsToEdit[12] = timeSoFar;

	m_params[myCommand->commandName] = paramsToEdit;


	if (isFinished)
	{
		//Here we will be reseting the bezier number, and deleting the script, but it's fine for now
		//myCommand->scriptComplete();
		myCommand->setComplete(true);

	}

	lua_pushboolean(L, true);	// index is OK

	return 7;		// There were 7 things on the stack
}

/*static*/ int cLuaBrain::l_GetObjectState(lua_State *L)
{
	int objectID = lua_tonumber(L, 1);	/* get argument */

										// Exist? 
	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	// Object ID is valid
	lua_pushnumber(L, myDeltaTime);	// index is OK
	lua_pushnumber(L, pGO->x);
	lua_pushnumber(L, pGO->y);
	lua_pushnumber(L, pGO->z);
	lua_pushnumber(L, pGO->Vx);
	lua_pushnumber(L, pGO->Vy);
	lua_pushnumber(L, pGO->Vz);

	return 7;		// There were 7 things on the stack
}

/*static*/  int cLuaBrain::l_GetObjectStateRot(lua_State *L)
{
	int objectID = lua_tonumber(L, 1);	/* get argument */

										// Exist? 
	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	// Object ID is valid
	lua_pushnumber(L, myDeltaTime);	// index is OK
	lua_pushnumber(L, pGO->post_Rotation.x);
	lua_pushnumber(L, pGO->post_Rotation.y);
	lua_pushnumber(L, pGO->post_Rotation.z);
	lua_pushnumber(L, pGO->rotSpeed.x);
	lua_pushnumber(L, pGO->rotSpeed.y);
	lua_pushnumber(L, pGO->rotSpeed.z);

	return 7;		// There were 7 things on the stack
}

/*static*/ int cLuaBrain::l_GetObjectStateCurve(lua_State *L)
{
	int objectID = lua_tonumber(L, 1);	/* get argument */

										// Exist? 
	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	if (pGO == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	// Object ID is valid
	lua_pushnumber(L, myDeltaTime);	// index is OK
	lua_pushnumber(L, pGO->post_Rotation.x);
	lua_pushnumber(L, pGO->post_Rotation.y);
	lua_pushnumber(L, pGO->post_Rotation.z);

	return 4;		// There were 7 things on the stack
}

/*static*/ int cLuaBrain::l_GetObjectStateFollow(lua_State *L)
{
	int objectID = lua_tonumber(L, 1);	// Gets follower's object ID

	int followID = lua_tonumber(L, 2);	// Gets followee's ID

										// Exist? 
	cGameObject* pGO = cLuaBrain::m_findObjectByID(objectID);

	cGameObject* pGOFollow = cLuaBrain::m_findObjectByID(followID);

	if (pGO == nullptr || pGOFollow == nullptr)
	{	// No, it's invalid
		lua_pushboolean(L, false);
		// I pushed 1 thing on stack, so return 1;
		return 1;
	}

	// Object ID is valid
	lua_pushnumber(L, myDeltaTime);	// index is OK
	lua_pushnumber(L, pGO->x);			//Follower's position
	lua_pushnumber(L, pGO->y);
	lua_pushnumber(L, pGO->z);
	lua_pushnumber(L, pGO->Vx);			//Follower's velocity
	lua_pushnumber(L, pGO->Vy);
	lua_pushnumber(L, pGO->Vz);
	lua_pushnumber(L, pGOFollow->x);	//Followee's position
	lua_pushnumber(L, pGOFollow->y);
	lua_pushnumber(L, pGOFollow->z);

	return 10;
}

/*static*/ int cLuaBrain::l_getParams(lua_State *L)
{

	lua_pushnumber(L, currentParams[0]);	// Command ID
	lua_pushnumber(L, currentParams[1]);	// Object ID
	lua_pushnumber(L, currentParams[2]);	// Destination X Value
	lua_pushnumber(L, currentParams[3]);	// Destination Y Value
	lua_pushnumber(L, currentParams[4]);	// Destination Z Value
	lua_pushnumber(L, currentParams[5]);	// Max velocity / time to complete movement
	lua_pushnumber(L, currentParams[6]);	// Reach max speed by (seconds / distance / %)
	lua_pushnumber(L, currentParams[7]);	// Slow to stop by (seconds / distance / %)
	lua_pushnumber(L, currentParams[8]);	// Initial X (position)
	lua_pushnumber(L, currentParams[9]);	// Initial Y
	lua_pushnumber(L, currentParams[10]);	// Initial Z

	return 11;
}

/*static*/ int cLuaBrain::l_getParamsRot(lua_State *L)
{
	lua_pushnumber(L, currentParams[0]);	// Command ID
	lua_pushnumber(L, currentParams[1]);	// Object ID
	lua_pushnumber(L, currentParams[2]);	// Required X Rotation (degrees)
	lua_pushnumber(L, currentParams[3]);	// Y rotation
	lua_pushnumber(L, currentParams[4]);	// Z Rotation
	lua_pushnumber(L, currentParams[5]);	// Max velocity / time to complete movement
	lua_pushnumber(L, currentParams[6]);	// Reach max speed by (seconds / distance / %)
	lua_pushnumber(L, currentParams[7]);	// Slow to stop by (seconds / distance / %)
	lua_pushnumber(L, currentParams[8]);	// Initial X
	lua_pushnumber(L, currentParams[9]);	// Initial Y
	lua_pushnumber(L, currentParams[10]);	// Initial Z

	return 11;
}

/*static*/ int cLuaBrain::l_getParamsCurve(lua_State *L)
{
	lua_pushnumber(L, currentParams[0]);	// Command ID
	lua_pushnumber(L, currentParams[1]);	// GameObject ID
	lua_pushnumber(L, currentParams[2]);	// End point X
	lua_pushnumber(L, currentParams[3]);	// End point Y
	lua_pushnumber(L, currentParams[4]);	// End point Z
	lua_pushnumber(L, currentParams[5]);	// Mid point X
	lua_pushnumber(L, currentParams[6]);	// Mid point Y
	lua_pushnumber(L, currentParams[7]);	// Mid point Z
	lua_pushnumber(L, currentParams[8]);	// Mid point 2 X
	lua_pushnumber(L, currentParams[9]);	// Mid point 2 Y
	lua_pushnumber(L, currentParams[10]);	// Mid point 2 Z
	lua_pushnumber(L, currentParams[11]);	// Time to complete the path
	lua_pushnumber(L, currentParams[12]);	// elapsed time
	lua_pushnumber(L, currentParams[13]);	// Initial X (rotation)
	lua_pushnumber(L, currentParams[14]);	// Initial Y
	lua_pushnumber(L, currentParams[15]);	// Initial Z

	return 16;
}

/*static*/ int cLuaBrain::l_getParamsFollow(lua_State *L)
{
	lua_pushnumber(L, currentParams[0]);		// Command ID
	lua_pushnumber(L, currentParams[1]);		// GameObject ID
	lua_pushnumber(L, currentParams[2]);		// Followee's ID
	lua_pushnumber(L, currentParams[3]);		// Closest distance allowed between the objects
	lua_pushnumber(L, currentParams[4]);		// Furthest distance, ie. beyond this distance, the follower should be at its max speed
	lua_pushnumber(L, currentParams[5]);		// Max speed the follower can travel at

	return 6;
}

/*static*/ 
std::vector< cGameObject* >* cLuaBrain::m_p_vecGOs;


// returns nullptr if not found
/*static*/ cGameObject* cLuaBrain::m_findObjectByID( int ID )
{
	for ( std::vector<cGameObject*>::iterator itGO = cLuaBrain::m_p_vecGOs->begin(); 
		  itGO != cLuaBrain::m_p_vecGOs->end(); itGO++ )
	{
		if ( (*itGO)->getID() == ID )
		{	// Found it!
			return (*itGO);
		}
	}//for ( std::vector<cGameObject*>::iterator itGO...
	// Didn't find it
	return nullptr;
}


std::string cLuaBrain::m_decodeLuaErrorToString( int error )
{
	switch ( error )
	{
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error"; 
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	}//switch ( error )

	// Who knows what this error is?
	return "Lua: UNKNOWN error";
}
