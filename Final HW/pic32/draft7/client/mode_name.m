function mode_name(mode)
switch mode
    case 0
        fprintf('IDLE mode is selected.\n');
    case 1
        fprintf('PWM mode is selected.\n');
    case 2
        fprintf('ITEST mode is selected.\n');
    case 3
        fprintf('HOLD mode is selected.\n');
    case 4
        fprintf('TRACK mode is selected.\n');
    otherwise
        fprintf('Invalid Mode %d.\n', mode);
end

end

