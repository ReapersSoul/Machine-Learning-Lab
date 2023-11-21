--lua json library
local json = require("json")

function Init(__NodePtr__)
	print("Init")
	Node_MakeOutput(__NodePtr__,0, "Output", "double", "0.0")
	--[[Node_MakeAttribute(__NodePtr__,0, function ()
		Node_ImGuiPushItemWidth(__NodePtr__,300) 
		Node_ImGuiInputInt(__NodePtr__, "Input", 0)
	end)]]--
end

function Process(__NodePtr__, forward)	
	if forward then
        local arr={}
        arr[1]=1
        arr[2]=2
        arr[3]=3
        arr[4]=4

		local output = json.encode(arr)
		print("Output: "..output)
		Node_SetOutputDataByIndex(__NodePtr__,0, output)
	else
		local input = Node_GetOutputDataByIndex(__NodePtr__,0)
		Node_SetInputDataByIndex(__NodePtr__,0, input)
	end
end