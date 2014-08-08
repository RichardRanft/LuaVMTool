--#!/bin/lua 
 -- Language: Lua

DEBUG = true;
DEBUGCMD = false;
VMNAME = "vmware/AlphaBuildEnv/SB_5.00.00.002.vmx";
VMSTARTED = false;
ShareRoot = "/mnt/hgfs/"

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
        for i = 1, tablelength(arg), 1 do
            print(cmdArg[i]);
        end
        usage();
        return;
    end
    if tablelength(cmdArg) >= minArgs then
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
            usage();
            return;
        end
    end
    return 0;
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
	os.exit(code, true);
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

-- VMWare wrapper functions

function startVM()
    -- vm start step
    local result = 0;
    command = "vmrun -T ws start " .. VMNAME .. " nogui";
    infile = io.popen(command, "r");
    if infile == nil then
        abort(" -- startVM() - unable to execute vmrun command!", 1);
    else
        if DEBUG == true then
            result = dumpOutput(infile, "startVM()");
            if DEBUGCMD == true then
                print(" -- " .. command);
            end
        else
            result = checkError(infile);
        end
    end
    exitCode, exitSignal = infile:close();
    if tonumber(exitCode) ~= nil then
        print(" --- error executing startVM() - " .. exitCode .. " : " .. exitSignal);
    end
    VMSTARTED = true;
    return result;
end

function stopVM()
    -- vm start step
    local result = 0;
    command = "vmrun -T ws stop " .. VMNAME .. " soft";
    infile = io.popen(command, "r");
    if infile == nil then
        abort(" -- stopVM() - unable to execute vmrun command!", 1);
    else
        if DEBUG == true then
            result = dumpOutput(infile, "stopVM()");
            if DEBUGCMD == true then
                print(" -- " .. command);
            end
        else
            result = checkError(infile);
        end
    end
    exitCode, exitSignal = infile:close();
    if tonumber(exitCode) ~= nil then
        print(" --- error executing stopVM() - " .. exitCode .. " : " .. exitSignal);
    end
    VMSTARTED = false;
    return result;
end

function runScriptInVM(interpreter, scriptText)
    -- runs script in guest
    local result = 0;
    command = "vmrun -T ws -gu root -gp ballytech runScriptInGuest " .. VMNAME .. " " .. interpreter .. " " .. scriptText;
    infile = io.popen(command, "r");
    if infile == nil then
        abort(" -- runScriptInVM() - unable to execute vmrun command!", 1);
    else
        if DEBUG == true then
            result = dumpOutput(infile, "runScriptInVM(" .. interpreter .. ", " .. scriptText .. ")");
            if DEBUGCMD == true then
                print(" -- " .. command);
            end
        else
            result = checkError(infile);
        end
    end
    exitCode, exitSignal = infile:close();
    if tonumber(exitCode) ~= nil then
        print(" --- error executing runScriptInVM() - " .. exitCode .. " : " .. exitSignal);
    end
    return result;
end

function runProgramInVM(program, args)
    -- runs script in guest
    local result = 0;
    command = "vmrun -T ws -gu root -gp ballytech runProgramInGuest " .. VMNAME .. " " .. program .. " " .. args;
    infile = io.popen(command, "r");
    if infile == nil then
        abort(" -- runProgramInVM() - unable to execute vmrun command!", 1);
    else
        if DEBUG == true then
            result = dumpOutput(infile, "runProgramInVM(" .. program .. ", " .. args .. ")");
            if DEBUGCMD == true then
                print(" -- " .. command);
            end
        else
            result = checkError(infile);
        end
    end
    exitCode, exitSignal = infile:close();
    if tonumber(exitCode) ~= nil then
        print(" --- error executing runScriptInVM() - " .. exitCode .. " : " .. exitSignal);
    end
    return result;
end

function createFolderInVM(path)
    local result = 0;
    command = "vmrun -T ws -gu root -gp ballytech createDirectoryInGuest " .. VMNAME .. " " .. path;
    infile = io.popen(command, "r");
    if infile == nil then
        abort(" -- createFolderInVM() - unable to execute vmrun command!", 1);
    else
        if DEBUG == true then
            result = dumpOutput(infile, "createFolderInVM(" .. path .. ")");
            if DEBUGCMD == true then
                print(" -- " .. command);
            end
        else
            result = checkError(infile);
        end
    end
    exitCode, exitSignal = infile:close();
    if tonumber(exitCode) ~= nil then
        print(" --- error executing createFolderInVM() - " .. exitCode .. " : " .. exitSignal);
    end
    return result;
