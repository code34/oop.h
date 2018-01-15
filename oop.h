//////////////////////////////////////////////////////////////
//
//  Title: RBS Object Oriented SQF Scripting
//  ----------------------------------------
//  File: oop.h
//  Author: Naught <dylanplecki@gmail.com>
//  Edited by: Neumatic
//  Version: 1.3.1
//
//  Description:
//  Contains preprocessor definitions and macros for designing
//  and implementing object oriented code into the SQF
//  scripting language. Uses global variables.
//
//  Note:
//  All API documentation can be found below in the
//  <Interactive (API) Macros and Definitions> group.
//
//////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////
//  Group: Basic Macros
//////////////////////////////////////////////////////////////

#define QUOTE(var) #var
#define DOUBLES(var1,var2) ##var1##_##var2
#define TRIPLES(var1,var2,var3) ##var1##_##var2##_##var3
#define DEFAULT_PARAM(idx,dft) param [idx, dft]
#define TO_LOCAL(var) _##var

//////////////////////////////////////////////////////////////
//  Group: Internal Definitions
//////////////////////////////////////////////////////////////

#define CONSTRUCTOR_METHOD "constructor"
#define DECONSTRUCTOR_METHOD "deconstructor"
#define AUTO_INC_VAR(className) (format ["%1_IDAI", className])

//////////////////////////////////////////////////////////////
//  Group: Internal Macros
//////////////////////////////////////////////////////////////

#define SAFE_VAR(var) (if (isNil {var}) then {nil} else {var})

#define ENSURE_INDEX(idx,dft) if ((count _this) <= idx) then {_this set [idx, dft]}
#define CHECK_THIS if (isNil "_this") then {_this = []} else {if (!(_this isEqualType [])) then {_this = [_this]}}

#define CHECK_ACCESS(lvl) if ((_access >= lvl) &&
#define CHECK_TYPE(typeStr) ((_argType == typeStr) || {(typeStr == "ANY")})
#define CHECK_NIL (_argType == "")
#define CHECK_MEMBER(name) (_member == name)
#define CHECK_VAR(typeStr,varName) {CHECK_MEMBER(varName)} && {CHECK_TYPE(typeStr) || {CHECK_NIL}}

#define GETVAR(var) (format ["%1_%2", _classID, var])
#define GETSVAR(var) (format ["%1_%2", _class, var])
#define GETCLASS(className) (NAMESPACE getVariable [className, {nil}])
#define CALLCLASS(className,member,args,access) ([_classID, member, args, access] call GETCLASS(className))
#define SPAWNCLASS(className,member,args,access) ([_classID, member, args, access] spawn GETCLASS(className))

#define VAR_DFT_FUNC(varName) {if (isNil "_this") exitWith {NAMESPACE getVariable [GETVAR(varName), nil]}; NAMESPACE setVariable [GETVAR(varName), _this]}
#define SVAR_DFT_FUNC(varName) {if (isNil "_this") exitWith {NAMESPACE getVariable [GETSVAR(varName), nil]}; NAMESPACE setVariable [GETSVAR(varName), _this]}
#define VAR_DELETE(varName) (NAMESPACE setVariable [GETVAR(varName), nil])

#define M_VAR_DFT_FUNC(varName) {if (isNil "_this") exitWith {missionNamespace getVariable [GETVAR(varName), nil]}; missionNamespace setVariable [GETVAR(varName), _this]}
#define M_SVAR_DFT_FUNC(varName) {if (isNil "_this") exitWith {missionNamespace getVariable [GETSVAR(varName), nil]}; missionNamespace setVariable [GETSVAR(varName), _this]}
#define M_VAR_DELETE(varName) (missionNamespace setVariable [GETVAR(varName), nil])

#define UI_VAR_DFT_FUNC(varName) {if (isNil "_this") exitWith {uiNamespace getVariable [GETVAR(varName), nil]}; uiNamespace setVariable [GETVAR(varName), _this]}
#define UI_SVAR_DFT_FUNC(varName) {if (isNil "_this") exitWith {uiNamespace getVariable [GETSVAR(varName), nil]}; uiNamespace setVariable [GETSVAR(varName), _this]}
#define UI_VAR_DELETE(varName) (uiNamespace setVariable [GETVAR(varName), nil])

#define P_VAR_DFT_FUNC(varName) {if (isNil "_this") exitWith {profileNamespace getVariable [GETVAR(varName), nil]}; profileNamespace setVariable [GETVAR(varName), _this]}
#define P_SVAR_DFT_FUNC(varName) {if (isNil "_this") exitWith {profileNamespace getVariable [GETSVAR(varName), nil]}; profileNamespace setVariable [GETSVAR(varName), _this]}
#define P_VAR_DELETE(varName) (profileNamespace setVariable [GETVAR(varName), nil])

