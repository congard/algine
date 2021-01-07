def gen(glfwValuesSource: str, valuesSource: str):
    glfwValues = glfwValuesSource.split("\n")
    del glfwValues[0]
    del glfwValues[-1]

    values = valuesSource.split("\n")
    del values[0]
    del values[-1]

    assert len(glfwValues) == len(values)

    macros = ['glfwToInternal', 'internalToGlfw']

    for macro in macros:
        for i in range(len(values)):
            print(macro + '(' + values[i] + ', ' + glfwValues[i] + ');')

        print()
