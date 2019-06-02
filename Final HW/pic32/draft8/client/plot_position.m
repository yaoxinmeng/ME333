function data = plot_position(mySerial)
    nsamples = fscanf(mySerial,'%d');  % first get the number of samples being sent
    
    MOTOR_SERVO_RATE = 200;         % 200 Hz motion control loop
    data = zeros(nsamples,2);       % two values per sample:  ref and actual
    times = zeros(nsamples);        % x-axis
    
    for i=1:nsamples
        data(i,:) = fscanf(mySerial,'%f %f');           % read in data (degs) from PIC32; 
        times(i) = (i-1) * 1000 / MOTOR_SERVO_RATE;     % time step in ms
    end
    if nsamples > 1						        
        plot(times,data(:,1),times,data(:,2));            % plot the reference and actual
    else
        fprintf('Only 1 sample received\n')
        disp(data);
    end
    % compute the average error
    score = mean(abs(data(:,1)-data(:,2)));
    fprintf('\nAverage error: %5.1f degrees\n',score);
    % label the plot
    str = sprintf('%d samples at %7.2f Hz taking %5.3f sec', ...
	      nsamples, MOTOR_SERVO_RATE, nsamples/MOTOR_SERVO_RATE);
    title(str);
    ylabel('Motor angle (degrees)');
    xlabel('Time (ms)');
    legend('Reference','Measured');
end

