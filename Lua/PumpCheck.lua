commandArray = {}
if (os.time()-tonumber(uservariables['PumpTime'])) > 3 and uservariables['PumpError'] == '' then
    if uservariables['PumpState']==1 and otherdevices["MotorCheck"] == 'Closed' then 
        commandArray['Variable:PumpError']='Pump no power failure (Pump still OFF)';
        print('Pump no power failure (Pump still OFF)');
        commandArray['Pump']='Off'
    elseif uservariables['PumpState']==0 and otherdevices["MotorCheck"] == 'Open' then 
        commandArray['Variable:PumpError']='Pump relay jam failure (Pump still ON)';
        print('Pump relay jam failure (Pump still ON)')
        commandArray['Pump']='Off'        
    end
end

    print('!!!'..otherdevices['Pump'].."/"..uservariables['PumpState'].."/"..(os.time()-tonumber(uservariables['PumpTime'])))
-- check state of device
if otherdevices['Pump'] == 'On' and uservariables['PumpState']==0 then
    commandArray['Variable:PumpTime']=tostring(os.time());
    commandArray['Variable:PumpState']='1';
    print('Pump is ON')
    commandArray['Variable:PumpError']=''
    
elseif otherdevices['Pump'] == 'Off' and uservariables['PumpState']==1 then
    commandArray['Variable:PumpTime']=tostring(os.time());
    commandArray['Variable:PumpState']='0';
    print('Pump is OFF')
    commandArray['Variable:PumpError']=''    
end

return commandArray
