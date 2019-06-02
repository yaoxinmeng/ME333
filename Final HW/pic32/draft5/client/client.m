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
display_msg = [newline "     a: Read current sensor (ADC counts)    b: Read current sensor (mA)" ...
               newline "     c: Read encoder (counts)               d: Read encoder(degs)" ...
               newline "     e: Reset encoder                       f: Set PWM (-100 to 100)" ...
               newline "     p: Unpower the motor                   q: Quit" ...
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
        case 'a'    % Read current (counts)
            n = fscanf(mySerial,'%d');                      % get the current count from PIC32
            fprintf('The current is %d ADC counts.\n',n);   % print it to the screen
        case 'b'    % Read current (mA)
            f = fscanf(mySerial,'%f');              % get the current from PIC32
            fprintf('The current is %f mA.\n',f);   % print it to the screen
        case 'c'    % Read encoder (counts)
            n = fscanf(mySerial,'%d');                      % get the encoder count from PIC32
            fprintf('The motor angle is %d counts.\n',n);   % print it to the screen
        case 'd'    % Read encoder (degs)
            f = fscanf(mySerial,'%f');                      % get the encoder count from PIC32
            fprintf('The motor angle is %f degrees.\n',f);  % print it to the screen
        case 'e'    % Reset encoder
            fprintf('Motor angle is reset to 0 degrees.\n');
        case 'f'    % Set PWM
            selection = input('\nENTER PWM: ', 'f');        % read the user's choice
            fprintf(mySerial,'%f\n',selection);             % send the command to the PIC32
        case 'p'    % Unpower the motor
            fprintf('Motor is unpowered.\n');   
        case 'q'    % IDLE mode and quit
            has_quit = true;    % exit client
        case 'r'    % get mode
            f = fscanf(mySerial,'%d');              % get the mode (int) from PIC32
            mode_name(f);                           % executes command
        otherwise
            fprintf('Invalid Selection %c\n', selection);
    end
end

end
