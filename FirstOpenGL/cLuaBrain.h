#ifndef _cLuaBrain_HG_
#define _cLuaBrain_HG_

extern "C" {
#include <Lua5.3.3\lua.h>
#include <Lua5.3.3\lua.h>
#include <Lua5.3.3\lauxlib.h>
#include <Lua5.3.3/lualib.h>
}

#include <string>
#include <vector>
#include "cGameObject.h"
#include <map>

#include "iCommand.h"
#include "Command.h"
#include "CommandGroup.h"

class cLuaBrain
{
public:
	// Init Lua and set callback functions
	cLuaBrain();
	~cLuaBrain();
	void LoadScript( std::string scriptName, 
					 std::string scriptSource );
	void DeleteScript( std::string scriptName );
	// Passes a pointer to the game object vector
	void SetObjectVector( std::vector< cGameObject* >* p_vecGOs );
	// Call all the active scripts that are loaded
	void Update(float deltaTime);
	// Called by Lua
	// Passes object ID, new velocity, etc.
	// Returns valid (true or false)
	// Passes: 
	// - position (xyz)
	// - velocity (xyz)
	// called "setObjectState" in lua
	static int l_UpdateObject( lua_State *L );
	// Passes object ID
	// Returns valid (true or false)
	// - position (xyz)
	// - velocity (xyz)
	// called "getObjectState" in lua
	static int l_GetObjectState( lua_State *L );

	static int l_GetObjectStateRot(lua_State *L);

	static int l_SetObjectStateRot(lua_State *L);

	static int l_getParams(lua_State *L);

	static int l_getParamsRot(lua_State *L);

	static int l_GetObjectStateCurve(lua_State *L);

	static int l_SetObjectStateCurve(lua_State *L);

	static int l_getParamsCurve(lua_State *L);

	static int l_GetObjectStateFollow(lua_State *L);
	
	static int l_getParamsFollow(lua_State *L);

	void retrieveMasterGroup(CommandGroup* master, std::map < std::string /*name*/, std::vector <float> /*params*/ > theParams,
								std::map < std::string /*file name*/, std::string /*THE SCRIPT*/ > theScripts);

private:
	std::map< std::string /*scriptName*/, 
			  std::string /*scriptSource*/ > m_mapScripts;

	static std::vector< cGameObject* >* m_p_vecGOs;
	// returns nullptr if not found
	static cGameObject* m_findObjectByID( int ID );

	lua_State* m_pLuaState;

	std::string m_decodeLuaErrorToString( int error );

	static float myDeltaTime;

	static float timeSoFarForBezier;

	void setDeltaTime(float theTime);

	static std::vector < float > currentParams;

	static std::vector < CommandGroup* > commandGroups;

	static std::vector < Command* > activeCommands;

	static std::map < std::string /*file name*/, std::string /*THE SCRIPT*/ > m_luaScripts;

	static std::map < std::string /*name*/, std::vector <float> /*params*/ > m_params;
};

#endif
