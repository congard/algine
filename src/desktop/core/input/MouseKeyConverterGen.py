from KeyConverterGen import *

# https://www.glfw.org/docs/3.3.1/group__buttons.html#ga3e2f2cf3c4942df73cc094247d275e74

glfwValuesSource = '''
GLFW_MOUSE_BUTTON_1
GLFW_MOUSE_BUTTON_2
GLFW_MOUSE_BUTTON_3
GLFW_MOUSE_BUTTON_4
GLFW_MOUSE_BUTTON_5
GLFW_MOUSE_BUTTON_6
GLFW_MOUSE_BUTTON_7
GLFW_MOUSE_BUTTON_8
GLFW_MOUSE_BUTTON_LEFT
GLFW_MOUSE_BUTTON_RIGHT
GLFW_MOUSE_BUTTON_MIDDLE
'''

valuesSource = '''
Key1
Key2
Key3
Key4
Key5
Key6
Key7
Key8
Left
Right
Middle
'''

gen(glfwValuesSource, valuesSource)
