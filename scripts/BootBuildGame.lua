#!/bin/lua 
 -- Language: Lua

DEBUG = true;
DEBUGCMD = false;
VMNAME = "/home/sgp1000/vmware/AlphaBuildEnv/SB_5.00.00.002.vmx";
VMSTARTED = false;
ShareRoot = "/mnt/hgfs/"
SANDBOX = ""
GAME = ""

-- Utilities

function tablelength(T)
    local count = 0
    for _ in pairs(T) do count = count + 1 end
    return count
end

function list_iter(t)
    local i = 0
    local n = tablelength(t)
    return function ()
        i = i + 1
        if i <= n then return t[i] end
    end
end

function evalCmdLine(cmdArg, minArgs, maxArgs)
    local listArgs = {};
    if tablelength(cmdArg) < minArgs then
        print("Too few arguments");
        for i = 1, tablelength(cmdArg), 1 do
            print(cmdArg[i]);
        end
        usage();
        return;
    elseif tablelength(cmdArg) >= minArgs then
        for i = 1, tablelength(cmdArg), 1 do
            if cmdArg[i] ~= nil then
                listArgs[i] = cmdArg[i];
            end
        end
        if tablelength(listArgs) > maxArgs then
            print("Too many arguments");
            for i = 1, tablelength(listArgs), 1 do
                print(listArgs[i]);
            end
        end
        usage();
        return;
    end
    return 0;
end

function loadParamFile()
    local buildParams = io.open("buildParams.ini", "r");
    if buildParams == nil then
        abort(" --- Error: unable to open buildParams.ini file!");
    end
    local params = {};
    local i = 1;
    for line in buildParams:lines() do
        if line ~= nil then
            params[i] = line;
            print(" -- parsed " .. params[i]);
            i = i + 1;
        end
    end
    return params;
end

function dumpOutput(fileHandle, message)
    -- dump command output to console
    if message ~= nil then
        print(" -- " .. message);
    end
    local result = 0;
    local errorFound = false;
    for line in fileHandle:lines() do
        if line ~= nil then
            if string.find(line, "()Error()") ~= nil then
                errorFound = true;
                result = 2;
            end
            print(" --- " .. line);
        end
    end
    return result;
end

function checkError(fileHandle)
    -- dump command output to console
    if message ~= nil then
        print(" -- " .. message);
    end
    local result = 0;
    local errorFound = false;
    for line in fileHandle:lines() do
        if line ~= nil then
            if string.find(line, "()Error()") ~= nil then
                errorFound = true;
                result = 2;
            end
            if errorFound == true then
                print(" --- " .. line);
                abort(" --- " .. line, result);
            end
        end
    end
    return result;
end

function abort(reason, code)
	print(reason);
        if VMSTARTED == true then
            stopVM();
        end
	return code, true;
end

function wait(seconds)
    if seconds == nil then
        print(" -- wait() - seconds is nil");
        return 29;
    end
    if DEBUG == true then
        print(" -- wait() : " .. seconds);
    end
    os.execute("sleep " .. tonumber(seconds));
    return 0;
end

-- build scripts

function dumbwait()
    program = ShareRoot .. "buildScripts/dumbwait";
    result = runScriptInVM("\"/bin/sh\"", "\"" .. program .. "\"");
end

function setupVMScripts(Sandbox, GameCore, Game, RepoPath)
    -- vm compile step
    local result = 0;
    local refresh = false;
    if NoRefresh ~= nil then
        refresh = NoRefresh;
    end
    if DEBUG == true then
        print(" -- setupVMScripts(" .. Sandbox .. ", " .. GameCore .. ", " .. Game .. ", " .. tostring(refresh) .. ")");
    else
        print(" -- setupVMScripts()");
    end
    program = ShareRoot .. "buildScripts/setupVM";
    if refresh then
        args = Sandbox .. " " .. GameCore .. " " .. Game .. " 1";
    else
        args = Sandbox .. " " .. GameCore .. " " .. Game;
    end
    local command = "\"" .. program .. " " .. args .. "\"";
    result = os.execute(command);

    return result, "setup VM scripts";
