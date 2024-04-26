import numpy as np
import skfuzzy as fuzz

class Node:
    def __init__(self, name, type_):
        self.name = name
        self.type = type_
        self.inputs = {}
        self.output = None
        self.certainty = None

    def set_input(self, value, port, certainty):
        self.inputs[port] = (value, certainty)

    def compute_output(self):
        # Generate a universe of discourse for certainty factor (0 to 1)
        universe = np.linspace(0, 1, 101)
        
        if self.type == 'adder':
            values, certainties = zip(*self.inputs.values())
            self.output = np.sum(values)
            # Start with a certainty of 1 and reduce it using fuzzy AND for each input certainty
            aggregate_certainty = np.ones_like(universe)
            for certainty in certainties:
                temp_certainty = np.ones_like(universe) * certainty
                aggregate_certainty = fuzz.fuzzy_and(universe, aggregate_certainty, universe, temp_certainty)[1]
            self.certainty = aggregate_certainty.min()
            
        elif self.type == 'multiplier':
            values, certainties = zip(*self.inputs.values())
            self.output = np.prod(values)
            # Start with a certainty of 0 and increase it using fuzzy OR for each input certainty
            aggregate_certainty = np.zeros_like(universe)
            for certainty in certainties:
                temp_certainty = np.ones_like(universe) * certainty
                aggregate_certainty = fuzz.fuzzy_or(universe, aggregate_certainty, universe, temp_certainty)[1]
            self.certainty = aggregate_certainty.max()

        return self.output

class Circuit:
    def __init__(self):
        self.nodes = {}
        self.connections = []

    def add_node(self, name, type_):
        self.nodes[name] = Node(name, type_)

    def add_connection(self, from_node, to_node, port):
        self.connections.append((from_node, to_node, port))

    def request_value(self, node):
        value = float(input(f"Please enter a value for node {node}: "))
        certainty = float(input("Enter Certainty Factor for node (0.0 to 1.0): "))
        certainty = max(0.0, min(1.0, certainty))  # Clamp certainty to [0.0, 1.0]
        return value, certainty

    def evaluate(self, node):
        if self.nodes[node].type == 'input':
            value, certainty = self.request_value(node)
            self.nodes[node].set_input(value, 1, certainty)
            self.nodes[node].output = value  # Direct input to output for input nodes
            self.nodes[node].certainty = certainty
        else:
            for from_node, to_node, port in self.connections:
                if to_node == node and from_node not in self.nodes[node].inputs:
                    self.evaluate(from_node)
                    output = self.nodes[from_node].output
                    certainty = self.nodes[from_node].certainty
                    self.nodes[to_node].set_input(output, port, certainty)
            self.nodes[node].compute_output()

    def query_node(self):
        node = input("Which node are you interested in? (a-j or q to quit): ")
        if node == 'q':
            return False
        if node in self.nodes:
            self.evaluate(node)
            print(f"The output value at node {node} is {self.nodes[node].output} with a certainty of {self.nodes[node].certainty}")
        else:
            print("Node not found.")
        return True

# Example usage
circuit = Circuit()
# Adding nodes and their types
nodes_info = {
    'a': 'input', 'b': 'input', 'c': 'input', 'd': 'input',
    'e': 'adder', 'f': 'adder', 'g': 'multiplier',
    'i': 'output', 'j': 'output'
}
for node, type_ in nodes_info.items():
    circuit.add_node(node, type_)

# Adding connections
connections_info = [
    ('a', 'f', 1), ('b', 'e', 1), ('c', 'e', 2), ('d', 'g', 2),
    ('e', 'f', 2), ('e', 'g', 1), ('f', 'i', 1), ('g', 'j', 1)
]
for from_node, to_node, port in connections_info:
    circuit.add_connection(from_node, to_node, port)

# Interactive query
while circuit.query_node():
    pass