#define MOD_VAR(varName,mod) MEMBER(varName,MEMBER(varName,nil) mod)

#define GET_AUTO_INC(className) (NAMESPACE getVariable [AUTO_INC_VAR(className), 0])

#define INSTANTIATE_CLASS(className) \
    NAMESPACE setVariable [className, { \
    private _classID = param [0, "", [""]]; \
    if (_classID isEqualTo "") exitWith {nil}; \
    if (_classID == "new") exitWith { \
        NAMESPACE setVariable [AUTO_INC_VAR(className), (GET_AUTO_INC(className) + 1)]; \
        private _code = compile format ['["%1", (param [0, "", [""]]), (param [1, nil]), 0] call GETCLASS(className)', (format ["%1_%2", className, GET_AUTO_INC(className)])]; \
        [CONSTRUCTOR_METHOD, (param [1, nil])] call _code; \
        _code; \
    }; \
    if (_classID == "delete") exitWith { \
        [DECONSTRUCTOR_METHOD, (param [2, nil])] call (param [1, {nil}, [{}]]); \
    }; \
    if (_classID == "static") exitWith { \
        [className, (param [1, "", [""]]), (param [2, nil]), 0] call GETCLASS(className); \
    }; \
    params ["", ["_member", "", [""]], ["_this", nil], ["_access", 0, [0]]]; \
    if (_member isEqualTo "") exitWith {nil}; \
    private _argType = if (isNil "_this") then {""} else {typeName _this}; \
    private _class = className;

#define FINALIZE_CLASS if (!isNil "_parentClass") exitWith {CALLCLASS(_parentClass,_member,_this,1)};}]

//////////////////////////////////////////////////////////////
//  Group: Interactive (API) Macros and Definitions
//////////////////////////////////////////////////////////////

/*
    Define: NAMESPACE
    Defines the usable namespace for all preceeding classes.
    When extending a class from another class, both classes must be within the same namespace.
*/
#ifndef NAMESPACE
#define NAMESPACE missionNamespace
#endif

/*
    Macro: CLASS(className)
    Initializes a new class, or overwrites an existing one.
    Interaction with the class can be performed with the following code:
        ["memberName", args] call ClassName;
    This code must be executed in the correct namespace, and will only have access to public members.

    Parameters:
        className - The name of the class [string].

    See Also:
        <CLASSEXTENDS>
*/
#define CLASS(className) INSTANTIATE_CLASS(className)

/*
    Macro: CLASS_EXTENDS(childClassName,parentClassName)
    Initializes a new class extending a parent class, or overwrites an existing class.
    Interaction with the class can be performed with the following code:
        ["memberName", args] call ClassName;
    This code must be executed in the correct namespace, and will only have access to public members.

    Parameters:
        childClassName - The name of the child class [string].
        parentClassName - The name of the parent class [string].

    See Also:
        <CLASS>
*/
#define CLASS_EXTENDS(childClassName,parentClassName) INSTANTIATE_CLASS(childClassName) \
    private _parentClass = parentClassName;

/*
    Defines:
    - PRIVATE
        Initializes a private member within a class.
        Private members may only be accessed by members of its own class.
    - PROTECTED
        Initializes a protected member within a class.
        Protected members may only be accessed by members of its own class or child classes.
    - PUBLIC
        Initializes a public member within a class.
        Public members may be accessed by anyone.
*/
#define PRIVATE CHECK_ACCESS(2)
#define PROTECTED CHECK_ACCESS(1)
#define PUBLIC CHECK_ACCESS(0)

/*
    Macro: FUNCTION(typeStr,fncName)
    Initializes a new function member of a class.

    Parameters:
        typeStr - The typeName of the argument. Reference <http://community.bistudio.com/wiki/typeName> [string].
        fncName - The name of the function member [string].

    See Also:
        <VARIABLE>
*/
#define FUNCTION(typeStr,fncName) {CHECK_MEMBER(fncName)} && {CHECK_TYPE(typeStr)}) exitWith

/*
    Macros:
        VARIABLE(typeStr,varName)
        STATIC_VARIABLE(typeStr,varName)

    Description:
        Initializes a new variable member of a class. Static variables do not change between classes.

    Parameters:
        typeStr - The typeName of the argument. Reference <http://community.bistudio.com/wiki/typeName> [string].
        varName - The name of the variable member [string].

    See Also:
        <FUNCTION>
*/
#define VARIABLE(typeStr,varName) CHECK_VAR(typeStr,varName)) exitWith VAR_DFT_FUNC(varName)
#define STATIC_VARIABLE(typeStr,varName) CHECK_VAR(typeStr,varName)) exitWith SVAR_DFT_FUNC(varName)

/*
    Macro: DELETE_VARIABLE(varName)
    Deletes (nils) a variable which has been defined using the <VARIABLE> macro.
    This macro must be used inside a member function, and works regardless of the variable's protection.

    Parameters:
        varName - The name of the variable member to delete [string].

    See Also:
        <VARIABLE>
*/
#define DELETE_VARIABLE(varName) VAR_DELETE(varName)

/*
    Macros:
        M_VARIABLE(typeStr,varName)
        M_STATIC_VARIABLE(typeStr,varName)

    Description:
        Initializes a new mission variable member of a class. Static mission variables do not change between classes.

    Parameters:
        typeStr - The typeName of the argument. Reference <http://community.bistudio.com/wiki/typeName> [string].
        varName - The name of the mission variable member [string].

    See Also:
        <FUNCTION>
*/
#define M_VARIABLE(typeStr,varName) CHECK_VAR(typeStr,varName)) exitWith M_VAR_DFT_FUNC(varName)
#define STATIC_M_VARIABLE(typeStr,varName) CHECK_VAR(typeStr,varName)) exitWith M_SVAR_DFT_FUNC(varName)

/*
    Macro: DELETE_M_VARIABLE(varName)
    Deletes (nils) a mission variable which has been defined using the <UI_VARIABLE> macro.
    This macro must be used inside a member function, and works regardless of the mission variable's protection.

    Parameters:
        varName - The name of the mission variable member to delete [string].

    See Also:
        <M_VARIABLE>
*/
#define DELETE_M_VARIABLE(varName) M_VAR_DELETE(varName)

/*
    Macros:
        UI_VARIABLE(typeStr,varName)
        UI_STATIC_VARIABLE(typeStr,varName)

    Description:
        Initializes a new ui variable member of a class. Static ui variables do not change between classes.

    Parameters:
        typeStr - The typeName of the argument. Reference <http://community.bistudio.com/wiki/typeName> [string].
        varName - The name of the ui variable member [string].

    See Also:
        <FUNCTION>
*/
#define UI_VARIABLE(typeStr,varName) CHECK_VAR(typeStr,varName)) exitWith UI_VAR_DFT_FUNC(varName)
#define STATIC_UI_VARIABLE(typeStr,varName) CHECK_VAR(typeStr,varName)) exitWith UI_SVAR_DFT_FUNC(varName)

/*
    Macro: DELETE_UI_VARIABLE(varName)
    Deletes (nils) a ui variable which has been defined using the <UI_VARIABLE> macro.
    This macro must be used inside a member function, and works regardless of the ui variable's protection.

    Parameters:
        varName - The name of the ui variable member to delete [string].

    See Also:
        <UI_VARIABLE>
*/
#define DELETE_UI_VARIABLE(varName) UI_VAR_DELETE(varName)

/*
    Macros:
        P_VARIABLE(typeStr,varName)
        P_STATIC_VARIABLE(typeStr,varName)

    Description:
        Initializes a new profile variable member of a class. Static profile variables do not change between classes.

    Parameters:
        typeStr - The typeName of the argument. Reference <http://community.bistudio.com/wiki/typeName> [string].
        varName - The name of the profile variable member [string].

    See Also:
        <FUNCTION>
*/
#define P_VARIABLE(typeStr,varName) CHECK_VAR(typeStr,varName)) exitWith P_VAR_DFT_FUNC(varName)
#define STATIC_P_VARIABLE(typeStr,varName) CHECK_VAR(typeStr,varName)) exitWith P_SVAR_DFT_FUNC(varName)

/*
    Macro: DELETE_P_VARIABLE(varName)
    Deletes (nils) a profile variable which has been defined using the <PF_VARIABLE> macro.
    This macro must be used inside a member function, and works regardless of the profile variable's protection.

    Parameters:
        varName - The name of the profile variable member to delete [string].

    See Also:
        <P_VARIABLE>
*/
#define DELETE_P_VARIABLE(varName) P_VAR_DELETE(varName)

/*
    Macro: MEMBER(memberStr,args)
    Calls a member function or gets/sets a member variable. This will only work on members
    of the current class. All class members (private, protected, public) can be accessed through this
    macro. All public and protected members of parent classes will be available while using this macro.
    If accessing a variable member, passing a nil argument will retrieve the variable while anything else
    will set the variable to the value of the argument.

    Parameters:
        memberStr - The name of the member function or variable [string].
        args - The arguments to be passed to the member function or variable [any].
*/
#define MEMBER(memberStr,args) CALLCLASS(_class,memberStr,args,2)

/*
    Macro: SPAWN_MEMBER(memberStr,args)
    Spawns a member function or sets a member variable. This will only work on members
    of the current class. All class members (private, protected, public) can be accessed through this
    macro. All public and protected members of parent classes will be available while using this macro.
    This unlike MEMBER cannot access a variable member by passing a nil argument. Anything else
    will set the variable to the value of the argument.

    Parameters:
        memberStr - The name of the member function or variable [string].
        args - The arguments to be passed to the member function or variable [any].
*/
#define SPAWN_MEMBER(memberStr,args) SPAWNCLASS(_class,memberStr,args,2)

/*
    Macro: FUNC_GETVAR(varName)
    Returns a variable of the current class, used as a function.

    Example:
        PUBLIC FUNCTION("","getSpawnState") FUNC_GETVAR("spawned");

    Parameters:
        varName - The name of the variable member [string].
*/
#define FUNC_GETVAR(varName) {MEMBER(varName,nil)}

/*
    Define: ENDCLASS
    Ends a class's initializaton and finalizes SQF output.
*/
#define ENDCLASS FINALIZE_CLASS

/*
    Macro: ADD(varName,value)
    Add a value to variable member.

    Parameters:
        varName - The name of the variable member [string].
        value - Value to add to variable member [any].
*/
#define ADD(varName,value) MOD_VAR(varName,+ value)

/*
    Macro: SUB(varName,value)
    Subtract a value from variable member.

    Parameters:
        varName - The name of the variable member [string].
        value - Value to subtract from variable member [any].
*/
#define SUB(varName,value) MOD_VAR(varName,- value)

/*
    Macro: INC(varName)
    Increase variable member scalar by 1.

    Parameters:
        varName - The name of the variable member [string].
*/
#define INC(varName) ADD(varName,1)

/*
    Macro: DEC(varName)
    Decrease variable member scalar by 1.

    Parameters:
        varName - The name of the variable member [string].
*/
#define DEC(varName) SUB(varName,1)

/*
    Macro: PUSH(varName,element)
    Push element to back of variable member array. Returns element index.

    Parameters:
        varName - The name of the variable member array [string].
        element - Element to push to back of variable member array [any].
*/
#define PUSH(varName,element) MEMBER(varName,nil) pushBack element

/*
    Macro: REMOVE(varName,element)
    Find, remove and return element from variable member array.

    Parameters:
        varName - The name of the variable member array [string].
        element - Element to find and remove from variable member array [any].
*/
#define REMOVE(varName,element) (MEMBER(varName,nil) deleteAt (MEMBER(varName,nil) find element))
//#define REMOVE(varName,element) (MEMBER(varName,nil) call {_this deleteAt (_this find element)})

/*
    Macro: DELETE(varName,element)
    Delete element(s) from variable member array.

    Parameters:
        varName - The name of the variable member array [string].
        element - Element to delete from variable member array [any].
*/
#define DELETE(varName,element) MOD_VAR(varName,- [element])

/*
    Macro: SET(varName,idx,value)
    Set index in variable member array.

    Parameters:
        varName - The name of the variable member array [string].
        index - Index to set in variable member array [scalar].
        value - Value to set in variable member array [any].
*/
#define SET(varName,index,value) MEMBER(varName,nil) set [index, value]

/*
    Macro: GET(varName,index,default)
    Get index element from variable member array. Returns default value if no
    element at index.

    Parameters:
        varName - The name of the variable member array [string].
        index - Index to set in variable member array [scalar].
        default - Default value to return if no index [any].
*/
#define GET(varName,index,default) MEMBER(varName,nil) param [index, default]

/*
    Macro: CLEAR(varName)
    Clear variable member array of all elements.

    Parameters:
        varName - The name of the variable member array [string].
*/
#define CLEAR(varName) MEMBER(varName,nil) resize 0

/*
    Macro: SCRIPT_NAME(name)
    Assign script name to function.
*/
#define SCRIPT_NAME scriptName format ["%1: %2", _classID, _member]

/*
    Macro: IS_NIL(varName)
    Check if variable member has value.

    Parameters:
        varName - The name of the variable member [string].
*/
#define IS_NIL(varName) isNil {MEMBER(varName,nil)}