end

function copyGame(Sandbox, Game, NoRefresh)
    -- vm compile step
    local result = 0;
    local refresh = false;
    if NoRefresh ~= nil then
        refresh = NoRefresh;
    end
    if refresh == nil then
        refresh = false;
    end
    if DEBUG == true then
        print(" -- copyGame(" .. Sandbox .. ", " .. Game .. ", " .. tostring(refresh) .. ")");
    else
        print(" -- copyGame()");
    end
    program = ShareRoot .. "buildScripts/copyGame";
    if refresh then
        args = Sandbox .. " " .. Game .. " 1";
    else
        args = Sandbox .. " " .. Game;
    end
    local command = "\"" .. program .. " " .. args .. "\"";
    result = os.execute(command);

    return result, "copy game";
end

function copyGameCore(GameCore, Game, NoRefresh)
    -- vm compile step
    local result = 0;
    local refresh = false;
    if NoRefresh ~= nil then
        refresh = NoRefresh;
    end
    if refresh == nil then
        refresh = false;
    end
    if DEBUG == true then
        print(" -- copyGameCore(" .. GameCore .. ", " .. Game .. ", " .. tostring(refresh) .. ")");
    else
        print(" -- copyGameCore()");
    end
    program = ShareRoot .. "buildScripts/copyGameCore";
    if refresh then
        args = GameCore .. " " .. Game .. " 1";
    else
        args = GameCore .. " " .. Game;
    end
    local command = "\"" .. program .. " " .. args .. "\"";
    result = os.execute(command);

    return result, "copy game core";
end

function compileGraphics(Sandbox, GameCore, Game)
    -- vm compile step
    local result = 0;
    if DEBUG == true then
        print(" -- compileGraphics(" .. Sandbox .. ", " .. GameCore .. ", " .. Game .. ")");
    else
        print(" -- compileGraphics()");
    end
    program = ShareRoot .. "buildScripts/buildGraphics";
    args = Sandbox .. " " .. GameCore .. " " .. Game;
    local command = "\"" .. program .. " " .. args .. "\"";
    result = os.execute(command);

    return result, "compile graphics";
end

function compileInstall(Sandbox, GameCore, Game)
    -- vm compile step
    local result = 0;
    if DEBUG == true then
        print(" -- compileInstall(" .. Sandbox .. ", " .. GameCore .. ", " .. Game .. ")");
    else
        print(" -- compileInstall()");
    end
    program = ShareRoot .. "buildScripts/buildInstall";
    args = Sandbox .. " " .. GameCore .. " " .. Game;
    local command = "\"" .. program .. " " .. args .. "\"";
    result = os.execute(command);

    return result, "compile install";
end

function compileAllGames(Sandbox, GameCore, Game)
    -- vm compile step
    local result = 0;
    if DEBUG == true then
        print(" -- compileAllGames(" .. Sandbox .. ", " .. GameCore .. ", " .. Game .. ")");
    else
        print(" -- compileAllGames()");
    end
    program = ShareRoot .. "buildScripts/buildAllGames";
    args = Sandbox .. " " .. GameCore .. " " .. Game;
    local command = "\"" .. program .. " " .. args .. "\"";
    result = os.execute(command);

    return result, "compile game";
end

function publishGame(Sandbox, targetIP, gameName)
    -- vm publish game step
    local result = 0;
    if targetIP == nil then
        abort(" --- publishGame() - targetIP is nil!");
    end
    if gameName == nil then
        abort(" --- publishGame() - gameName is nil!");
    end
    if DEBUG == true then
        print(" -- publishGame(" .. targetIP .. ", " .. gameName .. ")");
    else
        print(" -- publishGame()");
    end
    program = ShareRoot .. "buildScripts/publish";
    args = targetIP .. " " .. gameName .. " " .. Sandbox;
    local command = "\"" .. program .. " " .. args .. "\"";
    result = os.execute(command);

    return result, "publish game";