end

function removeFolderInVM(path)
    local result = 0;
    command = "vmrun -T ws -gu root -gp ballytech deleteDirectoryInGuest " .. VMNAME .. " " .. path;
    infile = io.popen(command, "r");
    if infile == nil then
        abort(" -- removeFolderInVM() - unable to execute vmrun command!", 1);
    else
        if DEBUG == true then
            result = dumpOutput(infile, "removeFolderInVM(" .. path .. ")");
            if DEBUGCMD == true then
                print(" -- " .. command);
            end
        else
            result = checkError(infile);
        end
    end
    exitCode, exitSignal = infile:close();
    if tonumber(exitCode) ~= nil then
        print(" --- error executing removeFolderInVM() - " .. exitCode .. " : " .. exitSignal);
    end
    return result;
end

function addSharedFolder(pathOnGuest, pathOnHost)
    local result = 0;
    command = "vmrun -T ws -gu root -gp ballytech addSharedFolder " .. VMNAME .. " " .. pathOnGuest .. " " .. pathOnHost;
    infile = io.popen(command, "r");
    if infile == nil then
        abort(" -- addSharedFolder() - unable to execute vmrun command!", 1);
    else
        if DEBUG == true then
            result = dumpOutput(infile, "addSharedFolder(" .. pathOnGuest .. ", " .. pathOnHost .. ")");
            if DEBUGCMD == true then
                print(" -- " .. command);
            end
        else
            result = checkError(infile)
        end
    end
    exitCode, exitSignal = infile:close();
    if tonumber(exitCode) ~= nil then
        print(" --- error executing addSharedFolder() - " .. exitCode .. " : " .. exitSignal);
    end
    return result;
end

function removeSharedFolder(pathOnGuest)
    local result = 0;
    command = "vmrun -T ws -gu root -gp ballytech removeSharedFolder " .. VMNAME .. " " .. pathOnGuest;
    infile = io.popen(command, "r");
    if infile == nil then
        abort(" -- removeSharedFolder() - unable to execute vmrun command!", 1);
    else
        if DEBUG == true then
            result = dumpOutput(infile, "addSharedFolder(" .. pathOnGuest .. ")");
            if DEBUGCMD == true then
                print(" -- " .. command);
            end
        else
            result = checkError(infile);
        end
    end
    exitCode, exitSignal = infile:close();
    if tonumber(exitCode) ~= nil then
        print(" --- error executing removeSharedFolder() - " .. exitCode .. " : " .. exitSignal);
    end
    return result;
end

function copyFileToGuest(pathOnHost, pathOnGuest)
    local result = 0;
    command = "vmrun -T ws -gu root -gp ballytech copyFileFromHostToGuest " .. VMNAME .. " " .. pathOnHost .. " " .. pathOnGuest;
    infile = io.popen(command, "r");
    if infile == nil then
        abort(" -- copyFileToGuest() - unable to execute vmrun command!", 1);
    else
        if DEBUG == true then
            result = dumpOutput(infile, "copyFileToGuest(" .. pathOnHost .. ", " .. pathOnGuest .. ")");
            if DEBUGCMD == true then
            print(" -- " .. command);
            end
        else
            result = checkError(infile);
        end
    end
    exitCode, exitSignal = infile:close();
    if tonumber(exitCode) ~= nil then
        print(" --- error executing copyFileToGuest() - " .. exitCode .. " : " .. exitSignal);
    end
    return result;
end

function copyFileToHost(pathOnGuest, pathOnHost)
    local result = 0;
    command = "vmrun -T ws -gu root -gp ballytech copyFileFromGuestToHost " .. VMNAME .. " " .. pathOnGuest .. " " .. pathOnHost;
    infile = io.popen(command, "r");
    if infile == nil then
        abort(" -- copyFileToHost() - unable to execute vmrun command!", 1);
    else
        if DEBUG == true then
            result = dumpOutput(infile, "copyFileToHost(" .. pathOnGuest .. ", " .. pathOnHost .. ")");
            if DEBUGCMD == true then
            print(" -- " .. command);
            end
        else
            result = checkError(infile);
        end
    end
    exitCode, exitSignal = infile:close();
    if tonumber(exitCode) ~= nil then
        print(" --- error executing copyFileToHost() - " .. exitCode .. " : " .. exitSignal);
    end
    return result;
