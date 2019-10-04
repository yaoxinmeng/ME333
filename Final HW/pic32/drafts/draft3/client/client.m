function client(port)
%   provides a menu for accessing PIC32 motor control functions
%
%   client(port)
%
%   Input Arguments:
%       port - the name of the com port.  This should be the same as what
%               you use in screen or putty in quotes ' '
%
%   Example:
%       client('/dev/ttyUSB0') (Linux/Mac)
%       client('COM3') (PC)
%
%   For convenience, you may want to change this so that the port is hardcoded.
   
% Opening COM connection
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

fprintf('Opening port %s....\n',port);

% settings for opening the serial port. baud rate 230400, hardware flow control
% wait up to 120 seconds for data before timing out
mySerial = serial(port, 'BaudRate', 230400, 'FlowControl', 'hardware','Timeout',120); 
% opens serial connection
fopen(mySerial);
% closes serial port when function exits
clean = onCleanup(@()fclose(mySerial));                                 

has_quit = false;
display_msg = ["     c: Read encoder (counts)     d: Read encoder(degs)" ...
               newline "     e: Reset encoder             q: Quit" ...
               newline "     r: Get mode"];
% menu loop
while ~has_quit
    fprintf('PIC32 MOTOR DRIVER INTERFACE\n\n');
    % display the menu options; this list will grow
    fprintf('%s',display_msg);
    % read the user's choice
    selection = input('\nENTER COMMAND: ', 's');
     
    % send the command to the PIC32
    fprintf(mySerial,'%c\n',selection);
    
    % take the appropriate action
    switch selection
        case 'c'                         % Read encoder (counts)
            n = fscanf(mySerial,'%d');   % get the encoder count from PIC32
            fprintf('The motor angle is %d counts.\n',n);     % print it to the screen
        case 'd'
            n = fscanf(mySerial,'%f');   % get the encoder count from PIC32
            fprintf('The motor angle is %f degrees.\n',n);     % print it to the screen
        case 'e'
            fprintf('Motor angle is reset to 0 degrees.\n');
        case 'q'
            has_quit = true;             % exit client
        case 'r'                         % get mode
            % display the mode options
            fprintf('%s','\n    0: IDLE     1:PWM\n     2: ITEST     3:HOLD\n     4: TRACK\n');
            
            selection = input('\nENTER MODE: ', 's');   % read the user's choice
            fprintf(mySerial,'%d\n',selection);         % send the command to the PIC32
            fprintf('%s mode is selected.\n', mode_name(selection)); % prints the command to screen
        otherwise
            fprintf('Invalid Selection %c\n', selection);
    end
end

end
