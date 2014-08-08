--[[
This is a test/tutorial script for using vmtool to run a VMware virtual machine.
Refer to the comments for information pertaining to the relevant sections.
]]

-- utility functions
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

function getOS()
	local separator = package.config:sub(1,1);
	if separator == "/" then 
		return "Linux";
	end
	return "Windows";
end

-- test script
print(" ---- start");
if CVMInterface == nil then
    print(" -- CVMInterface is not available");
else
    print(" -- CVMInterface is available");
    -- create our VM Interface.
    VMInterface = CVMInterface();
    -- set up user name and password
    VMInterface.setHostUserName("sgp1000");
    VMInterface.setHostPassword("7w34034g354q634");
    VMInterface.setUserName("root");
    VMInterface.setPassword("ballytech");
    -- start our VM
	if getOS() == "Linux" then
		--VMInterface.startVM("/home/sgp1000/vmware/AlphaBuildEnv/SB_5.00.00.002");
		VMInterface.startVM("/home/sgp1000/vmware/OpenSuSE_12_3_x64/OpenSuSE_12_3_x64.vmx");
	else
		VMInterface.startVM("C:\\VMShare\\openSUSE_12.3x64_KDE_4_desktopv0.0.1-0.0.1\\openSUSE_12.3x64_KDE_4_desktopv0.0.1.x86_64-0.0.1.vmx");
	end
    -- set the tools timeout to 60 seconds
    VMInterface.setTimeout(60);
    -- check for the guest tools in the VM
    result = VMInterface.waitForTools();
    if result == 0 then
        -- tools are up, log in
        VMInterface.login();
        -- test running script text
        local script = "ls &> /mnt/hgfs/buildOutput/test1.txt"
        VMInterface.runScriptInVM("/bin/sh", script);
        -- test running an executable file
        VMInterface.runProgramInVM("sh /mnt/hgfs/buildScripts/test.sh", "testline\n");
        -- test getting a directory listing
        local fileList = VMInterface.listDirectoryInGuest("/");
        for i = 1, tablelength(fileList) do
            print(" -- file: " .. fileList[i]);
        end
    else
        print(" -- VM Guest tools not responding...");
    end
    -- set timeout back to default (5 min)
    VMInterface.setTimeout(300);
    -- log out
    VMInterface.logout();
    -- shut down
    VMInterface.stopVM();
end
print(" ---- end");
