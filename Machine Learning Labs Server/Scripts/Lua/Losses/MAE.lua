function CalculateLoss(input, target)
    return math.abs(input - target)
end

function CalculateLossDerivative(input, target)
    if input > target then
        return 1
    else
        return -1
    end
end