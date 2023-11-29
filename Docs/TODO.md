- [ ] Fix the Layer node weight resizing problem on backward pass
- [ ] Fix the backward pass for the self attention layer
- [ ] Fix the scripting node languages (chai, python, java, JS, lua, angel script)
	- [ ] Chai (create)
	- [ ] python (create)
	- [ ] java (create)
	- [ ] js (Update)
	- [ ] lua (update)
	- [ ] angel (create)
- [ ] Move GPU calls To DynamicCodeExecutionEngine so that it can be used across all nodes without having to be redefined
- [ ] Fix/Make the data transfer between the nodes to be more consistent
	- [ ] utility functions

- [ ] Fix the thread conflict when running constant train or forward
- [x] Fix serialization