end

function clean(Sandbox, Game)
    -- vm compile step
    local result = 0;
    if DEBUG == true then
        print(" -- clean(" .. Sandbox .. ", " .. Game .. ")");
    else
        print(" -- clean()");
    end
    program = ShareRoot .. "buildScripts/clean";
    args = Game .. " " .. Sandbox;
    local command = "\"" .. program .. " " .. args .. "\"";
    result = os.execute(command);

    return result;
end

function checkResult(result, message)
    if result ~= 0 then
        if message == nil then
            message = "unknown";
        end
        -- clean up and copy logs
        clean(SANDBOX, GAME);
        abort(" --- " .. message .. " step failed", result);
    end
end

-- Game build function

function buildGame(Path, Name, GameCore, Sandbox, VMName, Target)
    -- construct vm command
    local result = 0;
    local msg = "";
    if Path == nil then
        print(" -- buildGame() - Path is nil!");
        return 1;
    end
    if Name == nil then
        print(" -- buildGame() - Name is nil!");
        return 1;
    end
    if GameCore == nil then
        print(" -- buildGame() - GameCore is nil!");
        return 1;
    end
    if Sandbox == nil then
        print(" -- buildGame() - Sandbox is nil!");
        return 1;
    end
    if VMName == nil or VMName == "" then
        print(" -- buildGame() - VMName is nil, using " .. VMNAME);
    else
        VMNAME = VMName;
    end
    if Target == nil or Target == "" then
        print(" -- buildGame() - Target is nil, will not deploy game");
    end
    print(" -- building " .. Name .. " in " .. Sandbox .. " using VM " .. VMNAME);

    SANDBOX = Sandbox;
    GAME = Name;

    -- set up guest side scripts and logs
    result, msg = setupVMScripts(Sandbox, GameCore, Name, Path);
    checkResult(result, msg);

    -- copy game core
    result, msg = copyGameCore(GameCore, Name, "1");
    checkResult(result, msg);

    -- copy game
    result, msg = copyGame(Sandbox, Name);
    checkResult(result, msg);

    -- Compile the game
    result, msg = compileInstall(Sandbox, GameCore, Name);
    checkResult(result, msg);

    result, msg = compileGraphics(Sandbox, GameCore, Name);
    checkResult(result, msg);

    result, msg = compileAllGames(Sandbox, GameCore, Name);
    checkResult(result, msg);

    -- publish to EGM
    if Target ~= nil then
        result, msg = publishGame(Sandbox, Target, gameName);
        checkResult(result, msg);
    end

    clean(Sandbox, Name);

    return result;
end

function usage()
    print("Usage: ./(lua )BootBuildGame.lua <release path> <game name> <gamecore> <sandbox> <(optional)VM name> <(optional) deployment address>\n");
    print(" - If Lua is present in /bin then the interpreter does not need to be invoked on the command line.\n");
    print(" -- <release path>       the path to the source code to build.");
    print(" -- <game name>          the game name.");
    print(" -- <gamecore>           the gamecore CVS tag (core label).");
    print(" -- <sandbox>            the target sandbox environment.");
    print(" -- <VM name>            the target build environment.  Defaults to " .. VMNAME);
    print(" -- <deployment address> the target IP address to deploy the game to.");
end

-- execution
inArgs = loadParamFile();
if evalCmdLine(inArgs, 4, 6) ~= nil then
    path = inArgs[1];
    name = inArgs[2];
    gamecore = inArgs[3];
    sandbox = inArgs[4];
    vmname = inArgs[5];
    target = inArgs[6];
    if DEBUG == true then
        print(" -- DEBUG active");
    end
    if DEBUGCMD == true then
        print(" -- DEBUGCMD active");
    end
    result = buildGame(path, name, gamecore, sandbox, vmname, target);
    return result;
end
