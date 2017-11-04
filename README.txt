	Example:
	The simple class below will be compiled into fully-functional SQF code:
	
	///////////////////////////////////////////////////////////////////////////
	
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
		PUBLIC FUNCTION("","setUnit") { MEMBER("currentUnit",_this); };
		
		PUBLIC FUNCTION("string","deconstructor") {
			DELETE_VARIABLE("currentUnit");
			private _unitCount = MEMBER("unitCount",nil);
			_unitCount = _unitCount - 1;
			MEMBER("unitCount",_unitCount);
			hint _this;
		};
	ENDCLASS;

	///////////////////////////////////////////////////////////////////////////