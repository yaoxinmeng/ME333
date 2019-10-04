function msg = mode_name(mode)
switch mode
    case 0
        msg = "IDLE";
    case 1
        msg = "PWM";
    case 2
        msg = "ITEST";
    case 3
        msg = "HOLD";
    case 4
        msg = "TRACK";
    otherwise
        msg = "UNRECOGNISED";
end
end