end

function listDirectoryInGuest(pathOnGuest)
    local result = 0;
    command = "vmrun -T ws -gu root -gp ballytech listDirectoryInGuest " .. VMNAME .. " " .. pathOnGuest;
    infile = io.popen(command, "r");
    if infile == nil then
        abort(" -- listDirectoryInGuest() - unable to execute vmrun command!", 1);
    else
        for line in infile:lines() do
            if line ~= nil then
                print(" --- " .. line);
            end
        end
        if DEBUG == true then
            result = dumpOutput(infile, "listDirectoryInGuest(" .. pathOnGuest .. ")");
            if DEBUGCMD == true then
                print(" -- " .. command);
            end
        else
            result = checkError(infile);
        end
    end
    exitCode, exitSignal = infile:close();
    if tonumber(exitCode) ~= nil then
        print(" --- error executing listDirectoryInGuest() - " .. exitCode .. " : " .. exitSignal);
    end
    return result;
end

-- build scripts

function dumbwait()
    program = ShareRoot .. "buildScripts/dumbwait";
    result = runScriptInVM("\"/bin/sh\"", "\"" .. program .. "\"");
end

function compile(Sandbox, GameCore, Game)
    -- vm compile step
    local result = 0;
    if DEBUG == true then
        print(" -- compile(" .. Sandbox .. ", " .. GameCore .. ", " .. Game .. ")");
    end
    program = ShareRoot .. "buildScripts/buildgame";
    args = Sandbox .. " " .. GameCore .. " " .. Game;
    result = runScriptInVM("\"/bin/sh\"", "\"" .. program .. " " .. args .. "\"");

    return result;
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
    end
    program = ShareRoot .. "buildScripts/publish";
    args = targetIP .. " " .. gameName .. " " .. Sandbox;
    result = runScriptInVM("\"/bin/sh\"", "\"" .. program .. " " .. args .. "\"");

    return result;
end

function clean(Sandbox, Game)
    -- vm compile step
    local result = 0;
    if DEBUG == true then
        print(" -- clean(" .. Sandbox .. ", " .. Game .. ")");
    end
    program = ShareRoot .. "buildScripts/clean";
    args = Game .. " " .. Sandbox;
    result = runScriptInVM("\"/bin/sh\"", "\"" .. program .. " " .. args .. "\"");

    return result;
end

-- Game build function

function buildAlphaGame(Path, Name, GameCore, Sandbox, VMName, Target)
    -- construct vm command
    local result = 0;
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

    -- Shut down in case it was left running
    stopVM();

    wait(10);

    -- Start the VM
    result = startVM();

    if result ~= 0 then
        abort(" --- VM failed to start", result);
    end

    -- Give it time to fire up
    wait(30);

    -- Compile the game
    result = compile(Sandbox, GameCore, Name);

    if result ~= 0 then
        -- clean up and copy logs
        clean(Sandbox, Name);
        abort(" --- compile step failed", result);
    end

    -- publish to EGM
    if Target ~= nil then
        publishGame(Sandbox, Target, gameName);
    end

    -- clean up and copy logs
    clean(Sandbox, Name);

    -- Shut down
    result = stopVM();

    return result;
end

function usage()
    print("Usage: ./(lua )VAlphaBuildGame.lua <release path> <game name> <gamecore> <sandbox> <(optional)VM name> <(optional) deployment address>\n");
    print(" - If Lua is present in /bin then the interpreter does not need to be invoked on the command line.\n");
    print(" -- <release path>       the path to the source code to build.");
    print(" -- <game name>          the game name.");
    print(" -- <gamecore>           the gamecore CVS tag (core label).");
    print(" -- <sandbox>            the target sandbox environment.");
    print(" -- <VM name>            the target build environment.  Defaults to " .. VMNAME);
    print(" -- <deployment address> the target IP address to deploy the game to.");
end

-- execution
--if evalCmdLine(arg, 4, 6) ~= nil then
--    path = arg[1];
--    name = arg[2];
--    gamecore = arg[3];
--    sandbox = arg[4];
--    vmname = arg[5];
--    target = arg[6];
--    if DEBUG == true then
--        print(" -- DEBUG active");
--    end
--    if DEBUGCMD == true then
--        print(" -- DEBUGCMD active");
--    end
--    result = buildGame(path, name, gamecore, sandbox, vmname, target);
--    os.exit(result);
--end
