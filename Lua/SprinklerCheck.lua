commandArray = {}
if (os.time()-tonumber(uservariables['SprinklerTime'])) > 3 and uservariables['SprinklerError'] == '' then
    if uservariables['SprinklerState']==1 and otherdevices["MotorCheck"] == 'Closed' then 
        commandArray['Variable:SprinklerError']='Sprinkler no power failure (sprinkler still OFF)';
        print('Sprinkler no power failure (sprinkler still OFF)');
    elseif uservariables['SprinklerState']==0 and otherdevices["MotorCheck"] == 'Open' then 
        commandArray['Variable:SprinklerError']='Sprinkler relay jam failure (sprinkler still ON)';
        print('Sprinkler relay jam failure (sprinkler still ON)')
    end
end

-- check state of device
if otherdevices['Sprinkler'] == 'On' and uservariables['SprinklerState']==0 then
    commandArray['Variable:SprinklerTime']=tostring(os.time());
    commandArray['Variable:SprinklerState']='1';
    print('Sprinkler is ON')
    commandArray['Variable:SprinklerError']=''
    
elseif otherdevices['Sprinkler'] == 'Off' and uservariables['SprinklerState']==1 then
    commandArray['Variable:SprinklerTime']=tostring(os.time());
    commandArray['Variable:SprinklerState']='0';
    print('Sprinkler is OFF')
    commandArray['Variable:SprinklerError']=''    
end

return commandArray
