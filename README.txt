//////////////////////////////////////////////////////////////
//  Group: Examples
//////////////////////////////////////////////////////////////

/*
	Example:
	The simple class below will be compiled into fully-functional SQF code:
	
	(start code)
	
	#include "oop.h"
	
	CLASS("PlayerInfo")
		PRIVATE STATIC_VARIABLE("scalar","unitCount");
		PRIVATE VARIABLE("object","currentUnit");
		PUBLIC FUNCTION("object","constructor") {
			MEMBER("currentUnit",_this);
			private ["_unitCount"];
			_unitCount = MEMBER("unitCount",nil);
			if (isNil "_unitCount") then {_unitCount = 0};
			_unitCount = _unitCount + 1;
			MEMBER("unitCount",_unitCount);
		};
		PUBLIC FUNCTION("","getUnit") FUNC_GETVAR("currentUnit");
		PUBLIC FUNCTION("","setUnit") {
			MEMBER("currentUnit",_this);
		};
		PUBLIC FUNCTION("string","deconstructor") {
			DELETE_VARIABLE("currentUnit");
			private ["_unitCount"];
			_unitCount = MEMBER("unitCount",nil);
			_unitCount = _unitCount - 1;
			MEMBER("unitCount",_unitCount);
			hint _this;
		};
	ENDCLASS;
	
	(end)
	
	SQF class interaction:
	
	(start code)
	
	_playerInfo = ["new", player1] call PlayerInfo;
	_currentUnit = "getUnit" call _playerInfo;
	["setUnit", player2] call _playerInfo;
	["delete", _playerInfo, "Player Removed!"] call PlayerInfo;
	_playerInfo = nil;
	
	(end)
	
	Note: Both the constructor and deconstructor must be public.
*/