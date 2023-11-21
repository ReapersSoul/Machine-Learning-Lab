function Init(__NodePTR__){
    Node_MakeInput(__NodePTR__,0, "Input", "double","[]");
    Node_MakeOutput(__NodePTR__,0, "Output", "double","[]");
    print("JSNode Init");
}


function Process(__NodePTR__,forward){
    if(forward==true){
        var input = JSON.parse(Node_GetInputDataByIndex(__NodePTR__,0));
        print("JSNode Process");
        Node_SetOutputDataByIndex(__NodePTR__,0,JSON.stringify(input[0]));
    }else{
        var output = JSON.parse(Node_GetOutputDataByIndex(__NodePTR__,0));
        print("JSNode Process Backward");
        Node_SetInputDataByIndex(__NodePTR__,0,JSON.stringify(output[0]));
    }
}