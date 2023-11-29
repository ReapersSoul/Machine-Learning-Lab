function Activate(input)
    return input * 0.5 * (1 + math.tanh(math.sqrt(2 / math.pi) * (input + 0.044715 * math.pow(input, 3))))
end

function ActivateDerivative(input)
    return 0.5 * (1 + math.tanh(math.sqrt(2 / math.pi) * (input + 0.044715 * math.pow(input, 3)))) + input * 0.5 * (1 - math.pow(math.tanh(math.sqrt(2 / math.pi) * (input + 0.044715 * math.pow(input, 3))), 2)) * math.sqrt(2 / math.pi) * (1 + 0.134145 * math.pow(input, 2))
end