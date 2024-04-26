import numpy as np
import skfuzzy as fuzz
from skfuzzy import control as ctrl

# Step 1: Create the universe and fuzzy variables
temperature = ctrl.Antecedent(np.arange(0, 101, 1), 'temperature')
velocity = ctrl.Consequent(np.arange(0, 101, 1), 'velocity')

# Step 2: Define membership functions
temperature['low'] = fuzz.trimf(temperature.universe, [0, 0, 50])
temperature['medium'] = fuzz.trimf(temperature.universe, [0, 50, 100])
temperature['high'] = fuzz.trimf(temperature.universe, [50, 100, 100])

velocity['low'] = fuzz.trimf(velocity.universe, [0, 0, 50])
velocity['medium'] = fuzz.trimf(velocity.universe, [0, 50, 100])
velocity['high'] = fuzz.trimf(velocity.universe, [50, 100, 100])

# Step 3: Define the rules
rule1 = ctrl.Rule(temperature['low'], velocity['low'])
rule2 = ctrl.Rule(temperature['medium'], velocity['medium'])
rule3 = ctrl.Rule(temperature['high'], velocity['high'])

# Step 4: Create and simulate a fuzzy control system
velocity_control = ctrl.ControlSystem([rule1, rule2, rule3])
velocity_simulation = ctrl.ControlSystemSimulation(velocity_control)

# Input temperature
input_temperature = float(input("Ingrese la nueva temperatura (celsius): "))
velocity_simulation.input['temperature'] = input_temperature

# Compute the result
velocity_simulation.compute()
output_velocity = velocity_simulation.output['velocity']

print(f"Nueva temperatura: {input_temperature}°C")
print(f"La nueva velocidad del ventilador es de: {output_velocity:.2f}")
