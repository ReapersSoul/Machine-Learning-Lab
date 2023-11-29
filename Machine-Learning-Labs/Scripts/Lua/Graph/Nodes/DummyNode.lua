--lua json library
local json = require("json")

function Init(__NodePtr__)
	print("Init")
	Node_MakeInput(__NodePtr__,0, "Input", "double", 0.0)
	Node_MakeOutput(__NodePtr__,0, "Output", "double", 0.0)
	Node_MakeOutput(__NodePtr__,1, "Output", "double", 0.0)
	--[[Node_MakeAttribute(__NodePtr__,0, function ()
		Node_ImGuiPushItemWidth(__NodePtr__,300) 
		Node_ImGuiInputInt(__NodePtr__, "Input", 0)
	end)]]--
end

function printTable(table)
	for k,v in pairs(table) do
		if type(v) == "table" then
			print(k)
			printTable(v)
		else
			print(k,v)
		end
	end
end

function Process(__NodePtr__, forward)	
	if forward then
		local input = Node_GetInputDataByIndex(__NodePtr__,0)
		print("Input: "..input)
		--parse json
		local jsonInput = json.decode(input)
		--convert back to string
		local output = json.encode(jsonInput[1] * jsonInput[2])
		print("Output: "..output)
		Node_SetOutputDataByIndex(__NodePtr__,0, output)
	else
		local input = Node_GetOutputDataByIndex(__NodePtr__,0)
		Node_SetInputDataByIndex(__NodePtr__,0, input)
	end
end