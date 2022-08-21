# mircat
Matlab MEX wrapper written in C/C++ (MS Visual Studio) for DaylightSolutions MIRcat QCL laser (https://daylightsolutions.com/product/mircat/)

Requires DaylightSolutions SDK, which is provided with the laser (MIRcatSDK.lib and MIRcatSDK.h)

Syntaxis:
~~~Matlab
mircat('setParameterCommand',value); 	% set Parameter to provided value or execute command
result = mircat('getParameterCommand');	% get Parameter value or get command output
~~~

| Parameters/Commands:		| Call type |
| :---						| :----:	|
| arm						| get only	|
| disarm					| get only	|
| emission					| set&get	|
| isarmed					| get only	|
| isconnected				| get only	|
| isinterlocked				| get only	|
| istuned					| get only	|
| poweroff					| get only	|
| stepscan					| set only	|
| temperature				| get only	|
| temperaturestable			| get only	|
| temperaturewait			| get only	|
| wavelength				| get only	|
| wavelength_cm1			| set only	|
| wavelength_mic			| set only	|
| wavelength_trigger		| set only	|


**Example:**
~~~Matlab
mircat('isconnected')                % initialize the driver and check connection to the laser`
mircat('isinterlocked')              % check the status of the laser interlock

if ~mircat('isarmed')                % check if the laser is armed
	mircat('arm');                   % arm the laser
	while ~mircat('isarmed')         % wait till arming process is finished
		pause(0.5);
	end
end

mircat('temperature')                % check the laser temperature
mircat('temperaturestable')          % check if the laser temperature is is stable

mircat('wavelength_mic',lambda_um);  % set emission wavelength to lambda_um (in microns)

mircat('emission',1);                % emission ON
mircat('emission',0);                % emission OFF

mircat('poweroff');                  % power off the laser
clear('mircat');                     % clear driver from memory
~~~