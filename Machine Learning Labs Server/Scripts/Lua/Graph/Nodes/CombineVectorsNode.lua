--lua json library
local json = require("json")

function Init(__NodePtr__)
	print("Init")
	Node_MakeInput(__NodePtr__,0, "Input", "double", "[]")
	Node_MakeOutput(__NodePtr__,0, "Output", "double", "[]")
	--[[Node_MakeAttribute(__NodePtr__,0, function ()
		Node_ImGuiPushItemWidth(__NodePtr__,300) 
		Node_ImGuiInputInt(__NodePtr__, "Input", 0)
	end)]]--
end

function Process(__NodePtr__, forward)	
	if forward then
        local input = Node_GetInputDataByIndex(__NodePtr__,0)
        print("Input: "..input)
        local inputTable = json.decode(input)
        --flatten table
        local outputTable = {}
        for i=1,#inputTable do
            for j=1,#inputTable[i] do
                table.insert(outputTable, inputTable[i][j])
            end
        end
        local output = json.encode(outputTable)
		print("Output: "..output)
		Node_SetOutputDataByIndex(__NodePtr__,0, output)
	else
		local input = Node_GetOutputDataByIndex(__NodePtr__,0)
		Node_SetInputDataByIndex(__NodePtr__,0, input)
	end
end